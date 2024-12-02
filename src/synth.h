#ifndef Synth_h
#define Synth_h

#include <Audio.h>
#include "Voice.h"

// **** CHANGES 1/5 ************************************************************
#include "hexefx_audio_I16.h"
//#include "effect_platervbstereo.h"
// **** END CHANGES 1/5 ********************************************************

#ifndef ULLONG_MAX
#define ULLONG_MAX 0xffffffffffffffff
#endif

// Number of voices
const byte voiceCount = 4; // max = 16
const byte voicesPerMixer = 4;
const byte mixerCount = voiceCount/voicesPerMixer + voiceCount%voicesPerMixer > 0? 1:0;
const uint16_t maxDelayBlocks{100};
const uint16_t delayTimeMS = maxDelayBlocks*3;
//const byte mixerCount = 4;

/*
 * Synth
 */
class Synth{
  
  private:
    Voice *voices[voiceCount];
    AudioMixer4 *mixersVoices0[mixerCount];
    AudioMixer4 *mixerSynOut;

    AudioMixer4 *delaySendL;
    AudioMixer4 *delaySendR;

    AudioEffectDelay *delayL;
    AudioEffectDelay *delayR;
    //AudioAmplifier *delayL;
    //AudioAmplifier *delayR;

    AudioMixer4 *reverbSendL;
    AudioMixer4 *reverbSendR;

    // **** CHANGES 2/5 ********************************************************
    //AudioAmplifier *reverb; // WORKS
    AudioEffectPlateReverb_i16 *reverb; // Errno6: Device not configured    
    //AudioEffectPlateReverb *reverb; // ALSO WORKS
    //AudioEffectSpringReverb_i16 *reverb; // Errno6: Device not configured
    //AudioEffectReverb *reverb;
    
    // **** END CHANGES 2/5 ****************************************************

    AudioMixer4 *efxReturnsL;
    AudioMixer4 *efxReturnsR;

    AudioAmplifier *outputL;
    AudioAmplifier *outputR;

    AudioConnection* patchCordsVoices[mixerCount + voiceCount]; 
    
    AudioConnection *patchCordDelaySendDirectL;
    AudioConnection *patchCordDelaySendDirectR;
    AudioConnection *patchCordDelaySendFdbkL;
    AudioConnection *patchCordDelaySendFdbkR;
    AudioConnection *patchCordDelayL;
    AudioConnection *patchCordDelayR;
    AudioConnection *patchCordReverbSendDelayL;
    AudioConnection *patchCordReverbSendDelayR;
    AudioConnection *patchCordReverbSendDirectL;
    AudioConnection *patchCordReverbSendDirectR;
    AudioConnection *patchCordReverbL;
    AudioConnection *patchCordReverbR;
    AudioConnection *patchCordDirectRetL;
    AudioConnection *patchCordDirectRetR;
    AudioConnection *patchCordReverbRetL;
    AudioConnection *patchCordReverbRetR;
    AudioConnection *patchCordDelayRetL;
    AudioConnection *patchCordDelayRetR;
    AudioConnection *patchCordSendOutL;
    AudioConnection *patchCordSendOutR;

    
  public:
    Synth();

    void noteOn(byte channel, byte note, byte velocity);
    void noteOff(byte channel, byte note, byte velocity);
    void controlChange(byte channel, byte control, byte value);
    void pitchChange(byte channel, int pitch);
    void afterTouch(byte channel, byte pressure);
    Voice **getVoices();

    AudioAmplifier * getOutputL();
    AudioAmplifier * getOutputR();

};

/**
 * Constructor
 */

