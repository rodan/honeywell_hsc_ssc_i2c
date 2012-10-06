
#include <Wire.h>
#include "hsc_ssc_i2c.h"

// see hsc_ssc_i2c.h for a description of these values
// these defaults are valid only for chip HSCMRNN030PA2A3
#define SLAVE_ADDR 0x28
#define OUTPUT_MIN 0
#define OUTPUT_MAX 0x3fff       // 2^14
#define PRESSURE_MIN 0.0        // min is 0 for sensors that give absolute values
#define PRESSURE_MAX 206842.7   // 30Psi (and we want results in pascals)

unsigned long prev = 0, interval = 5000;

void setup()
{
    delay(200);            // my particular board needs this
    Serial.begin(9600);
    Wire.begin();
}

void loop()
{
    unsigned long now = millis();
    struct cs_raw ps;
    char p_str[10], t_str[10];
    uint8_t el;

    float p, t;

    if ((now - prev > interval) && (Serial.available() <= 0)) {

        prev = now;

        el = ps_get_raw(SLAVE_ADDR, &ps);

        // for some reason my chip triggers a diagnostic fault
        // on 50% of powerups without a notable impact 
        // to the output values.

        if ( el == 4 ) {
            Serial.println("err sensor missing");
        } else {

            if ( el == 3 ) {
                Serial.print("err diagnostic fault ");
                Serial.println(ps.status, BIN);
            }
            if ( el == 2 ) {
                // if data has already been feched since the last
                // measurement cycle
                Serial.print("warn stale data ");
                Serial.println(ps.status, BIN);
            }
            if ( el == 1 ) {
                // chip in command mode
                // no clue how to end up here
                Serial.print("warn command mode ");
                Serial.println(ps.status, BIN);
            }

            Serial.print("status      ");
            Serial.println(ps.status, BIN);
            Serial.print("bridge_data ");
            Serial.println(ps.bridge_data, DEC);
            Serial.print("temp_data   ");
            Serial.println(ps.temperature_data, DEC);
            Serial.println("");

            ps_convert(ps, &p, &t, OUTPUT_MIN, OUTPUT_MAX, PRESSURE_MIN,
                   PRESSURE_MAX);
            // floats cannot be easily printed out
            dtostrf(p, 2, 2, p_str);
            dtostrf(t, 2, 2, t_str);
            Serial.print("pressure    (Pa) ");
            Serial.println(p_str);
            Serial.print("temperature (dC) ");
            Serial.println(t_str);
            Serial.println("");
        }

    }
}

