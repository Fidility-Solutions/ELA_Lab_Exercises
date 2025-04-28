import json
import boto3
from datetime import datetime

iot_client = boto3.client('iot-data', region_name='ap-south-1')

def send_websocket_response(event, message):
    if 'requestContext' not in event:
        print("No requestContext found. Skipping WebSocket response (likely test event).")
        return

    try:
        domain = event['requestContext']['domainName']
        stage = event['requestContext']['stage']
        connection_id = event['requestContext']['connectionId']

        print(f"Sending WebSocket response to connection: {connection_id}")  # Debug log

        apigw = boto3.client('apigatewaymanagementapi',
                             endpoint_url=f"https://{domain}/{stage}")

        apigw.post_to_connection(
            ConnectionId=connection_id,
            Data=json.dumps(message).encode('utf-8')
        )
        print(f"Message sent: {message}")  # Debug log
    except Exception as e:
        print(f"Error sending WebSocket message: {e}")

def lambda_handler(event, context):
    print("Received event:", event)

    try:
        body = json.loads(event.get('body', '{}'))
        command = body.get('command')
        location = body.get('location', 'default_location')
    except Exception as e:
        send_websocket_response(event, {'error': f'Invalid input: {str(e)}'})
        return {'statusCode': 400, 'body': 'Bad Request'}

    if command == 'setInterval':
        try:
            interval = body.get('interval', body.get('value'))

            if interval not in [1, 2, 5, 60, 120, 300]:
                send_websocket_response(event, {'error': 'Invalid interval value'})
                return {'statusCode': 400, 'body': 'Invalid interval'}

            mqtt_topic = f"esp32/{location}/control"
            mqtt_payload = {
                'action': 'setInterval',
                'interval': interval,
                'timestamp': datetime.now().isoformat()
            }

            iot_client.publish(
                topic=mqtt_topic,
                qos=1,
                payload=json.dumps(mqtt_payload)
            )

            send_websocket_response(event, {
                'message': f'Interval updated to {interval}s for {location}'
            })
            return {'statusCode': 200, 'body': 'OK'}

        except Exception as e:
            send_websocket_response(event, {'error': str(e)})
            return {'statusCode': 500, 'body': 'Server error'}

    elif command == 'eraseEEPROM':
        try:
            mqtt_topic = f"esp32/{location}/control"
            mqtt_payload = {
                'action': 'eraseEEPROM',
                'timestamp': datetime.now().isoformat()
            }

            iot_client.publish(
                topic=mqtt_topic,
                qos=1,
                payload=json.dumps(mqtt_payload)
            )

            send_websocket_response(event, {
                'message': f'EEPROM erase command sent to {location}'
            })
            return {'statusCode': 200, 'body': 'OK'}

        except Exception as e:
            send_websocket_response(event, {'error': str(e)})
            return {'statusCode': 500, 'body': 'Server error'}

    else:
        send_websocket_response(event, {'error': f'Unsupported command: {command}'})
        return {'statusCode': 400, 'body': 'Unsupported command'}

