// lib/services/websocket_service.dart

import 'dart:convert';
import 'package:web_socket_channel/web_socket_channel.dart';
import '../models/sensor_data.dart';

class WebSocketService {
  WebSocketChannel? _channel;
  final String _wsUrl = 'wss://bwafegvzu0.execute-api.ap-south-1.amazonaws.com/WebSocketStage/';
  bool _isConnected = false;

  bool get isConnected => _isConnected;

  Future<void> connect() async {
    try {
      _channel = WebSocketChannel.connect(Uri.parse(_wsUrl));
      _isConnected = true;
      print('WebSocket connected');
    } catch (e) {
      _isConnected = false;
      print('WebSocket connection failed: $e');
      rethrow;
    }
  }

  Stream<SensorData> get dataStream {
    if (_channel == null) {
      throw Exception('WebSocket not connected');
    }
    
    return _channel!.stream.map((message) {
      try {
        final jsonData = json.decode(message);
        return SensorData.fromJson(jsonData);
      } catch (e) {
        print('Error parsing sensor data: $e');
        return SensorData.empty();
      }
    });
  }

  void send(String message) {
    if (_channel != null && _isConnected) {
      _channel!.sink.add(message);
    }
  }

  void disconnect() {
    if (_channel != null) {
      _channel!.sink.close();
      _isConnected = false;
      print('WebSocket disconnected');
    }
  }
}
