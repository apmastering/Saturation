#pragma once

#include <vector>

#include "tanh.h"
#include "sine.h"


class APComp  : public juce::AudioProcessor {
    
public:
    
    APComp();
        
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
        
    float getFloatKnobValue(ParameterNames parameter) const;
    
    void doPoly    (float& sample);
    void doCube    (float& sample);
    void doSqrt    (float& sample);
    void doLog     (float& sample);
    void doInverse (float& sample);
    void doSine    (float& sample);
    void doTanh    (float& sample, int channel);

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
private:

    std::vector<juce::AudioParameterFloat*> parameterList;
    
    TanhDistortionAntialiased tanhDistortionAntialiased;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APComp)
};
