/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DontDelayAudioProcessorEditor::DontDelayAudioProcessorEditor (DontDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    
    auto& params = processor.getParameters();
    
    AudioParameterFloat* dryWetParameter = (AudioParameterFloat*)params.getUnchecked(0);
    mDryWetSlider.setRange(dryWetParameter->range.start, dryWetParameter->range.end);
    mDryWetSlider.setBounds(0, 0, 100, 100);
    mDryWetSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mDryWetSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mDryWetSlider.setValue(*dryWetParameter);
    addAndMakeVisible(mDryWetSlider);
    
    mDryWetSlider.onValueChange = [this, dryWetParameter]{
        *dryWetParameter = mDryWetSlider.getValue();
    };
    
    mDryWetSlider.onDragStart = [dryWetParameter]{
        dryWetParameter->beginChangeGesture();
    };
    
    mDryWetSlider.onDragEnd = [dryWetParameter]{
        dryWetParameter->endChangeGesture();
    };
    
    AudioParameterFloat* feedbackParameter = (AudioParameterFloat*)params.getUnchecked(1);
    mFeedbackSlider.setRange(feedbackParameter->range.start, feedbackParameter->range.end);
    mFeedbackSlider.setBounds(105, 0, 100, 100);
    mFeedbackSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mFeedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mFeedbackSlider.setValue(*feedbackParameter);
    addAndMakeVisible(mFeedbackSlider);
    
    mFeedbackSlider.onValueChange = [this, feedbackParameter]{
        *feedbackParameter = mFeedbackSlider.getValue();
    };
    
    mFeedbackSlider.onDragStart = [feedbackParameter]{
        feedbackParameter->beginChangeGesture();
    };
    
    mFeedbackSlider.onDragEnd = [feedbackParameter]{
        feedbackParameter->endChangeGesture();
    };
    
    AudioParameterFloat* delayTimeParameter = (AudioParameterFloat*)params.getUnchecked(2);
    mDelayTimeSlider.setRange(delayTimeParameter->range.start, delayTimeParameter->range.end);
    mDelayTimeSlider.setBounds(205, 0, 100, 100);
    mDelayTimeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mDelayTimeSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mDelayTimeSlider.setValue(*delayTimeParameter);
    addAndMakeVisible(mDelayTimeSlider);
    
    mDelayTimeSlider.onValueChange = [this, delayTimeParameter]{
        *delayTimeParameter = mFeedbackSlider.getValue();
    };
    
    mDelayTimeSlider.onDragStart = [delayTimeParameter]{
        delayTimeParameter->beginChangeGesture();
    };
    
    mDelayTimeSlider.onDragEnd = [delayTimeParameter]{
        delayTimeParameter->endChangeGesture();
    };
    
}

DontDelayAudioProcessorEditor::~DontDelayAudioProcessorEditor()
{
}

//==============================================================================
void DontDelayAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void DontDelayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
