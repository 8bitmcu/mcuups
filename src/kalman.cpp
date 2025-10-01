#include "kalman.h"
#include <math.h>


// Initialize Kalman Filter
void KalmanFilter::init(double v_full, double v_empty, double est_runtime) {
    // Initial state
    x = est_runtime;
    P = 1.0;  // Initial uncertainty

    // Process and measurement noise (tuning parameters)
    Q = 0.001;  // Process noise: how much we expect the model to be wrong (lower = more trust)
    R = 0.05;   // Measurement noise: voltage measurement uncertainty (lower = more trust)

    // Model matrices
    F = 1.0;   // Assuming constant discharge rate for simplicity
    H = 1.0;   // Direct measurement relationship

    // Battery parameters
    this->v_full = v_full;
    this->v_empty = v_empty;
    this->est_runtime = est_runtime;

    // Dynamic capacity tracking
    this->total_elapsed_time = 0.0;
    this->initial_voltage = 0.0;
    this->first_measurement = true;
}

// Estimate total capacity based on discharge rate
double KalmanFilter::estimate_total_capacity(double current_voltage, double elapsed_time) {
    if (elapsed_time <= 0.0 || initial_voltage <= v_empty || current_voltage <= v_empty) {
        return est_runtime; // Default fallback capacity
    }

    // Calculate voltage drop and discharge rate
    double voltage_drop = initial_voltage - current_voltage;
    double total_voltage_range = v_full - v_empty;

    if (voltage_drop <= 0.0) {
        return est_runtime; // Default fallback if no discharge observed
    }

    // Estimate total capacity based on current discharge rate
    // If we've dropped voltage_drop in elapsed_time, 
    // total time would be: elapsed_time * (total_voltage_range / voltage_drop)
    double estimated_total_capacity = elapsed_time * (total_voltage_range / voltage_drop);

    // Apply reasonable bounds (e.g., between 0.1 second and 100 hours)
    if (estimated_total_capacity < 0.1) {
        estimated_total_capacity = 0.1;
    }
    if (estimated_total_capacity > 100.0 * 60 * 60) {
        estimated_total_capacity = 100.0 * 60 * 60;
    }

    return estimated_total_capacity;
}

// Convert voltage to remaining time estimate using dynamic capacity
double KalmanFilter::voltage_to_time(double voltage, double elapsed_time) {
    if (voltage <= v_empty) {
        return 0.0;
    }
    if (voltage >= v_full) {
        // If at full voltage, estimate based on total capacity
        return estimate_total_capacity(voltage, elapsed_time);
    }

    // Get current estimated total capacity
    double estimated_capacity = estimate_total_capacity(voltage, elapsed_time);

    // Linear interpolation between empty and full
    double voltage_range = v_full - v_empty;
    double voltage_ratio = (voltage - v_empty) / voltage_range;

    return voltage_ratio * estimated_capacity;
}

// Kalman Filter Prediction Step
void KalmanFilter::predict(double dt) {
    // Predict next state (remaining time decreases by dt)
    x = F * x - dt;

    // Ensure remaining time doesn't go negative
    if (x < 0.0) {
        x = 0.0;
    }

    // Predict error covariance
    P = F * P * F + Q;

    // Update total elapsed time
    total_elapsed_time += dt;
}

// Kalman Filter Update Step
void KalmanFilter::update(double voltage_measurement, double elapsed_time) {
    // Handle first measurement
    if (first_measurement) {
        initial_voltage = voltage_measurement;
        total_elapsed_time = 0.0;

        // Initialize state with voltage-based estimate
        x = voltage_to_time(voltage_measurement, 1.0); // Use 1.0 as placeholder for first measurement
        first_measurement = false;
        return;
    }

    // Convert voltage measurement to time estimate
    double z = voltage_to_time(voltage_measurement, total_elapsed_time);

    // Innovation (measurement residual)
    double y = z - H * x;

    // Innovation covariance
    double S = H * P * H + R;

    // Kalman gain
    double K = P * H / S;

    // Update state estimate
    x = x + K * y;

    // Update error covariance
    P = (1.0 - K * H) * P;

    // Ensure remaining time is within reasonable bounds
    if (x < 0.0) {
        x = 0.0;
    }

    double max_capacity = estimate_total_capacity(voltage_measurement, total_elapsed_time);
    if (x > max_capacity) {
        x = max_capacity;
    }
}

// Get current remaining time estimate
double KalmanFilter::get_remaining_time() {
    return x;
}

// Get current uncertainty (standard deviation)
double KalmanFilter::get_uncertainty() {
    return sqrt(P);
}

