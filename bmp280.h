#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>
#include <string>
#include <mqtt/async_client.h>

class BMP280 {
public:
    BMP280(int i2c_bus, int address, const std::string& mqtt_server, const std::string& mqtt_topic);
    ~BMP280();

    bool begin();
    float readTemperature();
    void publishTemperature();

private:
    int i2c_handle;
    int i2c_bus;
    int address;
    int32_t t_fine;
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;

    std::string mqtt_server;
    std::string mqtt_topic;
    mqtt::async_client* client;

    void writeRegister(uint8_t reg, uint8_t value);
    uint32_t read24(uint8_t reg);
    void readCalibrationData();
    int32_t compensateTemperature(int32_t adc_T);
};

#endif // BMP280_H
