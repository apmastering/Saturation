#pragma once

#include <algorithm>
#include <complex>
#include <cmath>
#include <array>

constexpr double PI = 3.14159265358979323846;


class TanhDistortionAntialiased {
private:
    
    float driveAmount;
    float previousInput[2];
    float previousOutput[2];
    
    float minInput;
    float maxInput;

    float tanhDistortion(float input) const {
        
        return std::tanh(input * driveAmount);
    }

    float firstAntiderivative(float input) const {
        
        if (driveAmount == 0.0f) return 0.0f;
        
        if (std::abs(input * driveAmount) < minInput) return 0.0f;
        if (std::abs(input * driveAmount) > maxInput) {
            
            float sign = input > 0 ? 1.0f : -1.0f;
            
            return maxInput * sign;
        }
        
        return std::log(std::cosh(input * driveAmount)) / driveAmount;
    }

public:
        
    TanhDistortionAntialiased() : driveAmount(1.0f) {
        
        previousInput[0] = 0.0f;
        previousInput[1] = 0.0f;
        previousOutput[0] = 0.0f;
        previousOutput[1] = 0.0f;
        
        minInput = std::numeric_limits<float>::min();
        maxInput = std::log(std::numeric_limits<float>::max());
    }

    void setDriveAmount(float newDriveAmount) {
        driveAmount = newDriveAmount;
    }

    float process(float inputSample, int channel) {
        
        const float inputChange = inputSample - previousInput[channel];
        float result;

        if (std::abs(inputChange) < 1e-6f) {
            
            result = tanhDistortion(inputSample);
            
        } else {

            const float currentAntiderivative = firstAntiderivative(previousInput[channel]);
            const float nextAntiderivative = firstAntiderivative(inputSample);
            result = (nextAntiderivative - currentAntiderivative) / inputChange;
            
            if (std::isnan(result)) result = tanhDistortion(inputSample);
        }

        result = 0.5f * (result + previousOutput[channel]) + 0.5f * tanhDistortion(0.5f * (inputSample + previousInput[channel]));
               
        result *= 0.75;

        if (std::isnan(result)) result = 0.0f;
        
        const float maxLevel = 2.0f;
        
        if (result > maxLevel) result = maxLevel;
        if (result < -maxLevel) result = -maxLevel;

        previousInput[channel] = inputSample;
        previousOutput[channel] = result;
        
        return result;
    }
};





class StereoAAIIRFilter {
private:
    std::vector<std::array<double, 2>> state;
    std::vector<double> coeffs;
    int order;
    double sampleRate;
    double cutoffFreq;

    double tanhAntiderivative(double x) {
        return std::log(std::cosh(x));
    }

    void calculateCoefficients() {
        double omega = 2.0 * M_PI * cutoffFreq / sampleRate;
        double alpha = std::sin(omega) / (2.0 * order);
        
        coeffs.resize(order + 1);
        for (int i = 0; i <= order; ++i) {
            coeffs[i] = std::pow(alpha, i + 1);
        }
    }

    // Helper function to check for NaN
    bool isNaN(double x) {
        return std::isnan(x) || std::isinf(x);
    }

    // Helper function to clamp values
    double clamp(double x, double min, double max) {
        return std::max(min, std::min(x, max));
    }

public:
    StereoAAIIRFilter(int order, double sampleRate, double cutoffFreq)
        : order(order), sampleRate(sampleRate), cutoffFreq(cutoffFreq) {
        reset();
        calculateCoefficients();
    }

    void reset() {
        state = std::vector<std::array<double, 2>>(order + 1, {0.0, 0.0});
    }
    
    
    float process(float input, int channel) {
        double y = 0.0;

        double antidev = tanhAntiderivative(input);

        if (isNaN(antidev)) {
            antidev = 0.0;
        }

        // Update state and compute output
        for (int i = 0; i <= order; ++i) {
            state[i][channel] = antidev - state[i][channel];
            
            // Check for NaN in state
            if (isNaN(state[i][channel])) {
                state[i][channel] = 0.0;
            }

            y += coeffs[i] * state[i][channel];
        }

        if (isNaN(y)) {
            y = 0.0;
        }

        // Compute derivative approximation
        double output = (y - state[0][channel]) * sampleRate;

        // Update state for next iteration
        for (int i = order; i > 0; --i) {
            state[i][channel] = state[i-1][channel];
        }
        state[0][channel] = y;

        return static_cast<float>(std::tanh(output));
    }

    void setCutoffFrequency(double newCutoffFreq) {
        cutoffFreq = clamp(newCutoffFreq, 20.0, sampleRate / 2.0);
        calculateCoefficients();
    }
};