// create and mix synth voices
inline Synth::Synth(){
  this->mixerSynOut = new AudioMixer4();
  this->mixerSynOut->gain(0, 1.0/float(mixerCount));
  this->mixerSynOut->gain(1, 1.0/float(mixerCount));
  this->mixerSynOut->gain(2, 1.0/float(mixerCount));
  this->mixerSynOut->gain(3, 1.0/float(mixerCount));

  for (int i = 0; i < mixerCount; i++) {
    this->mixersVoices0[i] = new AudioMixer4();
    this->mixersVoices0[i]->gain(0, 0.4);
    this->mixersVoices0[i]->gain(1, 0.4);
    this->mixersVoices0[i]->gain(2, 0.4);
    this->mixersVoices0[i]->gain(3, 0.4);
    
    this->patchCordsVoices[i] = 
      new AudioConnection(*this->mixersVoices0[i], 0, *this->mixerSynOut, i%voicesPerMixer);
  }
  
  for (int i = 0; i < voiceCount; i++) {
    this->voices[i] = new Voice();
    this->patchCordsVoices[i] = 
      new AudioConnection(*this->voices[i]->getOutput(), 0, *this->mixersVoices0[i/voicesPerMixer], i%voicesPerMixer);
  }

  // EFX chain   
  this->delaySendL = new AudioMixer4();
  this->delaySendR = new AudioMixer4();

  this->delaySendL->gain(0, 1.0); // direct in
  this->delaySendR->gain(0, 1.0);
  this->delaySendL->gain(1, 0.1); // feedback
  this->delaySendR->gain(1, 0.1);

  this->delayL = new AudioEffectDelay();
  this->delayR = new AudioEffectDelay();
  //this->delayL = new AudioAmplifier();
  //this->delayR = new AudioAmplifier();

  this->reverbSendL = new AudioMixer4();
  this->reverbSendR = new AudioMixer4();
  
  this->reverbSendL->gain(0, 0.5); // direct in
  this->reverbSendL->gain(1, 0.5); // delay in
  this->reverbSendR->gain(0, 0.5);
  this->reverbSendR->gain(1, 0.5);
  
  // ***** CHANGES 3/5  ********************************************************
  //this->reverb = new AudioAmplifier(); //  WORKS
  //this->reverb = new AudioEffectPlateReverb(); // WORKS
  //this->reverb = new AudioEffectSpringReverb_i16(); // Errno6
  this->reverb = new AudioEffectPlateReverb_i16(); // errno6
  //this->reverb = new AudioEffectReverb();
  // ***** END 3/5 *************************************************************

  this->outputL = 
    new AudioAmplifier();
  this->outputL->gain(1.0);
  //this->patchCordSendOutL = 
  //  new AudioConnection(*this->mixerSynOut, 0, *this->outputL, 0);
  
  this->outputR = 
    new AudioAmplifier();
  this->outputR->gain(1.0);

  this->efxReturnsL = new AudioMixer4();
  this->efxReturnsL->gain(0, 0.3);
  this->efxReturnsL->gain(1, 0.5);
  this->efxReturnsL->gain(2, 0.3);

  this->efxReturnsR = new AudioMixer4();
  this->efxReturnsR->gain(0, 0.3);
  this->efxReturnsR->gain(1, 0.5);
  this->efxReturnsR->gain(2, 0.3);

  
  this->patchCordDelaySendDirectL = 
    new AudioConnection(*this->mixerSynOut, 0, *this->delaySendL, 0);
  this->patchCordDelaySendDirectR = 
    new AudioConnection(*this->mixerSynOut, 0, *this->delaySendR, 0);
  this->patchCordDelaySendFdbkL = 
    new AudioConnection(*this->delayL, 0, *this->delaySendL, 1);
  this->patchCordDelaySendFdbkR = 
    new AudioConnection(*this->delayR, 0, *this->delaySendR, 1);
  
  this->patchCordDelayL = 
    new AudioConnection(*this->delaySendL, 0, *this->delayL, 0);
  this->patchCordDelayR = 
    new AudioConnection(*this->delaySendR, 0, *this->delayR, 0);
  
  this->patchCordReverbSendDirectL = 
    new AudioConnection(*this->mixerSynOut, 0, *this->reverbSendL, 0);
  this->patchCordReverbSendDirectR = 
    new AudioConnection(*this->mixerSynOut, 0, *this->reverbSendR, 0);
  
  this->patchCordReverbSendDelayL = 
    new AudioConnection(*this->delayL, 0, *this->reverbSendL, 0);
  this->patchCordReverbSendDelayR = 
    new AudioConnection(*this->delayR, 0, *this->reverbSendR, 0);
  
  this->patchCordReverbL = 
    new AudioConnection(*this->reverbSendL, 0, *this->reverb, 0);

  // CHANGES 4/5 ***************************************************************
  
  // Option 1
  //this->patchCordReverbR = 
  //  new AudioConnection(*this->reverbSendR, 0, *this->reverb, 0);
  
  // Options 2/3
  this->patchCordReverbR = 
    new AudioConnection(*this->reverbSendR, 0, *this->reverb, 1);
  // END CHANGES 4/5 ***********************************************************

  this->patchCordDirectRetL = 
    new AudioConnection(*this->mixerSynOut, 0, *this->efxReturnsL, 0);
  this->patchCordDelayRetL = 
    new AudioConnection(*this->delayL, 0, *this->efxReturnsL, 1);
  this->patchCordReverbRetL = 
    new AudioConnection(*this->reverb, 0, *this->efxReturnsL, 2);

  this->patchCordDirectRetR = 
    new AudioConnection(*this->mixerSynOut, 0, *this->efxReturnsR, 0);
  this->patchCordDelayRetR = 
    new AudioConnection(*this->delayR, 0, *this->efxReturnsR, 1);
  this->patchCordReverbRetR = 
    new AudioConnection(*this->reverb, 1, *this->efxReturnsR, 2);


  this->patchCordSendOutL = 
    new AudioConnection(*this->efxReturnsL, *this->outputL);

  this->patchCordSendOutR = 
    new AudioConnection(*this->efxReturnsR, *this->outputR);  

  //this->patchCordSendOutR = 
  //  new AudioConnection(*this->mixerSynOut, 0, *this->outputR, 0);

  // Configure Delay
  
  this->delayL->delay(0, delayTimeMS);
  this->delayR->delay(0, delayTimeMS);
  this->delayL->disable(1);
  this->delayR->disable(1);
  this->delayL->disable(2);
  this->delayR->disable(2);
  this->delayL->disable(3);
  this->delayR->disable(3);
  this->delayL->disable(4);
  this->delayR->disable(4);
  this->delayL->disable(5);
  this->delayR->disable(5);
  this->delayL->disable(6);
  this->delayR->disable(6);
  this->delayL->disable(7);
  this->delayR->disable(7);
  
  // Configure Reverb

  // **** CHANGES 5/5 **********************************************************

  //this->reverb->gain(1.0); // use when reverb is AudioAmplifier; Works
  
  // Use when reverb is AudioEffectPlateReverb, AudioEffectPlateReverb_i16, or AudioEffectSpringReverb_i16
  this->reverb->size(1.0);     // max reverb length
  this->reverb->lowpass(0.3);  // sets the reverb master lowpass filter
  this->reverb->lodamp(0.1);   // amount of low end loss in the reverb tail
  this->reverb->hidamp(0.2);   // amount of treble loss in the reverb tail
  this->reverb->diffusion(1.0);  // 1.0 is the detault setting, lower it to create more "echoey" reverb
  
  // Use only for _i16 effects
  this->reverb->wet_level(1.0f); 
  this->reverb->bypass_set(false);
  
  // **** END CHANGES 5/5 ******************************************************
  

}

