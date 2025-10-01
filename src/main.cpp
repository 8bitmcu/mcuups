#include <Arduino.h>
#include <EmonLib.h>
#include "smartapc.h"
#include "kalman.h"
#include "config.h"

unsigned long v_timer = 0;

KalmanFilter kf;
EnergyMonitor em;
SmartAPC apc;
UPSState ups;

unsigned long int upd_interval;


void setup() {
    pinMode(BATTERY_VOLTAGE_PIN, INPUT);
    pinMode(ZMCT103C_PIN, INPUT);

    // Initialize our SmartAPC "emulation"
    apc = SmartAPC();

    // Initialize EmonLib (ZMCT103C)
    em.current(ZMCT103C_PIN, ZMCT103C_CAL);

    // Initialize KalmanFilter
    kf.init(BATTERY_MAXVOLTAGE, BATTERY_MINVOLTAGE, BATTERY_AVGRUNTIME);

    v_timer = millis();

    upd_interval = millis();
}

void loop() {

    if(upd_interval < millis()) {
        // Read current output from ZMCT103C
        double irms = em.calcIrms(4096);
        if (irms < 0) {
          irms = 0; // Ensure non-negative
        }
        if (irms > 5.0) {
          irms = 5.0; // Reasonable upper bound
        }
        // Assume that we're charging if we're seeing a current draw above MIN_CURRENT
        ups.onBattery = irms <= ZMCT103C_MINCURRENT;

        // Read voltage output from voltage module
        long sum = 0;
        const int samples = 10;
        // Average multiple readings for stability
        for (int i = 0; i < samples; i++) {
            sum += analogRead(BATTERY_VOLTAGE_PIN);
            delay(2);
        }
        ups.batteryVoltage = (sum / samples) / 1024.0 * 5.0 * BATTERY_VOLTAGE_FACTOR;

        // Time since last update (usually ~1sec)
        unsigned long current_time = millis();
        double dt = (current_time - v_timer) / 1000.0;
        if (dt <= 0) {
          dt = 0.001; // Minimum dt to prevent issues
        }
        kf.predict(dt);

        // Update filter with new measurement, total elapsed time (in seconds)
        kf.update(ups.batteryVoltage, millis() / 1000);
        v_timer = millis();

        // Voltage correlation for state of charge (linear model)
        float voltage_range = BATTERY_MAXVOLTAGE - BATTERY_MINVOLTAGE;
        if (voltage_range > 0) {
            ups.batteryPercent = (byte)(((ups.batteryVoltage - BATTERY_MINVOLTAGE) / voltage_range) * 100.0);
        } else {
            ups.batteryPercent = 0; // Default fallback
        }

        // Bounds checking
        if (ups.batteryPercent > 100 || ups.batteryVoltage > BATTERY_MAXVOLTAGE) {
            ups.batteryPercent = 100;
        }
        if (ups.batteryPercent < 0 || ups.batteryVoltage < BATTERY_MINVOLTAGE) {
            ups.batteryPercent = 0;
        }

        // Runtime to empty in minutes
        ups.runtime = kf.get_remaining_time();

        if (ups.batteryPercent == 100 || ups.runtime > BATTERY_AVGRUNTIME * 60) {
          ups.runtime = BATTERY_AVGRUNTIME * 60;
        }

        upd_interval += 1000;
    }

    apc.readCommands(&ups);
    apc.sendCommands(&ups);
}

