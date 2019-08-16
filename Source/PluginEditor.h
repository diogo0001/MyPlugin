#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


class MyPluginAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    MyPluginAudioProcessorEditor (MyPluginAudioProcessor&);
    ~MyPluginAudioProcessorEditor();

    void paint (Graphics&) override;
    void resized() override;

private:
	Slider gainControlSlider;
    MyPluginAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyPluginAudioProcessorEditor)
};
