#pragma once

#include <cmath>
#include <algorithm>

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
        
        /*if (std::isnan(previousInput[0])) previousInput[0] = 0.0f;
        if (std::isnan(previousInput[1])) previousInput[1] = 0.0f;
        if (std::isnan(previousOutput[0])) previousOutput[0] = 0.0f;
        if (std::isnan(previousOutput[1])) previousOutput[1] = 0.0f;*/
        
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
