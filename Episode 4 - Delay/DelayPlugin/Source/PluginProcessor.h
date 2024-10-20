/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CircularBuffer.h"


using namespace juce;


//==============================================================================
/**
*/
class DelayPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    DelayPluginAudioProcessor();
    ~DelayPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================

    float CalcDelayTime(float BPM, int syncSetting) 
    {
        float quarterNoteDelay = 60.0 / BPM;
        float fullNoteDelay = quarterNoteDelay * 4;
        float fullNoteDotted = fullNoteDelay * 1.5;
        float fullNoteTriplet = fullNoteDelay / 1.5;
        float halfNoteDelay = quarterNoteDelay * 2;
        float halfNoteDotted = halfNoteDelay * 1.5;
        float halfNoteTriplet = halfNoteDelay / 1.5;
        float quarterNoteDotted = quarterNoteDelay * 1.5;
        float quarterNoteTriplet = quarterNoteDelay / 1.5;
        float eighthNoteDelay = quarterNoteDelay / 2.0;
        float eighthNoteDotted = eighthNoteDelay * 1.5;
        float eighthNoteTripletDelay = quarterNoteDelay / 3.0;
        float sixteenthNoteDelay = quarterNoteDelay / 4.0;
        float sixteenthNoteDotted = sixteenthNoteDelay * 1.5;
        float sixteenthNoteTripletDelay = quarterNoteDelay / 5.0;
        float thirtytwothNoteDelay = quarterNoteDelay / 6.0;


        double delayNotes[16] = { fullNoteDelay, fullNoteDotted, fullNoteTriplet,
            halfNoteDelay, halfNoteDotted, halfNoteTriplet, quarterNoteDelay,
            quarterNoteDotted, quarterNoteTriplet, eighthNoteDelay, eighthNoteDotted,
            eighthNoteTripletDelay, sixteenthNoteDelay, sixteenthNoteDotted, sixteenthNoteTripletDelay,
            thirtytwothNoteDelay };

        return (delayNotes[syncSetting] * 1000);
    }
    

    AudioParameterFloat* Master;
    AudioParameterFloat* Mix;
    AudioParameterFloat* Feedback;
    AudioParameterFloat* DelayTimeMs;

    float FinalDelayTime = 0.0f;


    AudioParameterBool* PingPongEnabled;
    AudioParameterBool* SyncEnabled;

    AudioParameterChoice* SyncSetting;

    AudioPlayHead::CurrentPositionInfo Playhead;

    CircularBuffer BufferL, BufferR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayPluginAudioProcessor)
};
