#include <cmath>

#include "APCommon.h"
#include "PluginProcessor.h"


APComp::APComp()
: AudioProcessor(BusesProperties()
                 .withInput("Input", juce::AudioChannelSet::quadraphonic(), true)
                 .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
apvts(*this, nullptr, "PARAMETERS", createParameterLayout()),
parameterList(static_cast<int>(ParameterNames::END) + 1) {
        
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
    
    const float inputGainValue  = getFloatKnobValue(ParameterNames::inGain);
    const float outputGainValue = getFloatKnobValue(ParameterNames::outGain);
    const int   selection       = getFloatKnobValue(ParameterNames::selection);
  
    float* channelData[2];
    
    for (int i = 0; i < totalNumInputChannels && i < 2; i++) channelData[i] = buffer.getWritePointer(i);
    
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        
        for (int channel = 0; channel < 2 && channel < totalNumInputChannels; channel++) {
                
            channelData[channel][sample] *= decibelsToGain(inputGainValue);

            switch (selection) {
                case static_cast<int>(ButtonName::tanh):
                    channelData[channel][sample] = tanh(channelData[channel][sample]);
                    break;
                    
                case static_cast<int>(ButtonName::sine):
                    channelData[channel][sample] = sin(channelData[channel][sample]);
                    break;
                    
                case static_cast<int>(ButtonName::inverse):
                    channelData[channel][sample] = channelData[channel][sample] / pow(1 + pow(channelData[channel][sample], 8), 1.0 / 8);
                    break;
                    
                case static_cast<int>(ButtonName::log):
                    channelData[channel][sample] = 4 * std::log(1 + std::abs(channelData[channel][sample])) * channelData[channel][sample] > 0 ? 1 : -1;
                    break;
                    
                case static_cast<int>(ButtonName::sqrt):
                    channelData[channel][sample] = std::sqrt(std::abs(channelData[channel][sample])) * channelData[channel][sample] > 0 ? 1 : -1;
                    break;
                    
                case static_cast<int>(ButtonName::cube):
                    channelData[channel][sample] = std::cbrt(channelData[channel][sample]);
                    break;
                   
                case static_cast<int>(ButtonName::poly):
                     const double D = 0.5;
                     const double E = 1.0;
                     const double F = 1.0;
                    
                     channelData[channel][sample] = D * channelData[channel][sample] * channelData[channel][sample] + E * channelData[channel][sample] + F;
            }
            
            channelData[channel][sample] *= decibelsToGain(outputGainValue);
        }
    }
}
