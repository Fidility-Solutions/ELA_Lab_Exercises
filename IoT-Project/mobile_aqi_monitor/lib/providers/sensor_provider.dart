// lib/providers/sensor_provider.dart

import 'dart:async';
import 'dart:convert';
import 'package:flutter/foundation.dart';
import '../models/sensor_data.dart';
import '../services/websocket_service.dart';

class SensorProvider with ChangeNotifier {
  final WebSocketService _webSocketService = WebSocketService();
  SensorData _sensorData = SensorData.empty();
  StreamSubscription? _subscription;
  
  // Available locations
  final List<String> _locations = [
    'Electronic City',
    'Bengaluru',
    'Neeladri Nagar',
    'Goodworks'
  ];
  
  String _selectedLocation = 'Electronic City';

  SensorData get sensorData => _sensorData;
  List<String> get locations => _locations;
  String get selectedLocation => _selectedLocation;
  bool get isConnected => _webSocketService.isConnected;

  Future<void> connectWebSocket() async {
    try {
      await _webSocketService.connect();
      _subscription = _webSocketService.dataStream.listen((data) {
        // Only update if data is for selected location or no location filter
        if (data.location == _selectedLocation || _selectedLocation.isEmpty) {
          _sensorData = data;
          notifyListeners();
        }
      }, onError: (error) {
        print('WebSocket error: $error');
      });
    } catch (e) {
      print('Failed to connect to WebSocket: $e');
    }
  }

  void setLocation(String location) {
    _selectedLocation = location;
    notifyListeners();
  }

  // Send a location preference to the server if needed
  void sendLocationPreference() {
    if (_webSocketService.isConnected) {
      _webSocketService.send(json.encode({
        'action': 'setLocation',
        'location': _selectedLocation
      }));
    }
  }

  void disconnect() {
    _subscription?.cancel();
    _webSocketService.disconnect();
  }

  @override
  void dispose() {
    disconnect();
    super.dispose();
  }
}
