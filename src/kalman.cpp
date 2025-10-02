#include "kalman.h"
#include <math.h>

void KalmanFilter::init(float process_noise, float sensor_noise, float estimated_error, float initial_value) {
    q = process_noise;
    r = sensor_noise;
    p = estimated_error;
    x = initial_value;
}

float KalmanFilter::update(float measurement) {
    // Prediction
    p = p + q;

    // Update
    k = p / (p + r);
    x = x + k * (measurement - x);
    p = (1 - k) * p;

    return x;
}
