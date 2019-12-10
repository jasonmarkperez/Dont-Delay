/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DontDelayAudioProcessor::DontDelayAudioProcessor()
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
    addParameter(mDryWetParameter = new AudioParameterFloat("drywet", "Dry Wet", 0.02, 1.0, 0.5));
    addParameter(mFeedbackParameter = new AudioParameterFloat("feedback", "Feedback", 0, 0.98, 0.5));
    addParameter(mDelayTimeParameter = new AudioParameterFloat("delay", "Delay Time", 0.0, MAX_DELAY_TIME, 0.5));

    mCircularBufferLeft = nullptr;
    mCircularBufferRight = nullptr;

    mCircularBufferWriteHead = 0;
    mCircularBufferLength = 0;
    mDelayReadHead = 0;
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
    mDelayTimeSmoothed = 0;
}

DontDelayAudioProcessor::~DontDelayAudioProcessor()
{
    if(mCircularBufferLeft != nullptr){
        delete [] mCircularBufferLeft;
        mCircularBufferLeft = nullptr;
    }
    
    if(mCircularBufferRight != nullptr){
        delete [] mCircularBufferRight;
        mCircularBufferRight = nullptr;
    }
    
}

//==============================================================================
const String DontDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DontDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DontDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DontDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DontDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DontDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DontDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DontDelayAudioProcessor::setCurrentProgram (int index)
{
}

const String DontDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void DontDelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DontDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mCircularBufferLength = sampleRate * MAX_DELAY_TIME;
    mDelayTimeInSamples = sampleRate * *mDelayTimeParameter;
    
    if(mCircularBufferLeft == nullptr){
        mCircularBufferLeft = new float[mCircularBufferLength];
    }
    
    if(mCircularBufferRight == nullptr){
        mCircularBufferRight = new float[mCircularBufferLength];
    }
    
    zeromem(mCircularBufferLeft, mCircularBufferLength * sizeof(float));
    zeromem(mCircularBufferRight, mCircularBufferLength * sizeof(float));
    
    mCircularBufferWriteHead = 0;
    mDelayTimeSmoothed = *mDelayTimeParameter;
    
}

void DontDelayAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DontDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void DontDelayAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    
    float *leftChannel = buffer.getWritePointer(0);
    float *rightChannel = buffer.getWritePointer(1);
    // Get the current write pointers for the left and right channels
    
    for(int i = 0; i < buffer.getNumSamples(); i++, mCircularBufferWriteHead++){
        
        mDelayTimeSmoothed = mDelayTimeSmoothed - 0.001 * (mDelayTimeSmoothed - *mDelayTimeParameter);
        mDelayTimeInSamples = getSampleRate() * mDelayTimeSmoothed;
        
        mCircularBufferLeft[mCircularBufferWriteHead] = leftChannel[i] + mFeedbackLeft;
        mCircularBufferRight[mCircularBufferWriteHead] = rightChannel[i] + mFeedbackRight;
        // Place the current sample from the buffer into the circular buffer
        
        mDelayReadHead = mCircularBufferWriteHead - mDelayTimeInSamples;
        // Set the read head backwards from the write head, by the number of samples you want to delay.
        
        if(mDelayReadHead < 0) mDelayReadHead += mCircularBufferLength;
        
        int readhead_x = (int)mDelayReadHead;
        int readhead_x1 = (int)readhead_x + 1;
        
        float readHeadFloat = mDelayReadHead - readhead_x;
        
        if(readhead_x1 >= mCircularBufferLength){
            readhead_x1 -= mCircularBufferLength;
        }
        
        // Construct the samples
        float leftDelaySample = linInterpolation(mCircularBufferLeft[readhead_x], mCircularBufferLeft[readhead_x1], readHeadFloat);
        float rightDelaySample = linInterpolation(mCircularBufferRight[readhead_x], mCircularBufferRight[readhead_x1], readHeadFloat);
        
        
        mFeedbackLeft = leftDelaySample * *mFeedbackParameter;
        mFeedbackRight = rightDelaySample * *mFeedbackParameter;
        
        buffer.setSample(0, i, buffer.getSample(0, i) * ( 1 - *mDryWetParameter) + leftDelaySample * *mDryWetParameter);
        buffer.setSample(1, i, buffer.getSample(1, i) * ( 1 - *mDryWetParameter) + rightDelaySample * *mDryWetParameter);
        if(mCircularBufferWriteHead >= mCircularBufferLength) mCircularBufferWriteHead = 0;
    }
}

//==============================================================================
bool DontDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DontDelayAudioProcessor::createEditor()
{
    return new DontDelayAudioProcessorEditor (*this);
}

//==============================================================================
void DontDelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DontDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

float DontDelayAudioProcessor::linInterpolation(float sample_x, float sample_x1, float inPhase) {
    return (1 - inPhase) * sample_x + inPhase * sample_x1;
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DontDelayAudioProcessor();
}
