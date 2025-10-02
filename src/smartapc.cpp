#include "smartapc.h"
#include <Arduino.h>

SmartAPC::SmartAPC() {
    Serial.begin(2400);
    Serial.setTimeout(100);

    update_battery = 0;
    prev_on_battery = false;
    low_battery_warn = false;
}

void SmartAPC::read_commands(UPSState *state) {
    while (Serial.available()) {
        char cmd = Serial.read();
        switch(cmd) {
            case 'Y':  // Set smart mode (required for init)
                Serial.print("SM\n\r");
                break;

            case 'a': //  Protocol info
                Serial.print("3.!$%+.BRQYafgjz\n\r");
                break;

            case 'f': // Battery percentage
                Serial.print(state->battery_percent, 1);
                Serial.print("\n\r");
                break;

            case 'g': // Battery nominal voltage (012, 024, 048)
                Serial.print("012\n\r");
                break;

            case 'j': // Estimated runtime in minutes
                Serial.print(state->runtime/60);
                Serial.print("\n\r");
                break;

            case 'B': // Battery voltage
                Serial.print(state->battery_voltage, 2);
                Serial.print("\n\r");
                break;

            case 'R': // Turn off Smart mode
                Serial.print("BYE\n\r");
                break;

            case 'Q': 
                Serial.print(state->on_battery ? "10\n\r" : "08\n\r");
                break;

            case 'w':
                Serial.print(irms, 3);
                Serial.print("\n\r");
                break;

            case '\r': // Carriage return (ignore)
            case '\n': // Newline (ignore)
            case 'z':
                break;

            default:  // Unknown command - send NA (Not Available)
                Serial.print("NA\n\r");
                break;
        }
    }
}

void SmartAPC::send_commands(UPSState *state) {
    if (state->on_battery) {
        if (update_battery == 0) {
            update_battery = millis();
        }
        if (millis() > update_battery) {
            Serial.print("!\n\r");
            update_battery += 30000;
        }
        prev_on_battery = true;
    } else if (prev_on_battery && !state->on_battery) {
        Serial.print("$\n\r");
        prev_on_battery = false;
    }

    if (state->battery_percent < 10.0 && !low_battery_warn) {
        Serial.print("%\n\r");
        low_battery_warn = true;
    } else if (state->battery_percent > 40.0 && low_battery_warn) {
        Serial.print("+\n\r");
        low_battery_warn = false;
    }
}

