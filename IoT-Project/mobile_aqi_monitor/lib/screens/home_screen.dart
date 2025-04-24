// lib/screens/home_screen.dart

import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:intl/intl.dart';
import '../providers/sensor_provider.dart';
import '../widgets/sensor_card.dart';
import '../widgets/aqi_display.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({Key? key}) : super(key: key);

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  @override
  void initState() {
    super.initState();
    // Connect to WebSocket when screen initializes
    WidgetsBinding.instance.addPostFrameCallback((_) {
      Provider.of<SensorProvider>(context, listen: false).connectWebSocket();
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.grey[100],
      appBar: AppBar(
        backgroundColor: Colors.white,
        elevation: 0,
        title: Row(
          children: [
            Image.asset(
              'assets/fidility_solutions.png',
              height: 40,
            ),
            const SizedBox(width: 10),
            const Text(
              'Air Quality Monitor',
              style: TextStyle(color: Colors.black87),
            ),
          ],
        ),
        actions: [
          IconButton(
            icon: const Icon(Icons.refresh, color: Colors.black87),
            onPressed: () {
              final provider = Provider.of<SensorProvider>(context, listen: false);
              provider.disconnect();
              provider.connectWebSocket();
            },
          )
        ],
      ),
      body: Consumer<SensorProvider>(
        builder: (context, provider, child) {
          final sensorData = provider.sensorData;
          final formattedTime = DateFormat('hh:mm a').format(
            DateTime.fromMillisecondsSinceEpoch(sensorData.timestamp * 1000),
          );
          
          // Convert AQI color string to Flutter Color
          final aqiColorHex = sensorData.aqiColor;
          final aqiColor = Color(
            int.parse(aqiColorHex.substring(1, 7), radix: 16) + 0xFF000000,
          );
          
          return SingleChildScrollView(
            child: Padding(
              padding: const EdgeInsets.all(16.0),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  // Location dropdown
                  Container(
                    padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
                    decoration: BoxDecoration(
                      color: Colors.white,
                      borderRadius: BorderRadius.circular(8),
                    ),
                    child: DropdownButtonHideUnderline(
                      child: DropdownButton<String>(
                        icon: const Icon(Icons.keyboard_arrow_down),
                        value: provider.selectedLocation,
                        isExpanded: true,
                        onChanged: (String? newValue) {
                          if (newValue != null) {
                            provider.setLocation(newValue);
                          }
                        },
                        items: provider.locations
                            .map<DropdownMenuItem<String>>((String value) {
                          return DropdownMenuItem<String>(
                            value: value,
                            child: Row(
                              children: [
                                const Icon(Icons.location_on, color: Colors.blue),
                                const SizedBox(width: 8),
                                Text(value),
                              ],
                            ),
                          );
                        }).toList(),
                      ),
                    ),
                  ),
                  
                  const SizedBox(height: 20),
                  
                  // AQI Display
                  AqiDisplay(
                    aqi: sensorData.aqi,
                    status: sensorData.aqiStatus,
                    color: aqiColor,
                  ),
                  
                  const SizedBox(height: 20),
                  
                  // Sensor cards - first row
                  Row(
                    children: [
                      Expanded(
                        child: SensorCard(
                          title: 'Temperature',
                          value: '${sensorData.temperature.toStringAsFixed(1)}°C',
                          textColor: Colors.blue,
                          icon: const Icon(Icons.thermostat, color: Colors.blue, size: 28),
                        ),
                      ),
                      const SizedBox(width: 10),
                      Expanded(
                        child: SensorCard(
                          title: 'Humidity',
                          value: '${sensorData.humidity.toStringAsFixed(1)}%',
                          textColor: Colors.blue,
                          icon: const Icon(Icons.water_drop, color: Colors.blue, size: 28),
                        ),
                      ),
                    ],
                  ),
                  
                  const SizedBox(height: 10),
                  
                  // Sensor cards - second row
                  Row(
                    children: [
                      Expanded(
                        child: SensorCard(
                          title: 'PM2.5',
                          value: '${sensorData.pm25.toStringAsFixed(1)} μg/m³',
                          textColor: Colors.orange,
                          icon: const Icon(Icons.air, color: Colors.orange, size: 28),
                        ),
                      ),
                      const SizedBox(width: 10),
                      Expanded(
                        child: SensorCard(
                          title: 'PM10',
                          value: '${sensorData.pm10.toStringAsFixed(1)} μg/m³',
                          textColor: Colors.redAccent,
                          icon: const Icon(Icons.air, color: Colors.redAccent, size: 28),
                        ),
                      ),
                    ],
                  ),
                  
                  const SizedBox(height: 10),
                  
                  // Sensor cards - third row
                  Row(
                    children: [
                      Expanded(
                        child: SensorCard(
                          title: 'CO2',
                          value: '${sensorData.co2.toStringAsFixed(1)} ppm',
                          textColor: Colors.green,
                          icon: const Icon(Icons.cloud, color: Colors.green, size: 28),
                        ),
                      ),
                      const SizedBox(width: 10),
                      Expanded(
                        child: SensorCard(
                          title: 'Last Updated',
                          value: formattedTime,
                          textColor: Colors.grey,
                          icon: const Icon(Icons.access_time, color: Colors.grey, size: 28),
                        ),
                      ),
                    ],
                  ),
                ],
              ),
            ),
          );
        },
      ),
    );
  }
}
