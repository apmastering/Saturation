#pragma once

#include "PluginProcessor.h"


class GUI  : public juce::AudioProcessorEditor, private juce::Timer {

public:
    
    GUI (APSaturation&);
    ~GUI() override;
    
    void paint (juce::Graphics&) override;
    void resized() override {};
    void timerCallback() override;
    ButtonName determineButton(const juce::MouseEvent &event);
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;
	void mouseWheelMove(const juce::MouseEvent& event,
						const juce::MouseWheelDetails& wheel) override;
    
private:
    
    APSaturation& audioProcessor;
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
    const int selectionColumn = 259;
    const int selectionFirstY = 159;
    const int selectionLastY = 446;
    const int numberOfSelections = 8;
    const int selectionRadius = 18;
    const float spacingY = (selectionLastY - selectionFirstY) / (numberOfSelections - 1);
    const int scopeL = 37;
    const int scopeT = 132;
    const int scopeR = 189;
    const int scopeB = 274;
    
    const int mathL = 230;
    const int mathT = 20;
    const int mathR = 451;
    const int mathB = 70;
    
    int previousMouseY;
    
    float snapshotGain;
    
    ButtonName currentButtonSelection;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GUI)
};