/**
 * Note on
 */
inline void Synth::noteOn(byte channel, byte note, byte velocity){
  bool foundOne = false;
  int oldestVoice = 0;
  unsigned long oldestVoiceTime = ULLONG_MAX;

  for (int i = 0; i < voiceCount; i++) {
    // Search for the oldest voice
    if(this->voices[i]->last_played < oldestVoiceTime){
      oldestVoiceTime = this->voices[i]->last_played;
      oldestVoice = i;
    }
    
    // Search for an inactive voice
    if(!this->voices[i]->isActive()){
      this->voices[i]->noteOn(channel, note, velocity);
      foundOne = true;
      break;
    }
  }

  // No inactive voice then will take over the oldest note
  if(!foundOne){
    this->voices[oldestVoice]->noteOn(channel, note, velocity);
  }
}

/**
 * Note off
 */
inline void Synth::noteOff(byte channel, byte note, byte velocity){
  for (int i = 0; i < voiceCount ; i++) {
    if(this->voices[i]->currentNote == note){
      this->voices[i]->noteOff(channel, note, velocity);
    }
  }
}

/**
 * Return the audio output
 */
inline AudioAmplifier * Synth::getOutputL(){
  return this->outputL;
}
inline AudioAmplifier * Synth::getOutputR(){
  return this->outputR;
}


/**
 * Control Change
 */
void Synth::controlChange(byte channel, byte control, byte value){
  for (int i = 0; i < voiceCount ; i++) {
    if(this->voices[i]->channel == channel && this->voices[i]->isActive()){
      this->voices[i]->controlChange(channel, control, value);
    }
  }
}


void Synth::pitchChange(byte channel, int pitch){
  for (int i = 0; i < voiceCount ; i++) {
    if(this->voices[i]->channel == channel && this->voices[i]->isActive()){
      this->voices[i]->pitchChange(channel, pitch);
    }
  }
}

void Synth::afterTouch(byte channel, byte pressure){
    //Serial.print("Received aftertouch");
  for (int i = 0; i < voiceCount ; i++) {
    if(this->voices[i]->channel == channel && this->voices[i]->isActive()){
      this->voices[i]->afterTouch(channel, pressure);
    }
  }
}

inline Voice** Synth::getVoices(){
  return this->voices;
}

#endif