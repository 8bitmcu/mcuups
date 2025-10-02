#include <Arduino.h>
#include <EmonLib.h>
#include "smartapc.h"
#include "kalman.h"
#include "config.h"

const int lfp_table_sz = sizeof(lfp_table) / sizeof(lfp_table[0]);

KalmanFilter v_filter;
EnergyMonitor em;
SmartAPC apc;
UPSState ups;

unsigned long int v_timer;
unsigned long int upd_interval;

// Samples voltage 10 times and gets the median value
// Filters the value using Kalman filter
float get_voltage() {
    const int SAMPLE_SIZE = 10;
    float samples[SAMPLE_SIZE];

    for(int i = 0; i < SAMPLE_SIZE; i++) {
        samples[i] = analogRead(BATTERY_VOLTAGE_PIN) * (5.0 / 1023.0) * BATTERY_VOLTAGE_FACTOR;
        delay(10);
    }

    // Simple bubble sort
    for(int i = 0; i < SAMPLE_SIZE - 1; i++) {
        for(int j = i + 1; j < SAMPLE_SIZE; j++) {
            if(samples[j] < samples[i]) {
                float temp = samples[i];
                samples[i] = samples[j];
                samples[j] = temp;
            }
        }
    }

    // Use median value
    return v_filter.update(samples[SAMPLE_SIZE / 2]);
}

// Linear correlation between percent and runtime
// Converts from hours to seconds
int percent_to_runtime(float percent) {
    return (BATTERY_AVGRUNTIME * 60 * 60) * percent / 100;
}

// Reads the current draw from mains and assume we're
// online if we're seeing a current draw above MIN_CURRENT
bool get_on_battery(double current) {
    // Assume that we're online/charging if we're seeing a current draw above MIN_CURRENT
    return current <= ZMCT103C_MINCURRENT;
}

// Converts a voltage value to a SoC
// Uses dynamic curve & linear interpolation to get a more accurate SoC
float voltage_to_percentage(float voltage) {
    // Below minimum
    if (voltage <= lfp_table[lfp_table_sz - 1].voltage) {
        return 0.0;
    }

    // Above maximum
    if (voltage >= lfp_table[0].voltage) {
        return 100.0;
    }

    // Linear interpolation between two points
    for (int i = 0; i < lfp_table_sz - 1; i++) {
        if (voltage >= lfp_table[i + 1].voltage) {
            float v1 = lfp_table[i].voltage;
            float v2 = lfp_table[i + 1].voltage;
            float p1 = lfp_table[i].percentage;
            float p2 = lfp_table[i + 1].percentage;

            // Linear interpolation formula
            return p1 + (voltage - v1) * (p2 - p1) / (v2 - v1);
        }
    }

    return 0;
}

void setup() {
    pinMode(BATTERY_VOLTAGE_PIN, INPUT);
    pinMode(ZMCT103C_PIN, INPUT);

    // Initialize our SmartAPC "emulation"
    apc = SmartAPC();

    // Initialize EmonLib (ZMCT103C)
    em.current(ZMCT103C_PIN, ZMCT103C_CAL);

    v_timer = millis();

    upd_interval = millis();

    v_filter.init(0.01, 0.2, 1, 0);
}

void loop() {
    if(upd_interval <= millis()) {
        apc.irms = em.calcIrms(4096);
        ups.on_battery = get_on_battery(apc.irms);
        ups.battery_voltage = get_voltage();
        ups.battery_percent = voltage_to_percentage(ups.battery_voltage);
        ups.runtime = percent_to_runtime(ups.battery_percent);

        upd_interval += 1000;
    }

    apc.read_commands(&ups);
    apc.send_commands(&ups);
}

