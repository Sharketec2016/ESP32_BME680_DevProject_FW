# ESP32 BME680 Weather Station

A comprehensive ESP32-based environmental monitoring project featuring the BME680 sensor with WiFi connectivity and a web server interface.

## Overview

This project demonstrates an intermediate ESP32 application that integrates:
- **BME680 Environmental Sensor**: Measures temperature, humidity, pressure, and air quality
- **WiFi Access Point**: Hosts a local network for device connectivity
- **Web Server**: Provides a web interface to monitor environmental data
- **LED Indicator**: Visual feedback for system activity and status
- **Modular Architecture**: Well-organized libraries for I2C communication, GPIO handling, and sensor management

## Hardware Requirements

- **ESP32 Development Board** (specifically tested with UPESY WROVER)
- **BME680 Sensor Module**
- **LED** with appropriate resistor
- Jumper wires and USB cable for programming

## Features

- Real-time environmental monitoring (temperature, humidity, pressure, air quality)
- WiFi access point setup
- Web-based dashboard for remote monitoring
- Modular, well-documented codebase
- FreeRTOS task management for concurrent operations
- I2C bus communication with the BME680 sensor

## Project Structure

```
ESP32/
├── src/                      # Source code
│   └── main.c               # Main application entry point
├── include/                  # Header files
│   └── project.h            # Project configuration
├── lib/                      # Libraries and components
│   ├── BME680_Sensor/       # BME680 sensor driver
│   ├── BME68x_SensorAPI/    # Official Bosch BME68x API
│   ├── I2C_Handling/        # I2C communication layer
│   ├── GPIO_Handling/       # GPIO operations (LED control)
│   ├── Errors/              # Error handling utilities
│   └── Esp_Ap_Webserver/    # WiFi AP and web server
├── test/                     # Unit and integration tests
├── CMakeLists.txt           # CMake build configuration
└── platformio.ini           # PlatformIO configuration
```

## Building and Uploading

### Prerequisites

- [PlatformIO](https://platformio.org/) installed
- ESP-IDF framework configured
- USB connection to ESP32 board

### Build

```bash
platformio run --environment upesy_wrover
```

### Upload to Device

```bash
platformio run --environment upesy_wrover --target upload
```

### Monitor Serial Output

```bash
platformio run --environment upesy_wrover --target monitor
```

## Configuration

Edit `platformio.ini` to configure:
- **Board**: UPESY WROVER (adjustable for other ESP32 boards)
- **Upload Port**: `/dev/ttyUSB*` (adjust for your system)
- **Baud Rate**: 115200

## Dependencies

- **ESP-IDF**: Official Espressif IoT Development Framework
- **FreeRTOS**: Real-time operating system for task management
- **Bosch BME68x Sensor API**: Official driver library for BME680 sensor

## Key Components

### BME680 Sensor Driver
Located in `lib/BME680_Sensor/`, provides high-level interface for sensor initialization, configuration, and data acquisition in sequential mode.

### I2C Communication
`lib/I2C_Handling/` handles I2C bus initialization and communication with the sensor.

### GPIO Handling
`lib/GPIO_Handling/` manages GPIO operations, including the alive LED indicator that blinks to show system activity.

### WiFi Access Point & Web Server
`lib/Esp_Ap_Webserver/` sets up the ESP32 as a WiFi access point and hosts a web server for data monitoring.

## Getting Started

1. Connect the BME680 sensor to the ESP32 via I2C (SDA, SCL pins)
2. Connect an LED to a GPIO pin (with appropriate resistor)
3. Configure the board and upload port in `platformio.ini`
4. Build and upload the firmware
5. Monitor the serial output to verify sensor initialization
6. Connect to the WiFi network hosted by the ESP32
7. Open a browser and navigate to the web server address to view environmental data

## Troubleshooting

- **Sensor not detected**: Verify I2C connection and bus configuration
- **Upload failure**: Check USB port configuration and driver installation
- **Web server not accessible**: Ensure WiFi AP is enabled in the code and connect to the correct network

## Author

Matthew Buchkowski  
Date: 2024-06-10

## License

See individual library licenses in their respective directories.

---

For more information about the BME68x API, visit the [Bosch Sensortec repository](https://github.com/boschsensortec/BME68x-Sensor-API).
