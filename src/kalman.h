/*
 *   Basic Kalman Filter implementation for estimating
 *   a battery's remaining runtime based on it's SoC.
 *
 *   Created by 8bitmcu, August 2025.
 *   http://en.wikipedia.org/wiki/Kalman_filter
 */

#ifndef kalman_h
#define kalman_h

class KalmanFilter {
public:

    // Initialize Kalman Filter
    void init(double v_full, double v_empty, double est_runtime);

    // Convert voltage to remaining time estimate (simple linear model)
    double voltage_to_time(double voltage, double elapsed_time);

    // Estimate the total capcity left
    double estimate_total_capacity(double current_voltage, double elapsed_time);

    // Kalman Filter prediction step
    void predict(double dt);

    // Kalman Filter update step
    void update(double voltage_measurement, double elapsed_time);

    // Get current remaining time estimate
    double get_remaining_time();

    // Get current uncertainty (standard deviation)
    double get_uncertainty();

private:
    // State variables
    double x;           // State estimate (remaining time in hours)
    double P;           // Error covariance

    // Model parameters
    double Q;           // Process noise covariance
    double R;           // Measurement noise covariance
    double H;           // Measurement matrix (maps state to measurement)
    double F;           // State transition matrix

    // Battery model parameters
    double v_full;      // Full battery voltage
    double v_empty;     // Empty battery voltage
    double est_runtime; // Estimated runtime

    // Dynamic capacity estimation
    double total_elapsed_time;
    double initial_voltage;
    bool first_measurement;
};

#endif

