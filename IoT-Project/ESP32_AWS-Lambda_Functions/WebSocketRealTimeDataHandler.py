import json
import boto3
import os
import time
from concurrent.futures import ThreadPoolExecutor

# Initialize AWS services
dynamodb = boto3.resource('dynamodb')
connections_table = dynamodb.Table('WebsocketConnections')
apigateway = boto3.client(
    'apigatewaymanagementapi',
    endpoint_url=os.environ['WEBSOCKET_ENDPOINT']
)

def lambda_handler(event, context):
    """
    Processes DynamoDB stream events and broadcasts sensor data to all connected WebSocket clients.
    """
    try:
        print("Raw event:", json.dumps(event))
        
        if "Records" not in event:
            return {"statusCode": 400, "body": "No records found"}
            
        processed_records = 0
        successful_sends = 0
        failed_sends = 0
        stale_connections = 0
        
        # Get all active WebSocket connections
        connections = get_all_connections()
        print(f"Found {len(connections)} active connections")
        
        for record in event["Records"]:
            if record.get("eventName") not in ["INSERT", "MODIFY"]:
                continue
                
            try:
                new_data = record["dynamodb"]["NewImage"]
                sensor_data = prepare_sensor_payload(new_data)
                
                # Broadcast to all connections
                results = broadcast_to_connections(connections, sensor_data)
                
                successful_sends += results['success']
                failed_sends += results['failures']
                stale_connections += results['stale']
                processed_records += 1
                
            except Exception as e:
                print(f"Error processing record: {str(e)}")
                continue
                
        print(f"Processed {processed_records} records. Sent to {successful_sends} clients. {failed_sends} failures. Removed {stale_connections} stale connections.")
        
        return {
            "statusCode": 200,
            "body": json.dumps({
                "processed_records": processed_records,
                "successful_sends": successful_sends,
                "failed_sends": failed_sends,
                "stale_connections": stale_connections
            })
        }
        
    except Exception as e:
        print(f"Critical error: {str(e)}")
        return {"statusCode": 500, "body": "Internal server error"}


def get_all_connections():
    """Retrieve all active WebSocket connections"""
    response = connections_table.scan(
        ProjectionExpression='connectionId'
    )
    return response.get('Items', [])


def prepare_sensor_payload(new_data):
    try:
        if "payload" in new_data and "M" in new_data["payload"]:
            new_data = new_data["payload"]["M"]

        def get_value(data, field, default=None):
            try:
                if 'S' in data.get(field, {}):
                    return data[field]['S']
                if 'N' in data.get(field, {}):
                    return data[field]['N']
                return default
            except (KeyError, ValueError, AttributeError):
                return default

        # MATCH FRONTEND EXPECTED FORMAT
        payload = {
            "action": "ControlCmd",       # Changed from "sensorUpdate"
            "command": "sensorUpdate",    # New required field
            "data": {
                "payload": {
                    "aqi": float(get_value(new_data, "aqi", 0)),
                    "pm25": float(get_value(new_data, "pm25", 0)),
                    "co2": float(get_value(new_data, "co2", 0)),
                    "pm10": float(get_value(new_data, "pm10", 0)),
                    "humidity": float(get_value(new_data, "humidity", 0)),
                    "temperature": float(get_value(new_data, "temperature", 0)),
                    "location": get_value(new_data, "location", "Unknown"),
                    "timestamp": int(float(get_value(new_data, "timestamp", time.time())))
                },
                "location": get_value(new_data, "location", "Unknown"),
                "timestamp": int(float(get_value(new_data, "timestamp", time.time())))
            }
        }
        print("Prepared payload:", json.dumps(payload, indent=2))
        return payload
    except Exception as e:
        print(f"Error in prepare_sensor_payload: {str(e)}")
        return None


def broadcast_to_connections(connections, payload):
    results = {'success': 0, 'failures': 0, 'stale': 0}
    
    def send_to_connection(connection):
        connection_id = connection['connectionId']
        try:
            # Add timeout for the post operation
            apigateway.post_to_connection(
                ConnectionId=connection_id,
                Data=json.dumps(payload)
            )
            print(f"Successfully sent to {connection_id}")
            return 'success'
            
        except apigateway.exceptions.GoneException:
            print(f"Stale connection: {connection_id}")
            try:
                connections_table.delete_item(
                    Key={'connectionId': connection_id}
                )
            except Exception as e:
                print(f"Failed to delete stale connection {connection_id}: {str(e)}")
            return 'stale'
            
        except Exception as e:
            print(f"Failed to send to {connection_id}: {str(e)}")
            return 'failure'
    
    # Limit concurrent workers to avoid throttling
    with ThreadPoolExecutor(max_workers=5) as executor:
        futures = [executor.submit(send_to_connection, conn) for conn in connections]
        for future in futures:
            result = future.result()
            results[result] += 1
    
    return results
