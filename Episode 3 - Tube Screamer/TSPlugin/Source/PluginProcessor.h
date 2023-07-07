/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <cmath>
#include <JuceHeader.h>
#include "Biquad.h"


using namespace juce;

//==============================================================================
/**
*/
class TSPluginAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    TSPluginAudioProcessor();
    ~TSPluginAudioProcessor() override;

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
    
    // --- Our audio DSP objects can go here, we will only be using one object type in this project, but we have four filters in total.
    
    Biquad InputStageHPF_L, InputStageHPF_R, ToneFilter_L, ToneFilter_R;
    
    // --- end DSP objects

    // --- Member objects, this is kept seperate from the DSP objects just to keep things clean and make a distinction of what process audio and what doesn't.

    // APVTS which will hold all of our parameter states
    AudioProcessorValueTreeState treestate;

    // --- End member objects
    
    // --- Member variables
    
    float inputStageFc = 300.f;
    float gainCompensation = 0.125;

    std::atomic<float>* psaturation = nullptr, *ptone = nullptr, *plevel = nullptr;
    AudioParameterChoice* pbypass = nullptr;

    float saturation = 0.0f;
    float tone = 0.0f;
    float level = 0.0f;
    bool bypass = false;
    
    // --- end Member variables
    
    // --- Member functions
    
    // Linear mapping function, this will allow us to calculate our current frequency cutoff without exposing the user to the cutoff value. The user will have a parameter between 0 and 1, we will then map that to our upper and lower cutoff bounds.
    float map(float x, float in_min, float in_max, float out_min, float out_max) 
    {
            return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    // This function will get our parameters from the treestate and store them in the plugins member variables
    void getParameters();
    
    // --- end Member funtions
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TSPluginAudioProcessor)
};
