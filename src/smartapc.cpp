#include "smartapc.h"
#include <Arduino.h>

SmartAPC::SmartAPC() {
    Serial.begin(2400);
    Serial.setTimeout(100);

    updateBattery = 0;
    prevOnBattery = false;
    lowBatteryWarn = false;
}

void SmartAPC::readCommands(UPSState *state) {
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
                Serial.print(state->batteryPercent, 1);
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
                Serial.print(state->batteryVoltage, 2);
                Serial.print("\n\r");
                break;

            case 'R': // Turn off Smart mode
                Serial.print("BYE\n\r");
                break;

            case 'Q': 
                Serial.print(state->onBattery ? "10\n\r" : "08\n\r");
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

void SmartAPC::sendCommands(UPSState *state) {
    if (state->onBattery) {
        if (updateBattery == 0) {
            updateBattery = millis();
        }
        if (millis() > updateBattery) {
            Serial.print("!\n\r");
            updateBattery += 30000;
        }
        prevOnBattery = true;
    } else if (prevOnBattery && !state->onBattery) {
        Serial.print("$\n\r");
        prevOnBattery = false;
    }

    if (state->batteryPercent < 10.0 && !lowBatteryWarn) {
        Serial.print("%\n\r");
        lowBatteryWarn = true;
    } else if (state->batteryPercent > 40.0 && lowBatteryWarn) {
        Serial.print("+\n\r");
        lowBatteryWarn = false;
    }
}
