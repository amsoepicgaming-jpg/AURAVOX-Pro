#include "PluginEditor.h"
#include <cmath>
namespace {
const auto bg=juce::Colour(0xff080d14), panel=juce::Colour(0xff131b26),gold=juce::Colour(0xffffb96a),blue=juce::Colour(0xff66b8ff),muted=juce::Colour(0xff96a9bc);
void section(juce::Graphics& g,juce::Rectangle<float> r,const juce::String& title){
 g.setColour(panel);g.fillRoundedRectangle(r,13.0f);g.setColour(juce::Colour(0xff324052));g.drawRoundedRectangle(r,13.0f,1.0f);
 g.setColour(gold);g.setFont(juce::Font(juce::FontOptions(17.0f,juce::Font::bold)));g.drawText(title,r.toNearestInt().reduced(20).removeFromTop(34),juce::Justification::centredLeft);
}
}
void AuravoxLookAndFeel::drawRotarySlider(juce::Graphics& g,int x,int y,int w,int h,float pos,float start,float end,juce::Slider& slider){
 auto b=juce::Rectangle<float>(float(x),float(y),float(w),float(h)).reduced(10.0f);float radius=juce::jmin(b.getWidth(),b.getHeight())*0.42f;auto c=b.getCentre();
 float angle=start+pos*(end-start);auto accent=slider.getColour(juce::Slider::rotarySliderFillColourId);
 juce::Path ring;ring.addCentredArc(c.x,c.y,radius,radius,0,start,end,true);g.setColour(juce::Colour(0xff2c3747));g.strokePath(ring,juce::PathStrokeType(6.0f));
 juce::Path arc;arc.addCentredArc(c.x,c.y,radius,radius,0,start,angle,true);g.setColour(accent);g.strokePath(arc,juce::PathStrokeType(5.0f));
 g.setColour(juce::Colour(0xff070b10));g.fillEllipse(c.x-radius+9,c.y-radius+9,2*(radius-9),2*(radius-9));
 g.setColour(juce::Colours::white);g.setFont(juce::Font(juce::FontOptions(19.0f)));
 g.drawFittedText(juce::String(slider.getValue(),slider.getInterval()>=1?0:1),juce::Rectangle<int>(int(c.x-radius),int(c.y-14),int(radius*2),28),juce::Justification::centred,1);
}
void AuravoxLookAndFeel::drawButtonBackground(juce::Graphics& g,juce::Button& button,const juce::Colour&,bool hover,bool down){
 auto r=button.getLocalBounds().toFloat().reduced(1);g.setColour(down?gold.darker(0.3f):hover?juce::Colour(0xff293a4e):juce::Colour(0xff192432));
 g.fillRoundedRectangle(r,7);g.setColour(button.getToggleState()?gold:juce::Colour(0xff41536a));g.drawRoundedRectangle(r,7,1.5f);
}
AURAVOXProAudioProcessorEditor::AURAVOXProAudioProcessorEditor(AURAVOXProAudioProcessor& p):AudioProcessorEditor(&p),processor(p){
 setLookAndFeel(&look);
 for(const auto& entry:std::initializer_list<std::pair<const char*,const char*>>{
  {"input","INPUT"},{"hpf","LOW CUT"},{"presence","PRESENCE"},{"threshold","THRESHOLD"},{"ratio","RATIO"},{"attack","ATTACK"},{"release","RELEASE"},{"drive","WARMTH"},{"delay","ECHO"},{"delaytime","ECHO TIME"},{"output","OUTPUT"},{"mix","DRY / WET"}
 })addKnob(entry.first,entry.second);
 bypass.setClickingTogglesState(true);addAndMakeVisible(bypass);bypassAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.state,"bypass",bypass);
 for(auto* button:{&modern,&radio,&warm})addAndMakeVisible(*button);
 modern.onClick=[this]{loadPreset(0);};radio.onClick=[this]{loadPreset(1);};warm.onClick=[this]{loadPreset(2);};
 setSize(1180,730);startTimerHz(25);
}
AURAVOXProAudioProcessorEditor::~AURAVOXProAudioProcessorEditor(){setLookAndFeel(nullptr);}
void AURAVOXProAudioProcessorEditor::addKnob(const char* id,const char* name){
 Knob k;k.slider=std::make_unique<juce::Slider>();k.slider->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);k.slider->setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);
 k.slider->setColour(juce::Slider::rotarySliderFillColourId,knobs.size()%3==1?blue:gold);
 k.label=std::make_unique<juce::Label>();k.label->setText(name,juce::dontSendNotification);k.label->setJustificationType(juce::Justification::centred);
 k.label->setColour(juce::Label::textColourId,muted);k.label->setFont(juce::Font(juce::FontOptions(13.0f,juce::Font::bold)));
 addAndMakeVisible(*k.slider);addAndMakeVisible(*k.label);k.attachment=std::make_unique<Attachment>(processor.state,id,*k.slider);knobs.push_back(std::move(k));
}
void AURAVOXProAudioProcessorEditor::loadPreset(int index){
 using S=std::pair<const char*,float>;
 const std::vector<S> presets[3]={
 {{"input",0},{"hpf",90},{"presence",2},{"threshold",-18},{"ratio",3},{"attack",15},{"release",120},{"drive",18},{"delay",8},{"delaytime",250},{"output",0},{"mix",100}},
 {{"input",0},{"hpf",115},{"presence",4.5f},{"threshold",-25},{"ratio",4.5f},{"attack",8},{"release",100},{"drive",12},{"delay",0},{"delaytime",180},{"output",0},{"mix",100}},
 {{"input",0},{"hpf",75},{"presence",1},{"threshold",-16},{"ratio",2.5f},{"attack",28},{"release",190},{"drive",34},{"delay",15},{"delaytime",340},{"output",-1},{"mix",100}}
 };
 for(const auto& [id,value]:presets[index])if(auto* param=processor.state.getParameter(id))param->setValueNotifyingHost(param->convertTo0to1(value));
}
void AURAVOXProAudioProcessorEditor::paint(juce::Graphics& g){
 g.fillAll(bg);juce::ColourGradient gradient(juce::Colour(0xff162336),500,90,bg,590,700,true);g.setGradientFill(gradient);g.fillRect(getLocalBounds());
 g.setColour(gold);g.setFont(juce::Font(juce::FontOptions(42.0f,juce::Font::bold)));g.drawText("A",29,15,60,64,juce::Justification::centred);
 g.setColour(juce::Colours::white);g.setFont(juce::Font(juce::FontOptions(28.0f)));g.drawText("A U R A V O X",95,17,350,38,juce::Justification::centredLeft);
 g.setColour(gold);g.setFont(juce::Font(juce::FontOptions(14.0f,juce::Font::bold)));g.drawText("P R O   /   V O C A L   S U I T E",98,54,360,24,juce::Justification::centredLeft);
 g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(13.0f)));g.drawText("REAL-TIME VOCAL ENHANCEMENT",755,28,400,35,juce::Justification::centredRight);
 g.setColour(gold.withAlpha(0.55f));g.drawLine(26,93,1154,93,1.2f);
 section(g,{25,112,355,445},"01   TONE & CLARITY");section(g,{397,112,355,445},"02   VOCAL DYNAMICS");section(g,{769,112,385,445},"03   SPACE & OUTPUT");section(g,{25,575,1129,127},"VOCAL PRESETS");
 g.setColour(muted);g.setFont(juce::Font(juce::FontOptions(12.0f)));g.drawText("Clean tone, intelligibility, warmth",45,150,320,20,juce::Justification::centredLeft);
 g.drawText("Threshold, ratio, response",417,150,320,20,juce::Justification::centredLeft);g.drawText("Echo, balance, level",789,150,310,20,juce::Justification::centredLeft);
 g.setColour(juce::Colour(0xff1d2e3f));g.fillRoundedRectangle(1032.0f,200.0f,71.0f,282.0f,8.0f);
 float level=juce::jlimit(0.0f,1.0f,processor.meter.load());float db=juce::Decibels::gainToDecibels(level,-60.0f);float amount=juce::jlimit(0.0f,1.0f,(db+60.0f)/60.0f);
 g.setColour(juce::Colour(0xff62e6af));g.fillRoundedRectangle(1052.0f,464.0f-243.0f*amount,30.0f,243.0f*amount,4.0f);
 g.setColour(juce::Colours::white);g.setFont(juce::Font(juce::FontOptions(12.0f)));g.drawText(juce::String(db,1)+" dB",1013,490,115,30,juce::Justification::centred);
 g.setColour(muted);g.drawText("LEVEL",1030,183,86,20,juce::Justification::centred);
 g.drawText("AURAVOX PRO • VST3 EFFECT • NO PITCH CORRECTION IN THIS VERSION",32,705,1070,18,juce::Justification::centredLeft);
}
void AURAVOXProAudioProcessorEditor::resized(){
 if(knobs.size()!=12)return;
 auto place=[this](int i,int x,int y,int w=136){knobs[(size_t)i].slider->setBounds(x,y,w,118);knobs[(size_t)i].label->setBounds(x,y+113,w,23);};
 place(0,56,187);place(1,218,187);place(2,132,345);place(3,429,185);place(4,591,185);place(5,429,340);place(6,591,340);
 place(7,796,185);place(8,914,185);place(9,795,340);place(10,914,340,110);place(11,1033,340,108);
 bypass.setBounds(1028,116,112,36);modern.setBounds(50,622,310,55);radio.setBounds(431,622,310,55);warm.setBounds(811,622,310,55);
}
