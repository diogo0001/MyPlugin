#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyPluginAudioProcessorEditor::MyPluginAudioProcessorEditor (MyPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (400, 300);

	auto &params = processor.getParameters();
	AudioParameterFloat *gainParam = (AudioParameterFloat*)params.getUnchecked(0);

	gainControlSlider.setBounds(0, 0, 100, 100);
	gainControlSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	gainControlSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	gainControlSlider.setRange(gainParam->range.start,gainParam->range.end);
	gainControlSlider.setValue(*gainParam);

	// Notify the host that the value is changing
	gainControlSlider.onDragStart = [gainParam] {
		gainParam->beginChangeGesture();
	};

	// Change the value  
	gainControlSlider.onValueChange = [this, gainParam] {
		*gainParam = gainControlSlider.getValue();
	};

	// Notify the host that the value stops changing
	gainControlSlider.onDragEnd = [gainParam] {
		gainParam->endChangeGesture();
	};

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
    g.drawFittedText ("The X Monster Gain", getLocalBounds(), Justification::centred, 1);
}

void MyPluginAudioProcessorEditor::resized() 
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}


