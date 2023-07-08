/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

//==============================================================================
TSPluginAudioProcessor::TSPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ) ,treestate(*this, nullptr, "PARAMETERS",
                           {
                               std::make_unique<AudioParameterFloat>(ParameterID{"DRIVE", 1}, "Drive", NormalisableRange<float>(0.0f, 1.0f), 0.5f)
                               ,std::make_unique<AudioParameterFloat>(ParameterID{"TONE", 1}, "Tone", NormalisableRange<float>(0.0f, 1.0f), 0.5f)
                               ,std::make_unique<AudioParameterFloat>(ParameterID{"LEVEL", 1}, "Level", NormalisableRange<float>(0.0f, 1.0f), 0.5f)
                               ,std::make_unique<AudioParameterChoice>("BYPASS", "Bypass", StringArray("OFF", "ON"), 1)
                           }) /* Because this is a relatively simple plugin with few paramters, we can define our parameters in the APTVS constructor, for larger plugins we typically define a ParameterLayout method and add our parameters to seperate
                          groups based on what the parameters are for*/
#endif
{
    // Initialise our input stage filters, 48000 fs initially, however this will be reset before each playback anyway and will change if the host (DAW) changes sample rate. We can call this method here as the filter type will not change, only the sample rate.
    InputStageHPF_L.Init(HPF, inputStageFc, 48000, 0.5, 0);
    InputStageHPF_R.Init(HPF, inputStageFc, 48000, 0.5, 0);
    
    // Same for tone filters. However, the cutoff of these filters will be modulated at every parameter change so this will be updated in real time.
    ToneFilter_L.Init(LPF, 5000.f, 48000, 0.5, 0);
    ToneFilter_R.Init(LPF, 5000.f, 48000, 0.5, 0);
}

TSPluginAudioProcessor::~TSPluginAudioProcessor()
{
}

//==============================================================================
const juce::String TSPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TSPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TSPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TSPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TSPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TSPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TSPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TSPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TSPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void TSPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TSPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    // Reset our filters with the sample rate
    InputStageHPF_L.Reset(sampleRate);
    InputStageHPF_R.Reset(sampleRate);
    ToneFilter_L.Reset(sampleRate);
    ToneFilter_R.Reset(sampleRate);
    
}

void TSPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TSPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void TSPluginAudioProcessor::getParameters()
{
    // Get parameters from treestate and apply to *std::atomic<float> variables in the case of float parameters and *AudioParameterChoice in the case of the choice parameter.
    psaturation = treestate.getRawParameterValue("DRIVE");
    ptone = treestate.getRawParameterValue("TONE");
    plevel = treestate.getRawParameterValue("LEVEL");
    pbypass = (static_cast<AudioParameterChoice*>(treestate.getParameter("BYPASS")));

    // Map our float parameters to desired bounds, save choice paramter.
    saturation = map(*psaturation, 0.0f, 1.0f, 50.0f, 1000.0f);
    tone = map(*ptone, 0.0f, 1.0f, 1000.f, 6000.f);
    level = map(*plevel, 0.0f, 1.0f, 0.0f, 1.5f);
    bypass = *pbypass;

    // Set frequency cutoff of tone filters.
    ToneFilter_L.SetFc(tone);
    ToneFilter_R.SetFc(tone);

    
}

void TSPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
    
    // Get write pointers to audio buffers.
    float* channelData_L = buffer.getWritePointer(0);
    float* channelData_R = buffer.getWritePointer(1);


    // Save parameters to local plugin variables and set tone filter cutoffs.
    getParameters();
    
    // As we are manually grabbing the channels we want, and we are not using any more than two at any one time we only need to handle the sample numbers. This loop will iterate through all the samples based on the block size of the buffer.
    for (int sample = 0; sample < buffer.getNumSamples(); sample++)
    {
        
        // This will be our DSP if our plugin is mono, it will use one set of our processing objects and the samples for the second (right) channel will be left unused. There are better ways of doing this (dynamic memory allocation) but this is simpler to understand.
        if (totalNumInputChannels == 1 && totalNumOutputChannels == 1)
        {
        
            // Get current input sample from buffer.
            float xn = channelData_L[sample];

            float dry = xn;
            
            // Process input with initial HPF. We also apply a mild sigmoid here to emulate the transistor non-linearity in the buffer.
            xn = mildSigmoid(InputStageHPF_L.ProcessSample(xn));
            
            // Saturate filtered signal with our tanh soft clipper.
            float xn_saturated = tanh(xn * saturation) / tanh(saturation);
            
            // Calculate output sample by applying the tone filter. Gain compensation is also applied due to additional gain applied by the soft clipper. Again apply additional non-linearity after filtering for transistor emulation. 
            float yn = mildSigmoid(ToneFilter_L.ProcessSample(xn_saturated) * gainCompensation);

            // This will check if bypass is activated, if bypass is activated then yn will become the dry input.
            yn = bypass ? dry : (yn * level);
            
            // Write output sample back to buffer.
            channelData_L[sample] = yn;
        
        }
        
        // This will be our DSP if our plugin is mono/stereo. The input sample of the single input channel is duplicated, the processing of each channel is then independant.
        if (totalNumInputChannels == 1 && totalNumOutputChannels == 2)
        {
            // Get current input samples from buffer and duplicate for second channel.
            float xn_L = channelData_L[sample];
            float xn_R = xn_L;

            float dry_L = xn_L;
            float dry_R = xn_R;
            
            // Process inputs with initial HPFs. We also apply a mild sigmoid here to emulate the transistor non-linearity in the buffer.
            xn_L = mildSigmoid(InputStageHPF_L.ProcessSample(xn_L));
            xn_R = mildSigmoid(InputStageHPF_R.ProcessSample(xn_R));
            
            // Saturate filtered signals with our tanh soft clipper.
            float xn_saturated_L = tanh(xn_L * saturation) / tanh(saturation);
            float xn_saturated_R = tanh(xn_R * saturation) / tanh(saturation);
            
            // Calculate output samples by applying the tone filters. Gain compensation is also applied due to additional gain applied by the soft clipper. Again apply additional non-linearity after filtering for transistor emulation. 
            float yn_L = mildSigmoid(ToneFilter_L.ProcessSample(xn_saturated_L) * gainCompensation);
            float yn_R = mildSigmoid(ToneFilter_R.ProcessSample(xn_saturated_R) * gainCompensation);

            // This will check if bypass is activated, if bypass is activated then yn will become the dry input.
            yn_L = bypass ? dry_L : (yn_L * level);
            yn_R = bypass ? dry_R : (yn_R * level);
            
            // Write output samples back to buffers.
            channelData_L[sample] = yn_L;
            channelData_R[sample] = yn_R;
        
        }
        
        // This will be our DSP if our plugin is stereo. This will take the input from both channels and process them independantly.
        if (totalNumInputChannels == 2 && totalNumOutputChannels == 2)
        {
            // Get current input samples from buffers.
            float xn_L = channelData_L[sample];
            float xn_R = channelData_R[sample];

            float dry_L = xn_L;
            float dry_R = xn_R;

            // Process inputs with initial HPFs.  We also apply a mild sigmoid here to emulate the transistor non-linearity in the buffer.
            xn_L = mildSigmoid(InputStageHPF_L.ProcessSample(xn_L));
            xn_R = mildSigmoid(InputStageHPF_R.ProcessSample(xn_R));

            // Saturate filtered signals with our tanh soft clipper.
            float xn_saturated_L = tanh(xn_L * saturation) / tanh(saturation);
            float xn_saturated_R = tanh(xn_R * saturation) / tanh(saturation);

            // Calculate output samples by applying the tone filters. Gain compensation is also applied due to additional gain applied by the soft clipper. Again apply additional non-linearity after filtering for transistor emulation. 
            float yn_L = mildSigmoid(ToneFilter_L.ProcessSample(xn_saturated_L) * gainCompensation);
            float yn_R = mildSigmoid(ToneFilter_R.ProcessSample(xn_saturated_R) * gainCompensation);

            // This will check if bypass is activated, if bypass is activated then yn will become the dry input.
            yn_L = bypass ? dry_L : (yn_L * level);
            yn_R = bypass ? dry_R : (yn_R * level);

            // Write output samples back to buffers.
            channelData_L[sample] = yn_L;
            channelData_R[sample] = yn_R;
        
        }
        
    }
    
}

//==============================================================================
bool TSPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

// We will use the JUCE generic audio processor editor for this project, this means we do not have to write any code for the GUI and we can focus on writing DSP code. You can add your own GUI but i feel it often overcomplicates things and gets in the way of learning DSP as a beginner.
juce::AudioProcessorEditor* TSPluginAudioProcessor::createEditor()
{
    return new GenericAudioProcessorEditor(*this);
}

//==============================================================================
void TSPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TSPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TSPluginAudioProcessor();
}
