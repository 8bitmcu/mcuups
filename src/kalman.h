/*
 *   Basic Kalman Filter implementation for estimating
 *   a battery's remaining runtime based on it's SoC.
 *
 *   Created by 8bitmcu, October 2025.
 *   http://en.wikipedia.org/wiki/Kalman_filter
 */

#ifndef kalman_h
#define kalman_h

class KalmanFilter {
    private:
        float q; // Process noise covariance
        float r; // Measurement noise covariance
        float x; // Estimated value
        float p; // Estimation error covariance
        float k; // Kalman gain
      
    public:
        void init(float process_noise, float sensor_noise, float estimated_error, float initial_value);
        float update(float measurement);
};

#endif

