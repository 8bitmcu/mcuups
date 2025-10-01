
// Analog input pin connected to ZMCT103C current sensor output
#define ZMCT103C_PIN              A0

// Calibration factor for the ZMCT103C module
#define ZMCT103C_CAL              1.26

// Current threshold in amperes, readings below this are considered noise/no charging
#define ZMCT103C_MINCURRENT       0.01

// Analog input pin for battery voltage divider module
#define BATTERY_VOLTAGE_PIN       A1

// Calibration factor for the battery voltage module
#define BATTERY_VOLTAGE_FACTOR    5.07

// Battery empty threshold in volts (represents 0% state of charge)
#define BATTERY_MINVOLTAGE        11.2

// Battery full threshold in volts (represents 100% state of charge) 
#define BATTERY_MAXVOLTAGE        14.1

// Expected battery discharge time in hours under typical load conditions
#define BATTERY_AVGRUNTIME        4.0
