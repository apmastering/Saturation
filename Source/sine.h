#pragma once

#include <cmath>
#include <algorithm>
#include <limits>


class SineDistortionAntialiased {

private:

    float driveAmount;
    float previousInput[2];
    float previousOutput[2];
    
    float minInput;
    float maxInput;

    float sineDistortion(float input) const {
        
        return std::sin(input * driveAmount);
    }

    float firstAntiderivative(float input) const {
        
        if (driveAmount == 0.0f) return 0.0f;
        
        if (std::abs(input * driveAmount) < minInput) return 0.0f;
        
        if (std::abs(input * driveAmount) > maxInput) {
            float sign = input > 0 ? 1.0f : -1.0f;
            return maxInput * sign;
        }
        
        return -std::cos(input * driveAmount) / driveAmount;
    }

public:
    
    SineDistortionAntialiased() : driveAmount(1.0f) {
        
        previousInput[0] = 0.0f;
        previousInput[1] = 0.0f;
        previousOutput[0] = 0.0f;
        previousOutput[1] = 0.0f;
        
        minInput = std::numeric_limits<float>::min();
        maxInput = std::acos(-1.0f);
    }

    void setDriveAmount(float newDriveAmount) {
        
        driveAmount = newDriveAmount;
    }

    float process(float inputSample, int channel) {
        
        const float inputChange = inputSample - previousInput[channel];
        float result;

        if (std::abs(inputChange) < 1e-6f) {
            result = sineDistortion(inputSample);
        } else {
            const float currentAntiderivative = firstAntiderivative(previousInput[channel]);
            const float nextAntiderivative = firstAntiderivative(inputSample);
            result = (nextAntiderivative - currentAntiderivative) / inputChange;
            
            if (std::isnan(result)) result = sineDistortion(inputSample);
        }

        result = 0.5f * (result + previousOutput[channel]) + 0.5f * sineDistortion(0.5f * (inputSample + previousInput[channel]));
               
        result *= 0.75f;

        if (std::isnan(result)) result = 0.0f;
        
        const float maxLevel = 2.0f;
        
        result = std::clamp(result, -maxLevel, maxLevel);

        previousInput[channel] = inputSample;
        previousOutput[channel] = result;
        
        return result;
    }
};
