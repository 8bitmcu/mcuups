/*
 *   Basic SmartAPC protocol implementation
 *
 *   Created by 8bitmcu, October 2025.
 *   https://docs.ampnuts.ru/eevblog.docs/eBooks/Apc_Ups_Bible.pdf
 */

#ifndef smartapc_h
#define smartapc_h

struct UPSState {
    float batteryVoltage;
    float batteryPercent;
    bool onBattery;
    int runtime;
};


class SmartAPC {
public:

    SmartAPC();

    void readCommands(UPSState *state);
    void sendCommands(UPSState *state);

private:

    bool prevOnBattery;

    unsigned long int updateBattery;

    bool lowBatteryWarn;
};

#endif

