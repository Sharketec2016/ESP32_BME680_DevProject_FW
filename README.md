# ESP32 BME680 Environmental Monitoring System

## Overview

This project implements an environmental monitoring system using an **ESP32 microcontroller** and a **BME680 environmental sensor**. The system continuously measures environmental conditions and provides real-time data through a web interface.

## Features

- **Environmental Sensing**: Measures temperature, pressure, humidity, and gas resistance using the BME680 sensor
- **Indoor Air Quality (IAQ)**: Calculates Indoor Air Quality metrics based on sensor readings
- **WiFi Connectivity**: Operates as a WiFi access point (softAP) for easy connectivity
- **Web Server Interface**: Provides a web-based interface to access sensor data in real-time
- **Status Indicator**: LED blinking pattern indicates system activity and health
- **Real-time Data Acquisition**: Continuous sampling of sensor data with thread-safe access using mutex protection
- **Multi-tasking**: Uses FreeRTOS for concurrent execution of sensor sampling, LED indication, and web server tasks

## Hardware Requirements

- **ESP32 Development Board**: Specifically configured for the upesy_wrover board
- **BME680 Sensor**: Environmental sensor connected via I2C bus
- **Status LED**: Connected to GPIO for activity indication
- **NVS (Non-Volatile Storage)**: For persistent configuration storage

## Project Structure

```
├── src/
│   ├── main.c                          # Main application entry point
│   └── CMakeLists.txt
├── lib/
│   ├── BME680_Sensor/
│   │   ├── esp_bme680.h               # BME680 interface definitions
│   │   └── esp_bme680.c               # BME680 implementation
│   ├── BME68x_SensorAPI/              # Bosch BME68x sensor API
│   ├── I2C_Handling/                  # I2C communication driver
│   ├── GPIO_Handling/                 # GPIO control for LED and sensors
│   ├── Errors/                        # Error handling utilities
│   └── Esp_Ap_Webserver/              # Web server implementation
├── include/
│   └── project.h                      # Main project header file
├── platformio.ini                     # PlatformIO configuration
└── CMakeLists.txt                    # CMake build configuration
```

## Sensor Configuration

The BME680 sensor is configured with the following parameters:

- **Sampling Mode**: Forced Mode
- **Temperature Oversampling**: 2x
- **Pressure Oversampling**: 16x
- **Humidity Oversampling**: 1x
- **IIR Filter**: Size 15
- **Heater**: Enabled

## Building and Flashing

### Prerequisites

- PlatformIO Core installed
- ESP32 development board connected via USB
- Appropriate USB drivers for your board

### Build

```bash
platformio run --environment upesy_wrover
```

### Upload and Monitor

```bash
platformio run --environment upesy_wrover --target upload --target monitor
```

Or use the configured target in `platformio.ini`:

```bash
platformio run --environment upesy_wrover
```

**Monitor Speed**: 115200 baud

## Application Flow

1. **Initialization**: Configures NVS, GPIO, I2C bus, and BME680 sensor
2. **WiFi Setup**: Initializes WiFi in softAP (access point) mode
3. **Web Server**: Starts the web server for data access
4. **Tasks**: Creates two FreeRTOS tasks:
   - **Alive Task**: Toggles LED every `BLINK_DELAY` milliseconds
   - **Data Acquisition Task**: Samples sensor data every 100ms with mutex protection

## Data Safety

Sensor data is protected using a FreeRTOS mutex (`sensor_data_mutex`) to ensure thread-safe access between the data acquisition task and the web server task.

## Debugging

The project includes logging support via `ESP_LOGI` macro. Debug information is output to the serial console at 115200 baud.

## Future Enhancements

- Data logging to SD card or cloud storage
- Historical data visualization
- Alert thresholds for environmental parameters
- Multiple sensor support
- Power management optimizations

## License

See individual library licenses in the `lib/` directories.

## References

- [Bosch BME680 Sensor Datasheet](https://www.bosch-sensortec.com/products/environmental-sensors/gas-sensors/bme680/)
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/)
- [PlatformIO Documentation](https://docs.platformio.org/)
