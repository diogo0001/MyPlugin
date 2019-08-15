/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyPluginAudioProcessorEditor::MyPluginAudioProcessorEditor (MyPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

	auto &params = processor.getParameters();
	AudioParameterFloat *gainParam = (AudioParameterFloat*)params.getUnchecked(0);

	gainControlSlider.setBounds(0, 0, 100, 300);
	gainControlSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	gainControlSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	gainControlSlider.setRange(gainParam->range.start,gainParam->range.end);
	gainControlSlider.setValue(*gainParam);
	gainControlSlider.addListener(this);
	addAndMakeVisible(gainControlSlider);
}

MyPluginAudioProcessorEditor::~MyPluginAudioProcessorEditor()
{
}

//==============================================================================
void MyPluginAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("The Monster Gain", getLocalBounds(), Justification::centred, 1);
}

void MyPluginAudioProcessorEditor::resized() 
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void MyPluginAudioProcessorEditor::sliderValueChanged(Slider *slider)
{
	auto &params = processor.getParameters();

	if (slider == &gainControlSlider) {
		AudioParameterFloat *gainParam = (AudioParameterFloat*)params.getUnchecked(0);
		*gainParam = gainControlSlider.getValue();
	}
	
}

