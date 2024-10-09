#pragma once

#include "PluginProcessor.h"


class GUI  : public juce::AudioProcessorEditor, private juce::Timer {

public:
    
    GUI (APComp&);
    ~GUI() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;
    ButtonName determineButton(const juce::MouseEvent &event);
    
private:
    
    APComp& audioProcessor;
                
    juce::Image backgroundImage;
    
    juce::Font customTypeface;
        
    juce::Slider inGainSlider;
    juce::Slider outGainSlider;
    juce::Slider selectionSlider;
            
    std::vector<std::pair<std::string, std::reference_wrapper<juce::Slider>>> sliders {
        {"inGainSlider",        std::ref(inGainSlider)},
        {"outGainSlider",       std::ref(outGainSlider)},
        {"selectionSlider",       std::ref(selectionSlider)},
    };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> selectionAttachment;
    
    const int ioRow1 = 349;
    const int ioRow2 = 430;
    const int ioColumn = 101;
    const int ioRadius = 34;
    
    const int selectionColumn = 260;
    const int selectionFirstY = 162;
    const int selectionLastY = 444;
    const int numberOfSelections = 7;
    const int selectionRadius = 20;

    const float spacingY = (selectionLastY - selectionFirstY) / (numberOfSelections - 1);
    
    int previousMouseY;
    
    ButtonName currentButtonSelection;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GUI)
};
