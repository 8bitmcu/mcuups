#include <EmonLib.h>

// Analog input pin connected to ZMCT103C current sensor output
#define ZMCT103C_PIN              A0

// Calibration factor for the ZMCT103C module
#define ZMCT103C_CAL              1.26

// Analog input pin for battery voltage divider module
#define BATTERY_VOLTAGE_PIN       A1

// Enter the measured DC voltage. Factor will be calculated automatically
#define BATTERY_KNOWNVOLTAGE      14.2

#define AC_VOLTAGE                120.0

EnergyMonitor em;

void setup() {
    pinMode(BATTERY_VOLTAGE_PIN, INPUT);
    pinMode(ZMCT103C_PIN, INPUT);

    // Initialize serial
    Serial.begin(57600);

    // Initialize EmonLib (ZMCT103C)
    em.current(ZMCT103C_PIN, ZMCT103C_CAL);
}

void loop() {

    // Read current output from ZMCT103C
    double irms = em.calcIrms(4096);

    Serial.print("AC Current: ");
    Serial.println(irms, 4);

    Serial.print("Est. AC Power: ");
    Serial.println(irms * AC_VOLTAGE, 4);

    // Read voltage output from voltage module, without factor
    long sum = 0;
    const int samples = 10;
    // Average multiple readings for stability
    for (int i = 0; i < samples; i++) {
        sum += analogRead(BATTERY_VOLTAGE_PIN);
        delay(2);
    }

    float volt = (sum / samples) / 1024.0 * 5.0;

    // Estimate factor based on known voltage
    float fact = BATTERY_KNOWNVOLTAGE / volt;

    Serial.print("Calculated BATTERY_VOLTAGE_FACTOR: ");
    Serial.println(fact, 2);
    Serial.println("");

    delay(1000);
}
