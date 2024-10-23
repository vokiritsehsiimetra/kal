#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>

class BMP280 {
public:
    BMP280(int i2c_bus, int address);
    ~BMP280();

    bool begin();
    float readTemperature();
    
private:
    int32_t t_fine;
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;

    void readCalibrationData();
    int32_t compensateTemperature(int32_t adc_T);

    int i2c_handle;
    int i2c_bus;
    int address;

    void writeRegister(uint8_t reg, uint8_t value);
    uint32_t read24(uint8_t reg);
};

#endif // BMP280_H
