#include "APCommon.h"
#include "PluginProcessor.h"


std::unique_ptr<juce::RangedAudioParameter> newFloatParam(ParameterNames paramName,
                                                          float minValue,
                                                          float maxValue,
                                                          float defaultValue) {
    
    ParameterQuery parameterQuery = queryParameter(paramName);
    
    return std::make_unique<juce::AudioParameterFloat>(juce::ParameterID(parameterQuery.id, static_cast<int>(paramName) + 1),
                                                       parameterQuery.label,
                                                       juce::NormalisableRange<float>(minValue, maxValue),
                                                       defaultValue);
}


std::unique_ptr<juce::RangedAudioParameter> newIntParam(ParameterNames paramName,
                                                        int minValue,
                                                        int maxValue,
                                                        int defaultValue) {
    
    ParameterQuery parameterQuery = queryParameter(paramName);

    
    return std::make_unique<juce::AudioParameterInt>(juce::ParameterID(parameterQuery.id, static_cast<int>(paramName) + 1),
                                                     parameterQuery.id,
                                                     minValue,
                                                     maxValue,
                                                     defaultValue,
                                                     parameterQuery.label);
}


juce::AudioProcessorValueTreeState::ParameterLayout APComp::createParameterLayout() {
    
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(newFloatParam(ParameterNames::inGain,     0.0f,    120.0f,     0.0f  ));
    params.push_back(newFloatParam(ParameterNames::outGain,    -24.0f,   0.0f,     -24.0f ));
    params.push_back(newIntParam(ParameterNames::selection,     0,       6,         0     ));

    return { params.begin(), params.end() };
}
