#ifndef __hsc_ssc_i2c_h_
#define __hsc_ssc_i2c_h_

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

struct cs_raw {
    uint8_t status;             // 2 bit
    uint16_t bridge_data;       // 14 bit
    uint16_t temperature_data;  // 11 bit
};

uint8_t ps_get_raw(const uint8_t slave_addr, struct cs_raw *raw);
uint8_t ps_convert(const struct cs_raw raw, float *pressure, float *temperature,
                   const uint16_t output_min, const uint16_t output_max, const float pressure_min,
                   const float pressure_max);

#endif
