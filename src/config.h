
// Analog input pin connected to ZMCT103C current sensor output
#define ZMCT103C_PIN              A0

// Calibration factor for the ZMCT103C module
#define ZMCT103C_CAL              1.26

// Current threshold in amperes, readings below this are considered noise/no charging
#define ZMCT103C_MINCURRENT       0.1

// Analog input pin for battery voltage divider module
#define BATTERY_VOLTAGE_PIN       A1

// Calibration factor for the battery voltage module
#define BATTERY_VOLTAGE_FACTOR    5.07

// Expected battery discharge time in hours under typical load conditions
#define BATTERY_AVGRUNTIME        5.25

struct voltage_map {
    float percentage;
    float voltage;
};

// Curve taken from: https://powmr.com/blogs/news/lifepo4-voltage-chart-and-soc
const voltage_map lfp_table[] = {
    {100.0, 14.60},
    {99.50, 13.80},
    {99.00, 13.52},
    {90.00, 13.40},
    {80.00, 13.32},
    {70.00, 13.20},
    {60.00, 13.12},
    {50.00, 13.04},
    {40.00, 13.00},
    {30.00, 12.92},
    {20.00, 12.80},
    {15.00, 12.20},
    {09.50, 12.00},
    {05.00, 11.20},
    {00.50, 10.16},
    {00.00, 10.00}
};

