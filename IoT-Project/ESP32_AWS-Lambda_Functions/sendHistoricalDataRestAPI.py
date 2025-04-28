import json
import time
from datetime import datetime, timedelta
from decimal import Decimal
import boto3
from boto3.dynamodb.conditions import Key
from dateutil.relativedelta import relativedelta

# Initialize DynamoDB
dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('ESP32_AirQualitySensorData')

# Constants
VALID_LOCATIONS = ["Electronic City", "Neeladri Nagar", "Bengaluru", "GoodWorks"]
VALID_GRANULARITY = ['sec', 'min', 'hr', 'day', 'month', 'year']
MAX_DATA_POINTS = 500  # Match frontend constant

def decimal_to_float(obj):
    """Recursively convert Decimal objects to float."""
    if isinstance(obj, list):
        return [decimal_to_float(i) for i in obj]
    elif isinstance(obj, dict):
        return {k: decimal_to_float(v) for k, v in obj.items()}
    elif isinstance(obj, Decimal):
        return float(obj)
    return obj

def get_time_range(granularity, range_value):
    """Calculate start time based on granularity and range."""
    end_time = datetime.now()
    
    if granularity == 'year':
        start_time = end_time - relativedelta(years=range_value)
    elif granularity == 'month':
        start_time = end_time - relativedelta(months=range_value)
    elif granularity == 'day':
        start_time = end_time - timedelta(days=range_value)
    elif granularity == 'hr':
        start_time = end_time - timedelta(hours=range_value)
    elif granularity == 'min':
        start_time = end_time - timedelta(minutes=range_value)
    else:  # seconds
        start_time = end_time - timedelta(seconds=range_value)
    
    return start_time, end_time

def validate_parameters(params):
    """Validate and sanitize input parameters."""
    location = params.get('location', 'Electronic City')
    granularity = params.get('granularity', 'hr')
    time_range = int(params.get('range', '24'))  # Default to 24 hours if not specified
    
    if location not in VALID_LOCATIONS:
        raise ValueError(f"Invalid location. Valid options: {VALID_LOCATIONS}")
        
    if granularity not in VALID_GRANULARITY:
        raise ValueError(f"Invalid granularity. Valid options: {VALID_GRANULARITY}")

    try:
        time_range = int(params.get('range', '24'))
    except ValueError:
        raise ValueError("Range must be a valid integer")
    
    # Additional validation for specific granularities
    if granularity == 'month' and time_range not in [1, 3, 6, 12, 24]:
        raise ValueError("Month range must be 1, 3, 6, 12, or 24")
    elif granularity == 'hr' and time_range not in [1, 6, 12, 24, 48, 72]:
        raise ValueError("Hour range must be 1, 6, 12, 24, 48, or 72")
    elif granularity == 'year' and time_range not in [1, 2, 5, 10]:
        raise ValueError("Year range must be 1, 2, 5, or 10")
    
    return location, granularity, time_range

def lambda_handler(event, context):
    """Handle historical air quality data requests."""
    print("Received event:", json.dumps(event))
    # Check if queryStringParameters exist
    query_params = event.get("queryStringParameters", {})
    
    # Log parameters received
    print("Query Parameters:", query_params)
    # Extract parameters safely
    location = query_params.get("location", "default_location")
    granularity = query_params.get("granularity", "hr")
    range_value = query_params.get("range", "24")
    
    headers = {
        'Content-Type': 'application/json',
        'Access-Control-Allow-Origin': '*',
        'Cache-Control': 'max-age=60'
    }
    
    try:
        # Extract and validate parameters
        params = event.get("queryStringParameters", {}) or {}
        location, granularity, time_range = validate_parameters(params)
        
        # Calculate time range
        start_time, end_time = get_time_range(granularity, time_range)
        start_timestamp = int(start_time.timestamp())
        end_timestamp = int(end_time.timestamp())

        # Verify timestamp order
        if start_timestamp > end_timestamp:
            raise ValueError("Invalid time range: start time cannot be after end time")

        # Build query parameters
        query_params = {
            'KeyConditionExpression': Key('location').eq(location) & 
                                    Key('timestamp').between(start_timestamp, end_timestamp),
            'ScanIndexForward': False,  # Get most recent first
            'Limit': MAX_DATA_POINTS
        }

        # Handle pagination
        last_evaluated_key = params.get('lastEvaluatedKey')
        if last_evaluated_key:
            try:
                query_params['ExclusiveStartKey'] = json.loads(last_evaluated_key)
            except json.JSONDecodeError as e:
                raise ValueError(f"Invalid lastEvaluatedKey format: {str(e)}")

        # Query DynamoDB
        response = table.query(**query_params)
        items = decimal_to_float(response.get('Items', []))

        # Log basic metrics
        print(f"Retrieved {len(items)} records for {location} between {start_time} and {end_time}")

        # Prepare response
        response_body = {
            'data': items,
            'count': len(items),
            'location': location,
            'timeRange': time_range,
            'granularity': granularity,
            'startTime': start_timestamp,
            'endTime': end_timestamp
        }

        # Include pagination token if more data exists
        if 'LastEvaluatedKey' in response:
            response_body['lastEvaluatedKey'] = json.dumps(decimal_to_float(response['LastEvaluatedKey']))
            response_body['hasMore'] = True
        else:
            response_body['hasMore'] = False

        return {
            'statusCode': 200,
            'headers': headers,
            'body': json.dumps(response_body)
        }
    
    except ValueError as e:
        print("Validation error:", str(e))
        return {
            'statusCode': 400,
            'headers': headers,
            'body': json.dumps({'error': str(e)})
        }
        
    except Exception as e:
        print("Processing failed:", str(e))
        return {
            'statusCode': 500,
            'headers': headers,
            'body': json.dumps({
                'error': "Internal server error",
                'message': str(e)
            })
        }
