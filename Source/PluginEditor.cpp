#include <algorithm>
#include <mutex>

#include "APCommon.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"


GUI::GUI (APComp& p)
: AudioProcessorEditor (&p),
audioProcessor (p),
backgroundImage (juce::ImageFileFormat::loadFrom(BinaryData::saturation_png, BinaryData::saturation_pngSize)),
customTypeface (APFont::getFont()),
inGainSlider(),
outGainSlider(),
selectionSlider(),
inGainAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "inGain", inGainSlider)),
outGainAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "outGain", outGainSlider)),
selectionAttachment (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "selection", selectionSlider)),
currentButtonSelection(ButtonName::none) {
          
    for (size_t i = 0; i < sliders.size(); ++i) {
        
        juce::Slider& slider = sliders[i].second.get();

        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        addAndMakeVisible(slider);
        slider.setVisible(false);
    }
    
    setSize (460, 490);
    
    const int refreshRate = 33;
    startTimer(refreshRate);
}


GUI::~GUI() {
    
    stopTimer();
}


void GUI::paint (juce::Graphics& g) {
    
    if (backgroundImage.isValid()) {
        g.drawImage(backgroundImage, getLocalBounds().toFloat());
    } else {
        g.fillAll(juce::Colours::lightgrey);
        g.setColour (juce::Colours::black);
        g.setFont (24.0f);
        g.drawFittedText ("AP Mastering - Saturation Distortion: GUI error", getLocalBounds(), juce::Justification::centredTop, 1);
    }
            
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    
    const int selection = audioProcessor.getFloatKnobValue(ParameterNames::selection);

    if (selection >= static_cast<int>(ButtonName::none)) return;
    
    if (selection < 0) return;
    
    g.fillEllipse(selectionColumn - selectionRadius,
                  selectionFirstY - selectionRadius + spacingY * selection,
                  selectionRadius * 2,
                  selectionRadius * 2);
    
    const float inputGainValue  = audioProcessor.getFloatKnobValue(ParameterNames::inGain);
    const float outputGainValue = audioProcessor.getFloatKnobValue(ParameterNames::outGain);
    
    std::string inputGainText  = floatToStringWithTwoDecimalPlaces(inputGainValue);
    std::string outputGainText = floatToStringWithTwoDecimalPlaces(outputGainValue);
    
    customTypeface.setHeight(40.0f);
    g.setFont(customTypeface);
    g.setColour (juce::Colours::white.withAlpha(0.6f));

    g.drawFittedText(inputGainText,
                     ioColumn - ioRadius,
                     ioRow1 - ioRadius,
                     ioRadius * 2,
                     ioRadius * 2,
                     juce::Justification::centred,
                     1);
    
    g.drawFittedText(outputGainText,
                     ioColumn - ioRadius,
                     ioRow2 - ioRadius,
                     ioRadius * 2,
                     ioRadius * 2,
                     juce::Justification::centred,
                     1);

    const int scopeWidth = scopeR - scopeL;
    const int scopeHeight = scopeB - scopeT;
    const int granularity = 100;
    const float grainWidth = scopeWidth / static_cast<float>(granularity);

    g.setColour(juce::Colours::black.withAlpha(0.7f));
    
    juce::Path path;
    juce::Path path2;
    
    for (int i = 0; i < granularity; ++i) {
        
        float y = scopeB - scopeHeight * 0.5f;
        float inputAmplitude = (i - granularity * 0.5f) / 20;
        float outputAmplitude = 0;
        float sign = inputAmplitude > 0 ? 1.0f : -1.0f;

        switch (selection) {
            case static_cast<int>(ButtonName::tanh):
                outputAmplitude = tanh(inputAmplitude);
                outputAmplitude *= -1.0f;
                break;
                
            case static_cast<int>(ButtonName::sine):
                outputAmplitude = sin(inputAmplitude);
                outputAmplitude *= -1.0f;
                break;
                
            case static_cast<int>(ButtonName::inverse):
                outputAmplitude = inputAmplitude / pow(1 + pow(inputAmplitude, 8), 1.0 / 8);
                outputAmplitude *= -1.0f;
                break;
                
            case static_cast<int>(ButtonName::log):
                outputAmplitude = std::log(1.0f + std::abs(inputAmplitude)) * -sign;
                break;
                
            case static_cast<int>(ButtonName::sqrt):
                outputAmplitude = std::sqrt(std::abs(inputAmplitude)) * sign;
                outputAmplitude *= -0.5;
                outputAmplitude = std::tanh(outputAmplitude);
                break;
                
            case static_cast<int>(ButtonName::cube):
                outputAmplitude = std::cbrt(inputAmplitude);
                outputAmplitude *= -0.5;
                outputAmplitude = std::tanh(outputAmplitude);
                break;
               
            case static_cast<int>(ButtonName::poly):
                                    
                const float D = 0.3f;
                const float E = 0.4f;
                const float F = 0.2f;
                
                outputAmplitude = -sign * D * inputAmplitude * inputAmplitude + E * inputAmplitude + F;
                outputAmplitude *= 0.5;
                outputAmplitude = std::tanh(outputAmplitude);
        }
        
        y += outputAmplitude / 2.5f * scopeHeight;
        
        if (y < scopeT) y = scopeT;
        if (y > scopeB) y = scopeB;
        
        if (i == 0) {
            
            path.startNewSubPath(scopeL, y);
            path2.startNewSubPath(scopeL, scopeT + (scopeB - scopeT) / 2);
            
            continue;
        }
 
        path.lineTo(scopeL + (i * grainWidth), y);
        path2.lineTo(scopeL + (i * grainWidth), scopeT + (scopeB - scopeT) / 2);
    }
    
    g.strokePath(path, juce::PathStrokeType(4.0f));
    g.strokePath(path2, juce::PathStrokeType(2.0f));
}


