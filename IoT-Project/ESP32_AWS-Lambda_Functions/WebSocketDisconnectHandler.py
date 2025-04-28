import json
import boto3
import os

def lambda_handler(event, context):
    try:
        connectionId = event['requestContext']['connectionId']
        
        dynamodb = boto3.client('dynamodb')
        
        # Remove trailing space here!
        delete_response = dynamodb.delete_item(
            TableName=os.environ['WEBDASHBOARD_TABLE'],
            Key={
                'connectionId': {'S':connectionId}
            }
        )
        
        print(f"Deleted connection: {connectionId}")
        
        return {
            'statusCode': 200,
            'body': json.dumps({'status': 'disconnected'})
        }

    except KeyError as e:
        print(f"Missing key in event: {str(e)}")
        return {
            'statusCode': 400,
            'body': json.dumps({'error': 'Invalid request format'})
        }

    except dynamodb.exceptions.ClientError as e:
        print(f"DynamoDB client error: {str(e)}")
        return {
            'statusCode': 500,
            'body': json.dumps({'error': 'Database operation failed'})
        }

    except Exception as e:
        print(f"Unexpected error: {str(e)}")
        return {
            'statusCode': 500,
            'body': json.dumps({'error': 'Internal server error'})
        }

