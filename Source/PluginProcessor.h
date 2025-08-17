#pragma once

#include <vector>


class APSaturation  : public juce::AudioProcessor {
    
public:
    
    APSaturation();
        
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
    
    void doSquaredSine  (float& sample);
    void doFold         (float& sample);
    void doCube         (float& sample);
    void doSqrt         (float& sample);
    void doLog          (float& sample);
    void doHard         (float& sample);
    void doSine         (float& sample);
    void doTanhStandard (float& sample);

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
private:

    float previousSample;
    
    void startOversampler(double sampleRate, int samplesPerBlock);

    std::atomic<bool> oversamplerReady;
    
    float inputGain;
    float outputGain;
                
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
    
    std::vector<juce::AudioParameterFloat*> parameterList;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (APSaturation)
};
