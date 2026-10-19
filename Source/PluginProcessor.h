#pragma once
#include <JuceHeader.h>
#include <array>
class AURAVOXProAudioProcessor final : public juce::AudioProcessor {
public:
 AURAVOXProAudioProcessor();
 void prepareToPlay(double,int) override;
 void releaseResources() override {}
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 juce::AudioProcessorEditor* createEditor() override;
 bool hasEditor() const override { return true; }
 const juce::String getName() const override { return "AURAVOX Pro"; }
 bool acceptsMidi() const override { return false; }
 bool producesMidi() const override { return false; }
 bool isMidiEffect() const override { return false; }
 double getTailLengthSeconds() const override { return 2.0; }
 int getNumPrograms() override { return 1; }
 int getCurrentProgram() override { return 0; }
 void setCurrentProgram(int) override {}
 const juce::String getProgramName(int) override { return "Modern Vocal"; }
 void changeProgramName(int,const juce::String&) override {}
 void getStateInformation(juce::MemoryBlock&) override;
 void setStateInformation(const void*,int) override;
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 static juce::AudioProcessorValueTreeState::ParameterLayout makeParams();
 juce::AudioProcessorValueTreeState state;
 std::atomic<float> meter {0.0f};
private:
 using Filter=juce::dsp::IIR::Filter<float>;
 std::array<juce::dsp::ProcessorChain<Filter,Filter,juce::dsp::Compressor<float>>,2> chains;
 std::array<juce::dsp::DelayLine<float>,2> delays;
 std::array<float,2> tone {0,0};
 double sampleRateHz=44100.0;
 float lastCut=-1, lastPresence=-1000;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AURAVOXProAudioProcessor)
};
