#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
AURAVOXProAudioProcessor::AURAVOXProAudioProcessor()
: AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),state(*this,nullptr,"PARAMETERS",makeParams()) {}
juce::AudioProcessorValueTreeState::ParameterLayout AURAVOXProAudioProcessor::makeParams() {
 juce::AudioProcessorValueTreeState::ParameterLayout p;
 auto add=[&](const char* id,const char* name,float low,float high,float def,float step=0.01f) {p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id,1},name,juce::NormalisableRange<float>(low,high,step),def));};
 add("input","Input Gain",-24,24,0);add("hpf","Low Cut",40,250,90,1);add("presence","Presence",-12,12,2);
 add("threshold","Threshold",-40,0,-18);add("ratio","Ratio",1,10,3);add("attack","Attack",1,100,15);add("release","Release",30,500,120,1);
 add("drive","Warmth",0,100,18,1);add("delay","Echo Mix",0,50,8,1);add("delaytime","Echo Time",60,750,250,1);
 add("output","Output Gain",-24,24,0);add("mix","Dry Wet",0,100,100,1);
 p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"bypass",1},"Bypass",false));return p;
}
bool AURAVOXProAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const {
 auto o=l.getMainOutputChannelSet();return (o==juce::AudioChannelSet::mono()||o==juce::AudioChannelSet::stereo()) && l.getMainInputChannelSet()==o;
}
void AURAVOXProAudioProcessor::prepareToPlay(double rate,int block) {
 sampleRateHz=rate;lastCut=-1;lastPresence=-1000;
 juce::dsp::ProcessSpec spec{rate,static_cast<juce::uint32>(block),1};
 for(int ch=0;ch<2;++ch){chains[ch].prepare(spec);chains[ch].reset();delays[ch].setMaximumDelayInSamples(static_cast<int>(rate));delays[ch].prepare(spec);delays[ch].reset();tone[ch]=0;}
 meter.store(0);
}
void AURAVOXProAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,juce::MidiBuffer&) {
 juce::ScopedNoDenormals guard;
 auto v=[&](const char* id){return state.getRawParameterValue(id)->load();};
 auto channels=juce::jmin(2,buffer.getNumChannels());
 if(channels==0)return;
 if(v("bypass")>0.5f){meter.store(buffer.getMagnitude(0,buffer.getNumSamples()));return;}
 const float in=juce::Decibels::decibelsToGain(v("input")),out=juce::Decibels::decibelsToGain(v("output"));
 const float mix=v("mix")*0.01f,echo=v("delay")*0.01f,drive=v("drive")*0.01f;
 const float driveGain=1.0f+drive*6.0f;
 if(v("hpf")!=lastCut){auto c=juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRateHz,v("hpf"));for(auto& chain:chains)*chain.get<0>().coefficients=*c;lastCut=v("hpf");}
 if(v("presence")!=lastPresence){auto c=juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRateHz,3200.0f,0.8f,juce::Decibels::decibelsToGain(v("presence")));for(auto& chain:chains)*chain.get<1>().coefficients=*c;lastPresence=v("presence");}
 for(int ch=0;ch<channels;++ch){
  auto& comp=chains[ch].get<2>();comp.setThreshold(v("threshold"));comp.setRatio(v("ratio"));comp.setAttack(v("attack"));comp.setRelease(v("release"));
  delays[ch].setDelay(juce::jlimit(1.0f,static_cast<float>(sampleRateHz*0.95),static_cast<float>(sampleRateHz*v("delaytime")/1000.0)));
 }
 float peak=0;
 for(int ch=0;ch<channels;++ch){
  auto* data=buffer.getWritePointer(ch);auto& hp=chains[ch].get<0>();auto& bell=chains[ch].get<1>();auto& comp=chains[ch].get<2>();
  for(int n=0;n<buffer.getNumSamples();++n){
   float dry=data[n];float wet=comp.processSample(0,bell.processSample(hp.processSample(dry*in)));
   if(drive>0.0001f)wet=std::tanh(wet*driveGain)/std::tanh(driveGain);
   float delayed=delays[ch].popSample(0);tone[ch]+=0.23f*(delayed-tone[ch]);delays[ch].pushSample(0,wet+tone[ch]*0.28f);
   wet=(wet+tone[ch]*echo)*out;data[n]=dry*(1-mix)+wet*mix;peak=juce::jmax(peak,std::abs(data[n]));
  }
 }
 meter.store(juce::jmax(peak,meter.load()*0.88f));
}
void AURAVOXProAudioProcessor::getStateInformation(juce::MemoryBlock& b){auto xml=state.copyState().createXml();copyXmlToBinary(*xml,b);}
void AURAVOXProAudioProcessor::setStateInformation(const void* data,int bytes){if(auto xml=getXmlFromBinary(data,bytes))if(xml->hasTagName(state.state.getType()))state.replaceState(juce::ValueTree::fromXml(*xml));}
juce::AudioProcessorEditor* AURAVOXProAudioProcessor::createEditor(){return new AURAVOXProAudioProcessorEditor(*this);}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new AURAVOXProAudioProcessor();}
