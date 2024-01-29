/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"


//==============================================================================
DelayPluginAudioProcessor::DelayPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(Master = new juce::AudioParameterFloat("MASTER", // parameterID
        "Master", // parameter name
        0.0f,   // minimum value
        1.0f,   // maximum value
        0.5f)); // default value

    addParameter(Mix = new juce::AudioParameterFloat("MIX", // parameterID
        "Mix", // parameter name
        0.0f,   // minimum value
        1.0f,   // maximum value
        0.5f)); // default value

    addParameter(Feedback = new juce::AudioParameterFloat("FEEDBACK", // parameterID
        "Feedback", // parameter name
        0.0f,   // minimum value
        1.0f,   // maximum value
        0.5f)); // default value

    addParameter(DelayTimeMs = new juce::AudioParameterFloat("DELAYTIMEMS", // parameterID
        "DelayTimeMs", // parameter name
        20.0f,   // minimum value
        2000.0f,   // maximum value
        100.f)); // default value

    addParameter(PingPongEnabled = new juce::AudioParameterBool("PINGPONGENABLED", "PingPongEnabled", false));

}


DelayPluginAudioProcessor::~DelayPluginAudioProcessor()
{
}

//==============================================================================
const juce::String DelayPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DelayPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DelayPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DelayPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DelayPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void DelayPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DelayPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    BufferL.Init(sampleRate, 2000.f);
    BufferR.Init(sampleRate, 2000.f);

}

void DelayPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void DelayPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // In case we have more outputs than inputs, this code clears any output
     // channels that didn't contain input data, (because these aren't
     // guaranteed to be empty - they may contain garbage).
     // This is here to avoid people getting screaming feedback
     // when they first compile a plugin, but obviously you don't need to keep
     // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Get write pointers to audio buffers.
    float* channelData_L = buffer.getWritePointer(0);
    float* channelData_R = buffer.getWritePointer(1);


    // As we are manually grabbing the channels we want, and we are not using any more than two at any one time we only need to handle the sample numbers. This loop will iterate through all the samples based on the block size of the buffer.
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {

        // This will be our DSP if our plugin is mono, it will use one set of our processing objects and the samples for the second (right) channel will be left unused. There are better ways of doing this (dynamic memory allocation) but this is simpler to understand.
        if (totalNumInputChannels == 1 && totalNumOutputChannels == 1)
        {

            // Get current input sample from buffer.
            float xn = channelData_L[sample];

            float dry = xn;

            float ynminusD = BufferL.BufferRead(*DelayTimeMs, true);

            float bufferinput = xn + *Feedback * ynminusD;

            BufferL.BufferWrite(bufferinput);

            float output = (dry * (1.f - *Mix)) + (*Mix * ynminusD);

            channelData_L[sample] = output * *Master;

        }

        // This will be our DSP if our plugin is mono/stereo. The input sample of the single input channel is duplicated, the processing of each channel is then independant.
        if (totalNumInputChannels == 1 && totalNumOutputChannels == 2)
        {
            // Get current input samples from buffer and duplicate for second channel.
            float xn_L = channelData_L[sample];
            float xn_R = xn_L;

            float dry_L = xn_L;
            float dry_R = xn_R;

            float ynminusD_L = BufferL.BufferRead(*DelayTimeMs, true);
            float ynminusD_R = BufferR.BufferRead(*DelayTimeMs, true);

            float bufferinput_L = xn_L + *Feedback * ynminusD_L;
            float bufferinput_R = xn_R + *Feedback * ynminusD_R;

            if (PingPongEnabled->get() != true)
            {

                BufferL.BufferWrite(bufferinput_L);
                BufferR.BufferWrite(bufferinput_R);

            }
            else if (PingPongEnabled->get() == true)
            {

                BufferL.BufferWrite(bufferinput_R);
                BufferR.BufferWrite(bufferinput_L);

            }

            float output_L = (dry_L * (1.f - *Mix)) + (*Mix * ynminusD_L);
            float output_R = (dry_R * (1.f - *Mix)) + (*Mix * ynminusD_R);

            channelData_L[sample] = output_L * *Master;
            channelData_R[sample] = output_R * *Master;
        
        }

        // This will be our DSP if our plugin is stereo. This will take the input from both channels and process them independantly.
        if (totalNumInputChannels == 2 && totalNumOutputChannels == 2)
        {

            // Get current input samples from buffers.
            float xn_L = channelData_L[sample];
            float xn_R = channelData_R[sample];

            float dry_L = xn_L;
            float dry_R = xn_R;

            float ynminusD_L = BufferL.BufferRead(*DelayTimeMs, true);
            float ynminusD_R = BufferR.BufferRead(*DelayTimeMs, true);

            float bufferinput_L = xn_L + *Feedback * ynminusD_L;
            float bufferinput_R = xn_R + *Feedback * ynminusD_R;

            if (PingPongEnabled->get() != true)
            {

                BufferL.BufferWrite(bufferinput_L);
                BufferR.BufferWrite(bufferinput_R);

            }
            else if (PingPongEnabled->get() == true)
            {

                BufferL.BufferWrite(bufferinput_R);
                BufferR.BufferWrite(bufferinput_L);

            }

            float output_L = (dry_L * (1.f - *Mix)) + (*Mix * ynminusD_L);
            float output_R = (dry_R * (1.f - *Mix)) + (*Mix * ynminusD_R);

            channelData_L[sample] = output_L * *Master;
            channelData_R[sample] = output_R * *Master;

        }

    }
}

//==============================================================================
bool DelayPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayPluginAudioProcessor::createEditor()
{
    return new GenericAudioProcessorEditor (*this);
}

//==============================================================================
void DelayPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayPluginAudioProcessor();
}
