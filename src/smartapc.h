/*
 *   Basic SmartAPC protocol implementation
 *
 *   Created by 8bitmcu, October 2025.
 *   https://docs.ampnuts.ru/eevblog.docs/eBooks/Apc_Ups_Bible.pdf
 */

#ifndef smartapc_h
#define smartapc_h

struct UPSState {
    float battery_voltage;
    float battery_percent;
    bool on_battery;
    int runtime;
};


class SmartAPC {
public:

    SmartAPC();

    void read_commands(UPSState *state);
    void send_commands(UPSState *state);

    double irms;

private:

    bool prev_on_battery;

    unsigned long int update_battery;

    bool low_battery_warn;

};

#endif

