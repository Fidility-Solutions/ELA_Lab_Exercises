// lib/models/sensor_data.dart

class SensorData {
  final double aqi;
  final double pm25;
  final double co2;
  final double pm10;
  final double humidity;
  final double temperature;
  final String location;
  final int timestamp;

  SensorData({
    required this.aqi,
    required this.pm25,
    required this.co2,
    required this.pm10,
    required this.humidity,
    required this.temperature,
    required this.location,
    required this.timestamp,
  });

  factory SensorData.fromJson(Map<String, dynamic> json) {
    final payload = json['data']['payload'];
    
    return SensorData(
      aqi: payload['aqi'].toDouble(),
      pm25: payload['pm25'].toDouble(),
      co2: payload['co2'].toDouble(),
      pm10: payload['pm10'].toDouble(),
      humidity: payload['humidity'].toDouble(),
      temperature: payload['temperature'].toDouble(),
      location: payload['location'],
      timestamp: payload['timestamp'],
    );
  }

  // For initial state or when there's no data
  factory SensorData.empty() {
    return SensorData(
      aqi: 0,
      pm25: 0,
      co2: 0,
      pm10: 0,
      humidity: 0,
      temperature: 0,
      location: 'Unknown',
      timestamp: DateTime.now().millisecondsSinceEpoch ~/ 1000,
    );
  }

  // Helper method to get AQI text status
  String get aqiStatus {
    if (aqi <= 50) return 'Good';
    if (aqi <= 100) return 'Moderate';
    if (aqi <= 150) return 'Unhealthy for Sensitive Groups';
    if (aqi <= 200) return 'Unhealthy';
    if (aqi <= 300) return 'Very Unhealthy';
    return 'Hazardous';
  }

  // Helper method to get AQI color
  String get aqiColor {
    if (aqi <= 50) return '#90EE90'; // Light green
    if (aqi <= 100) return '#FFFF00'; // Yellow
    if (aqi <= 150) return '#FFA500'; // Orange
    if (aqi <= 200) return '#FF0000'; // Red
    if (aqi <= 300) return '#800080'; // Purple
    return '#800000'; // Maroon
  }
}
