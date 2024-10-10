#include <cmath>

#include "APCommon.h"
#include "PluginProcessor.h"


APComp::APComp()
: AudioProcessor(BusesProperties()
                 .withInput("Input", juce::AudioChannelSet::quadraphonic(), true)
                 .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
apvts(*this, nullptr, "PARAMETERS", createParameterLayout()),
parameterList(static_cast<int>(ParameterNames::END) + 1),
tanhDistortionAntialiased() {
        
    for (int i = 0; i < static_cast<int>(ParameterNames::END); ++i) {
        
        parameterList[i] = static_cast<juce::AudioParameterFloat*>(apvts.getParameter(queryParameter(static_cast<ParameterNames>(i)).id));
    }
}


void APComp::prepareToPlay(double sampleRate, int samplesPerBlock) {
}


float APComp::getFloatKnobValue(ParameterNames parameter) const {
    
    return parameterList[static_cast<int>(parameter)]->get();
}


void APComp::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    
    juce::ScopedNoDenormals noDenormals;

    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) buffer.clear(i, 0, buffer.getNumSamples());
    
    const int   selection       = getFloatKnobValue(ParameterNames::selection);
  
    float* channelData[2];
    
    for (int i = 0; i < totalNumInputChannels && i < 2; i++) channelData[i] = buffer.getWritePointer(i);
    
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        
        for (int channel = 0; channel < 2 && channel < totalNumInputChannels; channel++) {
                            
            switch (selection) {
                case static_cast<int>(ButtonName::tanh):
                    doTanh(channelData[channel][sample], channel);
                    break;
                    
                case static_cast<int>(ButtonName::sine):
                    doSine(channelData[channel][sample]);
                    break;
                    
                case static_cast<int>(ButtonName::inverse):
                    doInverse(channelData[channel][sample]);
                    break;
                    
                case static_cast<int>(ButtonName::log):
                    doLog(channelData[channel][sample]);
                    break;
                    
                case static_cast<int>(ButtonName::sqrt):
                    doSqrt(channelData[channel][sample]);
                    break;
                    
                case static_cast<int>(ButtonName::cube):
                    doCube(channelData[channel][sample]);
                    break;
                   
                case static_cast<int>(ButtonName::poly):
                    doPoly(channelData[channel][sample]);
            }
        }
    }
}


void APComp::doTanh (float& sample, int channel) {
    
    const float inputGainValue  = getFloatKnobValue(ParameterNames::inGain);
    const float outputGainValue = getFloatKnobValue(ParameterNames::outGain);
    
    tanhDistortionAntialiased.setDriveAmount(decibelsToGain(inputGainValue));
    
    sample = tanhDistortionAntialiased.process(sample, channel);

    sample *= decibelsToGain(outputGainValue);
}


void APComp::doSine (float& sample) {
    
    const float inputGainValue  = getFloatKnobValue(ParameterNames::inGain);
    const float outputGainValue = getFloatKnobValue(ParameterNames::outGain);

    sample *= decibelsToGain(inputGainValue);

    sample = std::sin(sample);
        
    sample *= decibelsToGain(outputGainValue);
}


void APComp::doInverse (float& sample) {
    
    const float inputGainValue  = getFloatKnobValue(ParameterNames::inGain);
    const float outputGainValue = getFloatKnobValue(ParameterNames::outGain);

    sample *= decibelsToGain(inputGainValue);

    sample = sample / pow(1 + pow(sample, 8), 1.0 / 8);
    
    sample *= decibelsToGain(outputGainValue);
}


void APComp::doLog (float& sample) {
    
    const float inputGainValue  = getFloatKnobValue(ParameterNames::inGain);
    const float outputGainValue = getFloatKnobValue(ParameterNames::outGain);

    float sign = sample > 0 ? 1.0f : -1.0f;

    sample *= decibelsToGain(inputGainValue);
        
    sample = std::log(1 + std::abs(sample)) * sign;
    
    sample *= 0.4;
    
    sample = std::tanh(sample);
        
    sample *= decibelsToGain(outputGainValue);
}


void APComp::doSqrt (float& sample) {
    
    const float inputGainValue  = getFloatKnobValue(ParameterNames::inGain);
    const float outputGainValue = getFloatKnobValue(ParameterNames::outGain);

    float sign = sample > 0 ? 1.0f : -1.0f;

    sample *= decibelsToGain(inputGainValue);

    sample = std::sqrt(std::abs(sample)) * sign;
    sample *= 0.4;
    sample = std::tanh(sample);
    
    sample *= decibelsToGain(outputGainValue);
}


void APComp::doCube (float& sample) {
    
    const float inputGainValue  = getFloatKnobValue(ParameterNames::inGain);
    const float outputGainValue = getFloatKnobValue(ParameterNames::outGain);

    sample *= decibelsToGain(inputGainValue);

    sample = std::cbrt(sample);
    sample *= 0.5;
    sample = std::sin(sample);
    
    sample *= decibelsToGain(outputGainValue);
}


void APComp::doPoly (float& sample) {
    
    const float inputGainValue  = getFloatKnobValue(ParameterNames::inGain);
    const float outputGainValue = getFloatKnobValue(ParameterNames::outGain);

    const float D = 0.4f;
    const float E = 0.4f;
    const float F = 0.0f;

    float sign = sample > 0 ? 1.0f : -1.0f;

    if (sample == 0) return;
    
    sample *= decibelsToGain(inputGainValue);
    
    sample = sign * D * sample * sample + E * sample + F;
    float inputScaling = decibelsToGain(inputGainValue);
    inputScaling *= 0.2;
    if (inputScaling > 1.0f) sample /= inputScaling;
    sample = std::sin(sample);
    
    sample *= decibelsToGain(outputGainValue);
}
