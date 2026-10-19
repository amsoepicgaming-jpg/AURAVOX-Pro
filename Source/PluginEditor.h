#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <vector>
class AuravoxLookAndFeel final : public juce::LookAndFeel_V4 {
public:
 void drawRotarySlider(juce::Graphics&,int,int,int,int,float,float,float,juce::Slider&) override;
 void drawButtonBackground(juce::Graphics&,juce::Button&,const juce::Colour&,bool,bool) override;
};
class AURAVOXProAudioProcessorEditor final : public juce::AudioProcessorEditor,private juce::Timer {
public:
 explicit AURAVOXProAudioProcessorEditor(AURAVOXProAudioProcessor&);
 ~AURAVOXProAudioProcessorEditor() override;
 void paint(juce::Graphics&) override;
 void resized() override;
private:
 using Attachment=juce::AudioProcessorValueTreeState::SliderAttachment;
 struct Knob {std::unique_ptr<juce::Slider> slider;std::unique_ptr<juce::Label> label;std::unique_ptr<Attachment> attachment;};
 AURAVOXProAudioProcessor& processor;
 AuravoxLookAndFeel look;
 std::vector<Knob> knobs;
 juce::TextButton bypass{"BYPASS"}, modern{"MODERN VOCAL"},radio{"RADIO READY"},warm{"WARM & SMOOTH"};
 std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
 void addKnob(const char*,const char*);
 void loadPreset(int);
 void timerCallback() override {repaint(1028,180,110,370);}
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AURAVOXProAudioProcessorEditor)
};
