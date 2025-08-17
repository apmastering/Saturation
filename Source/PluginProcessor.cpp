#include <cmath>
#include <algorithm>
#include <complex>
#include <array>

#include "APCommon.h"
#include "PluginProcessor.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


APSaturation::APSaturation()
: AudioProcessor(BusesProperties()
                 .withInput("Input", juce::AudioChannelSet::stereo(), true)
                 .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
apvts(*this, nullptr, "PARAMETERS", createParameterLayout()),
inputGain(0),
outputGain(0),
parameterList(static_cast<int>(ParameterNames::END) + 1) {
        
    for (int i = 0; i < static_cast<int>(ParameterNames::END); ++i) {
        
        parameterList[i] = static_cast<juce::AudioParameterFloat*>(apvts.getParameter(queryParameter(static_cast<ParameterNames>(i)).id));
    }
}


void APSaturation::prepareToPlay(double sampleRate, int samplesPerBlock) {
    
    oversamplerReady.store(false);
    
    startOversampler(sampleRate, samplesPerBlock);
}


float APSaturation::getFloatKnobValue(ParameterNames parameter) const {
    
    return parameterList[static_cast<int>(parameter)]->get();
}


void APSaturation::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    
    juce::ScopedNoDenormals noDenormals;

    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    const float gainSmoothingCoefficient = 0.0001f;
    
    const float inputGainValueKnob  = getFloatKnobValue(ParameterNames::inGain);
    const float outputGainValueKnob = getFloatKnobValue(ParameterNames::outGain);
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) buffer.clear(i, 0, buffer.getNumSamples());

    if (!oversamplerReady.load()) return;

    juce::dsp::AudioBlock<float> originalBlock(buffer);
    juce::dsp::AudioBlock<float> mainBlock;
    
    if (totalNumInputChannels < 1) return;
    
    if (totalNumInputChannels == 1) mainBlock = originalBlock.getSingleChannelBlock(0);
    
    if (totalNumInputChannels > 1) mainBlock = originalBlock.getSubsetChannelBlock(0, 2);
    
    juce::dsp::AudioBlock<float> oversampledBlock = oversampler->processSamplesUp (mainBlock);
    
    const int selection = getFloatKnobValue(ParameterNames::selection);
  
    float* channelData[2];
    
    for (int i = 0; i < totalNumInputChannels && i < 2; i++) channelData[i] = oversampledBlock.getChannelPointer(i);
    
    for (int sample = 0; sample < oversampledBlock.getNumSamples(); ++sample) {
        
        for (int channel = 0; channel < 2 && channel < totalNumInputChannels; channel++) {
            
            inputGain  = inputGainValueKnob  * gainSmoothingCoefficient + (1 - gainSmoothingCoefficient) * inputGain;
            outputGain = outputGainValueKnob * gainSmoothingCoefficient + (1 - gainSmoothingCoefficient) * outputGain;
                            
            switch (selection) {
                case static_cast<int>(ButtonName::tanh):
                    doTanhStandard(channelData[channel][sample]);
                    break;
                    
                case static_cast<int>(ButtonName::sine):
                    doSine(channelData[channel][sample]);
                    break;
                    
                case static_cast<int>(ButtonName::hard):
                    doHard(channelData[channel][sample]);
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
                   
                case static_cast<int>(ButtonName::fold):
                    doFold(channelData[channel][sample]);
                    break;
                    
                case static_cast<int>(ButtonName::squaredSine):
                    doSquaredSine(channelData[channel][sample]);
            }
        }
    }
    
    oversampler->processSamplesDown (mainBlock);
}


void APSaturation::doSine (float& sample) {
    
    sample *= decibelsToGain(inputGain);

    sample = std::sin(sample);
        
    sample *= decibelsToGain(outputGain);
}


void APSaturation::doTanhStandard (float& sample) {
    
    sample *= decibelsToGain(inputGain);

    sample = std::tanh(sample);
        
    sample *= decibelsToGain(outputGain);
}


void APSaturation::doHard (float& sample) {
    
    sample *= decibelsToGain(inputGain);

    sample = sample / pow(1 + pow(sample, 8), 1.0 / 8);
    
    sample *= decibelsToGain(outputGain);
}


void APSaturation::doLog (float& sample) {
    
    sample *= decibelsToGain(inputGain);
    
    float sign = sample > 0 ? 1.0f : -1.0f;
        
    sample = std::log(1 + std::abs(sample)) * sign;
    
    sample *= 0.6;
    
    sample = sample / pow(1 + pow(sample, 8), 1.0 / 8);

    sample *= decibelsToGain(outputGain);
}


void APSaturation::doSqrt (float& sample) {

    sample *= decibelsToGain(inputGain);

    float sign = sample > 0 ? 1.0f : -1.0f;

    sample = std::sqrt(std::abs(sample)) * sign;
    sample *= 0.6;
    sample = sample / pow(1 + pow(sample, 8), 1.0 / 8);

    sample *= decibelsToGain(outputGain);
}


void APSaturation::doCube (float& sample) {

    sample *= decibelsToGain(inputGain);

    sample = std::cbrt(sample);
    sample *= 0.5;
    sample = std::sin(sample);
    
    sample *= decibelsToGain(outputGain);
}


void APSaturation::doFold (float& sample) {

    float threshold = 0.9f;
    
    sample *= decibelsToGain(inputGain);

    sample = std::fmod(sample, 2.0f * threshold);
    
    if (sample > threshold) {
        sample = 2.0f * threshold - sample;
    } else if (sample < -threshold) {
        sample = -2.0f * threshold - sample;
    }

    sample *= decibelsToGain(outputGain);
}


void APSaturation::doSquaredSine (float& sample) {
        
    sample *= decibelsToGain(inputGain);

    float sign = sample > 0 ? 1.0f : -1.0f;

    sample = std::sin(sample * sample * sign);
            
    sample *= decibelsToGain(outputGain);
}


void APSaturation::startOversampler(double sampleRate, int samplesPerBlock) {
    
    oversampler.reset();
    
    oversampler = std::make_unique<juce::dsp::Oversampling<float>>(2, 3, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple);
    
    oversampler->initProcessing(static_cast<size_t>(samplesPerBlock));
    oversampler->reset();
    
    setLatencySamples(oversampler->getLatencyInSamples());
         
    oversamplerReady.store(true);
}
