
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyPluginAudioProcessor::MyPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	addParameter(gain = new AudioParameterFloat("gain","Gain",0.0f,1.5f,0.5f));
	gainSmoothed = gain->get();
	circularBufferLeft = nullptr;
	circularBufferRight = nullptr;
	circularBufferWriteHead = 0;
	circularBufferLenght = 0;
	delayReadHead = 0;
	delayTimeInSamples = 0;
	delayInSec = 0.2;
	feedbackLeft = 0;
	feedbackRight = 0;
	feedback = 0.5; 
	dryWet = 0.5;
}

MyPluginAudioProcessor::~MyPluginAudioProcessor()
{
	if (circularBufferLeft != nullptr)
	{
		delete [] circularBufferLeft;
		circularBufferLeft = nullptr;
	}

	if (circularBufferRight != nullptr)
	{
		delete [] circularBufferRight;
		circularBufferRight = nullptr;
	}
}

//==============================================================================
const String MyPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MyPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MyPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MyPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MyPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MyPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MyPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MyPluginAudioProcessor::setCurrentProgram (int index)
{
}

const String MyPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void MyPluginAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void MyPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	delayTimeInSamples = sampleRate * delayInSec;
	circularBufferLenght = sampleRate * MAX_DELAY_TIME;

	if (circularBufferLeft == nullptr) 
	{
		circularBufferLeft = new float[circularBufferLenght];
	}
	 
	if (circularBufferRight == nullptr) 
	{
		circularBufferRight = new float[circularBufferLenght];
	}

	circularBufferWriteHead = 0;
	feedbackLeft = 0;
	feedbackRight = 0;
}

void MyPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MyPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MyPluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

  
    auto* channelLeft = buffer.getWritePointer(0);
	auto* channelRight = buffer.getWritePointer(1);

	for (int sample = 0; sample < buffer.getNumSamples(); sample++) 
	{
		gainSmoothed = gainSmoothed - 0.004*(gainSmoothed - gain->get());
		channelLeft[sample] *= gainSmoothed;
		channelRight[sample] *= gainSmoothed;

		circularBufferLeft[circularBufferWriteHead] = channelLeft[sample] + feedbackLeft;
		circularBufferRight[circularBufferWriteHead] = channelRight[sample] + feedbackRight;
		
		delayReadHead = circularBufferWriteHead - delayTimeInSamples;

		if (delayReadHead < 0) {
			delayReadHead += circularBufferLenght;
		}

		float delaySampleLeft = circularBufferLeft[(int)delayReadHead];
		float delaySampleRight = circularBufferRight[(int)delayReadHead];

		feedbackLeft = delaySampleLeft * feedback;
		feedbackRight = delaySampleRight * feedback;

		float outLeftSample = buffer.getSample(0, sample) * dryWet + delaySampleLeft * (1  -dryWet);
		float outRightSample = buffer.getSample(1, sample) * dryWet + delaySampleRight * (1 - dryWet);

		buffer.setSample(0, sample, outLeftSample);
		buffer.setSample(1, sample, outRightSample);

		circularBufferWriteHead++;

		if (circularBufferWriteHead >= circularBufferLenght)
		{
			circularBufferWriteHead = 0;
		}
	}
}

//==============================================================================
bool MyPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MyPluginAudioProcessor::createEditor()
{
    return new MyPluginAudioProcessorEditor (*this);
}

//==============================================================================
void MyPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MyPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyPluginAudioProcessor();
}
