#include "bmp280.h"
#include <pigpio.h>
#include <iostream>
#include <unistd.h>
#include <mqtt/async_client.h>

#define BMP280_REG_TEMP_XLSB 0xFC
#define BMP280_REG_TEMP_LSB  0xFB
#define BMP280_REG_TEMP_MSB  0xFA
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG    0xF5

BMP280::BMP280(int i2c_bus, int address, const std::string& mqtt_server, const std::string& mqtt_topic)
    : i2c_bus(i2c_bus), address(address), i2c_handle(-1), mqtt_server(mqtt_server), mqtt_topic(mqtt_topic) {
    client = new mqtt::async_client(mqtt_server, "");
}

BMP280::~BMP280() {
    if (i2c_handle >= 0) {
        i2cClose(i2c_handle);
    }
    delete client;
}

bool BMP280::begin() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialization failed" << std::endl;
        return false;
    }

    i2c_handle = i2cOpen(i2c_bus, address, 0);
    if (i2c_handle < 0) {
        std::cerr << "Failed to open I2C" << std::endl;
        return false;
    }

    // Read calibration data
    readCalibrationData();

    // Configure the BMP280
    writeRegister(BMP280_REG_CTRL_MEAS, 0x27); // Normal mode, temp and pressure oversampling 1
    writeRegister(BMP280_REG_CONFIG, 0xA0);    // Standby time 1000ms

    sleep(1); // Wait for sensor to stabilize

    // Connect to MQTT broker
    mqtt::connect_options connOpts;
    client->connect(connOpts)->wait();

    return true;
}

void BMP280::writeRegister(uint8_t reg, uint8_t value) {
    i2cWriteByteData(i2c_handle, reg, value);
}

uint32_t BMP280::read24(uint8_t reg) {
    uint32_t value = i2cReadByteData(i2c_handle, reg) << 16;
    value |= i2cReadByteData(i2c_handle, reg + 1) << 8;
    value |= i2cReadByteData(i2c_handle, reg + 2);
    return value;
}

void BMP280::readCalibrationData() {
    dig_T1 = i2cReadWordData(i2c_handle, 0x88);
    dig_T2 = i2cReadWordData(i2c_handle, 0x8A);
    dig_T3 = i2cReadWordData(i2c_handle, 0x8C);
}

int32_t BMP280::compensateTemperature(int32_t adc_T) {
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

float BMP280::readTemperature() {
    int32_t adc_T = read24(BMP280_REG_TEMP_MSB);
    adc_T >>= 4;
    int32_t temp = compensateTemperature(adc_T);
    return temp / 100.0;
}

void BMP280::publishTemperature() {
    float temperature = readTemperature();
    std::string payload = "Temperature: " + std::to_string(temperature) + " °C";
    mqtt::message_ptr pubmsg = mqtt::make_message(mqtt_topic, payload);
    pubmsg->set_qos(1);
    client->publish(pubmsg)->wait_for(std::chrono::seconds(10));
}
