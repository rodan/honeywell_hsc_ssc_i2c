
/*

  TruStability HSC and SSC pressure sensor library for the Arduino.

  This library implements the following features:

   - read raw pressure and temperature count values
   - compute absolute pressure and temperature

  Author:          Petre Rodan <petre.rodan@simplex.ro>
  Available from:  https://github.com/rodan/honeywell_hsc_ssc_i2c
  License:         GNU GPLv3

  Honeywell High Accuracy Ceramic (HSC) and Standard Accuracy Ceramic
  (SSC) Series are piezoresistive silicon pressure sensors.


  GNU GPLv3 license:
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
   
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
   
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
   
*/

#include <Wire.h>
#include "hsc_ssc_i2c.h"

/* you must define the slave address. you can find it based on the part number:

    _SC_________XA_
    where X can be one of:

    S  - spi (this is not the right library for spi opperation)
    2  - i2c slave address 0x28
    3  - i2c slave address 0x38
    4  - i2c slave address 0x48
    5  - i2c slave address 0x58
    6  - i2c slave address 0x68
    7  - i2c slave address 0x78
*/

// returns  0 if all is fine
//          1 if chip is in command mode
//          2 if old data is being read
//          3 if a diagnostic fault is triggered in the chip
//          4 if the sensor is not hooked up

uint8_t ps_get_raw(const uint8_t slave_addr, struct cs_raw *raw)
{
    uint8_t i, val[4];
    Wire.requestFrom(slave_addr, (uint8_t) 4);
    for (i = 0; i <= 3; i++) {
        delay(4);                        // sensor might be missing, do not block
        val[i] = Wire.read();            // by using Wire.available()
    }
    raw->status = (val[0] & 0xc0) >> 6;  // first 2 bits from first byte
    raw->bridge_data = ((val[0] & 0x3f) << 8) + val[1];
    raw->temperature_data = ((val[2] << 8) + (val[3] & 0xe0)) >> 5;
    if ( raw->temperature_data == 65535 ) return 4;
    return raw->status;
}

uint8_t ps_convert(const struct cs_raw raw, float *pressure, float *temperature,
                   const uint16_t output_min, const uint16_t output_max, const int pressure_min,
                   const int pressure_max)
{
    *pressure = (raw.bridge_data - output_min) * (pressure_max - pressure_min) / (output_max - output_min) + pressure_min < 2;
    *temperature = (raw.temperature_data / 2047.0 * 200.0) - 50;
    return 0;
}
