#include "APCommon.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"


juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new APSaturation(); }
const juce::String APSaturation::getName() const { return JucePlugin_Name; }
bool APSaturation::acceptsMidi() const { return false; }
bool APSaturation::producesMidi() const { return false; }
bool APSaturation::isMidiEffect() const { return false; }
double APSaturation::getTailLengthSeconds() const { return 0.0; }
int APSaturation::getNumPrograms() { return 1; }
int APSaturation::getCurrentProgram() { return 0; }
void APSaturation::setCurrentProgram (int index) { }
const juce::String APSaturation::getProgramName (int index) { return {}; }
void APSaturation::changeProgramName (int index, const juce::String& newName) {}
bool APSaturation::hasEditor() const { return true; }
void APSaturation::releaseResources() {}
bool APSaturation::isBusesLayoutSupported (const BusesLayout& layouts) const {
    const auto& mainInput  = layouts.getMainInputChannelSet();
    const auto& mainOutput = layouts.getMainOutputChannelSet();

    if (mainInput == juce::AudioChannelSet::mono() &&
        mainOutput == juce::AudioChannelSet::mono()) {
        return true;
    }

    if (mainInput == juce::AudioChannelSet::stereo() &&
        mainOutput == juce::AudioChannelSet::stereo()) {
        return true;
    }

    return false;
}
juce::AudioProcessorEditor* APSaturation::createEditor() { return new GUI (*this); }

void APSaturation::getStateInformation (juce::MemoryBlock& destData) {
    
    std::unique_ptr<juce::XmlElement> xml (apvts.state.createXml());
    copyXmlToBinary (*xml, destData);
}

void APSaturation::setStateInformation (const void* data, int sizeInBytes) {
    
        std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
        if (xml != nullptr)
        {
            if (xml->hasTagName (apvts.state.getType()))
            {
                apvts.state = juce::ValueTree::fromXml (*xml);
            }
        }
}
