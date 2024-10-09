#pragma once

#include <map>
#include <string>
#include <JuceHeader.h>

#define DEBUG_MODE 0


enum class ButtonName {
    tanh,
    sine,
    inverse,
    log,
    sqrt,
    cube,
    poly,
    input,
    output,
    none
};


enum class ParameterNames {
    inGain,
    outGain,
    selection,
    END
};


struct ParameterQuery {
    std::string id;
    std::string label;
    ParameterNames parameterEnum;
};


double linearToExponential(double linearValue, double minValue, double maxValue);
double gainToDecibels(double gain);
double decibelsToGain(double decibels);
std::string floatToStringWithTwoDecimalPlaces(float value);
ParameterQuery queryParameter(ParameterNames paramName, const std::string& parameterStringName = "");

class APFont {
public:
    static juce::Font getFont() {
        
        static juce::Font customTypeface = createFont();
        return customTypeface;
    }

private:
    static juce::Font createFont() {
        
        auto typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::KnockoutFlyweight_otf, BinaryData::KnockoutFlyweight_otfSize);

        if (typeface != nullptr) {
            
            return juce::Font(juce::FontOptions(typeface));
        }
        
        return juce::Font(juce::FontOptions(14.0f));
    }
};
