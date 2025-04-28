## ESP32 AWS Lambda Functions ##
	-> This repository contains AWS Lambda functions used to interface an ESP32-based IoT system with AWS Cloud Services.
	-> The functions handle REST API requests and WebSocket events for real-time and historical data management.

#Project Structure#
	*File*					*Description*
	------					--------------
	1. WebSocketConnectHandler.py		Lambda function triggered when a client connects to the WebSocket API. It manages connection initialization tasks.
	2. WebSocketDisconnectHandler.py	Lambda function triggered when a client disconnects from the WebSocket API. It manages resource cleanup tasks.
	3. WebSocketRealTimeDataHandler.py	Handles incoming real-time data from the ESP32 and broadcasts it to connected WebSocket clients.
	4. sendHistoricalDataRestAPI.py		Handles REST API requests from clients to fetch historical sensor data stored in the backend (e.g., DynamoDB).
	5. WebSocketSetIntervalHandler.py	Allows clients to set/update the data push interval (how often they want to receive data) over the WebSocket.


# Architecture Overview #
	-> ESP32 pushes sensor data via WebSocket to AWS API Gateway ➔ Lambda ➔ Database.

    	* Clients can fetch:
    		-> Real-time data via WebSocket.
        	-> Historical data via a REST API endpoint (triggering sendHistoricalDataRestAPI.py).

    	* Clients can also customize data reception intervals dynamically.





# Requirements #
    	1. AWS Account
    	2. DynamoDB (or any backend database for storing historical data)
    	3. AWS Lambda
    	4. API Gateway (REST + WebSocket APIs)
    	5. IAM Roles with correct permissions
    	6. CloudWatch for view error logs

# Setup Instructions #

    	1. Deploy each Python file as an individual AWS Lambda function.

    	* Set appropriate triggers: *

        2. REST API Gateway (WebHistoricalRestAPI) ➔ sendHistoricalDataRestAPI.py

        3. WebSocket Events (Connect/Disconnect/Message) ➔ WebSocket handlers.

    	4. Configure IAM roles and policies to allow database read/write access.

    	5. Set environment variables (e.g., database table names, time intervals) if needed.


## Notes ##

    -> Ensure WebSocket route keys are correctly mapped:
 
	1. Unders ESP32_SensorDataUpdates Websocket API Gateway
	
        	-> $connect ➔ WebSocketConnectHandler.py

        	-> $disconnect ➔ WebSocketDisconnectHandler.py

        	-> RealTimeDataUpdate ➔ WebSocketRealTimeDataHandler.py

        	-> ControlCmd ➔ WebSocketSetIntervalHandler.py
        	
        2. Under WebHistoricalRestAPI REST API Gateway:
        
        	-> For Historical Data ➔ sendHistoricalDataRestAPI.py

    -> Use DynamoDB TTLs or cleanup scripts if storing large amounts of real-time data.

    -> Ensure efficient error handling inside all Lambda functions for better reliability.