void GUI::resized() {}


void GUI::timerCallback() {
    
    repaint();
}


ButtonName GUI::determineButton(const juce::MouseEvent &event) {
        
    if (event.x > selectionColumn - selectionRadius &&
        event.x < selectionColumn + selectionRadius) {
        
        for (int i = 0; i < static_cast<int>(ButtonName::none); ++i) {
            
            if (event.y > selectionFirstY - selectionRadius + spacingY * i &&
                event.y < selectionFirstY + selectionRadius + spacingY * i) {

                return static_cast<ButtonName>(i);
            }
        }
    }
    
    if (event.x > ioColumn - ioRadius &&
        event.x < ioColumn + ioRadius) {
        
        if (event.y > ioRow1 - ioRadius &&
            event.y < ioRow1 + ioRadius) {
            
            return ButtonName::input;
        }
        
        if (event.y > ioRow2 - ioRadius &&
            event.y < ioRow2 + ioRadius) {
            
            return ButtonName::output;
        }
    }
    
    return ButtonName::none;
}


void GUI::mouseDown (const juce::MouseEvent& event) {
    
    previousMouseY = event.position.y;
    
    currentButtonSelection = determineButton(event);
    
    if (currentButtonSelection == ButtonName::none) return;
    if (currentButtonSelection == ButtonName::input) return;
    if (currentButtonSelection == ButtonName::output) return;

    selectionSlider.setValue(static_cast<int>(currentButtonSelection));
}


void GUI::mouseDrag (const juce::MouseEvent& event) {
    
    if (currentButtonSelection != ButtonName::input &&
        currentButtonSelection != ButtonName::output)
        return;
        
    const float delta = (previousMouseY - event.position.y) * 0.1;

    if (currentButtonSelection == ButtonName::input) {
        
        const float inputGainValue  = audioProcessor.getFloatKnobValue(ParameterNames::inGain);

        inGainSlider.setValue(inputGainValue + delta);
    }
    
    if (currentButtonSelection == ButtonName::output) {
        
        const float outputGainValue = audioProcessor.getFloatKnobValue(ParameterNames::outGain);

        outGainSlider.setValue(outputGainValue + delta);
    }
    
    previousMouseY = event.position.y;
}


void GUI::mouseUp (const juce::MouseEvent& event) {
    
    currentButtonSelection = ButtonName::none;
}
