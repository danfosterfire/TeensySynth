#ifndef Voice_h
#define Voice_h

#include <Audio.h>
#include <Arduino.h>

// one cent where an octave is 1.0
float one_cent{0.000833333};

/*
 * Voice
 */
class Voice{
  private:
    AudioSynthWaveformModulated *osc0;
    AudioSynthWaveformModulated *osc1;
    AudioSynthWaveformModulated *osc2;
    AudioSynthWaveformModulated *osc3;
    AudioFilterStateVariable        *filter;
    AudioMixer4 *filterIn;
    AudioAmplifier *ampTouch;
    AudioAmplifier *ampEnv;
    AudioEffectEnvelope *ampEnv0;
    AudioMixer4              *output;
    AudioConnection          *patchCord0;
    AudioConnection          *patchCord1;
    AudioConnection          *patchCord2;
    AudioConnection *patchCord3;
    AudioConnection *patchCord4;
    AudioConnection *patchCord5;
    AudioConnection *patchCord6;
    AudioConnection *patchCord7;
    AudioConnection *patchCord8;
    AudioConnection *patchCord9;
    AudioConnection *patchCord10;
    AudioConnection *patchCord11;

    bool notePlayed;

  public:
    Voice();

    byte currentNote;
    byte channel;  // for MPE
    unsigned long last_played;
    uint8_t filter_touch_octs{4};
    uint8_t timbre_scale{2};
    
    AudioMixer4 * getOutput();
    void noteOn(byte channel, byte note, byte velocity);
    void noteOff(byte channel, byte note, byte velocity);
    void controlChange(byte channel, byte control, byte value);
    void pitchChange(byte channel, int pitch);
    void afterTouch(byte channel, byte pressure);
    bool isActive();
};

/**
 * Constructor
 */

inline Voice::Voice(){

    // Generic instantiate four oscillators, filter, and output mixer
    this->osc0 = new AudioSynthWaveformModulated();
    this->osc0->begin(WAVEFORM_SINE);
    this->osc0->amplitude(0.0);

    this->osc1 = new AudioSynthWaveformModulated();
    this->osc1->begin(WAVEFORM_SINE);
    this->osc1->amplitude(0.0);

    this->osc2 = new AudioSynthWaveformModulated();
    //this->osc2->begin(WAVEFORM_BANDLIMIT_SAWTOOTH);
    this->osc2->begin(WAVEFORM_SINE);
    this->osc2->amplitude(0.5);

    this->osc3 = new AudioSynthWaveformModulated();
    this->osc3->begin(WAVEFORM_SINE);
    this->osc3->amplitude(0.5);

    this->filter = new AudioFilterStateVariable();
    this->filter->frequency(1600.0);
    //this->filter->octaveControl(3);
    //this->filter->resonance(0.33);
    //this->filter->inputDrive(1.0);
    //this->filter->passbandGain(0);

    this->filterIn = new AudioMixer4();
    this->filterIn->gain(0, 0.5);
    this->filterIn->gain(1, 0.5);
    this->filterIn->gain(2, 0.25);
    this->filterIn->gain(3, 0.25);

    this->output = new AudioMixer4();
    this->output->gain(0, 0.2);
    this->output->gain(1, 0.8);

    this->ampTouch = new AudioAmplifier();
    this->ampTouch->gain(0.0);

    this->ampEnv = new AudioAmplifier();
    this->ampEnv->gain(1.0);

    this->ampEnv0 = new AudioEffectEnvelope();
    this->ampEnv0->delay(0.0);
    this->ampEnv0->attack(3.0);
    this->ampEnv0->hold(0.0);
    this->ampEnv0->decay(0.0);
    this->ampEnv0->sustain(1.0);
    this->ampEnv0->release(5.0);
    this->ampEnv0->releaseNoteOn(0.0);

    // patch specific: generate patch cords, set up LFOs and carrier oscs
    this->osc0->frequency(0.1);
    this->osc0->amplitude(0.1);
    this->patchCord0 = new AudioConnection(*this->osc0, 0, *this->osc1, 0);
    this->osc1->frequency(0.15);
    this->osc1->amplitude(0.1);
    this->osc1->frequencyModulation(0.1);

    this->patchCord1 = new AudioConnection(*this->osc1, 0, *this->osc2, 0);
    this->osc2->frequencyModulation(one_cent);
    this->osc2->frequency(440.0);

    this->patchCord2 = new AudioConnection(*this->osc1, 0, *this->osc3, 0);
    this->osc3->frequencyModulation(one_cent);
    this->osc3->frequency(440.0);

    this->patchCord4 = new AudioConnection(*this->osc2, 0, *this->filterIn, 0);
    this->patchCord5 = new AudioConnection(*this->osc3, 0, *this->filterIn, 1);
    this->patchCord6 = new AudioConnection(*this->filterIn, 0, *this->filter, 0);
    this->patchCord7 = new AudioConnection(*this->filter, 0, *this->ampTouch, 0);
    this->patchCord8 = new AudioConnection(*this->filter, 0, *this->ampEnv, 0);
    this->patchCord9 = new AudioConnection(*this->ampEnv, 0, *this->ampEnv0, 0);
    this->patchCord10 = new AudioConnection(*this->ampEnv0, 0, *this->output, 0);
    this->patchCord11 = new AudioConnection(*this->ampTouch, 0, *this->output, 0);

    this->notePlayed = false;
}

/**
 * Return the audio output
 */
inline AudioMixer4 * Voice::getOutput(){
  return this->output;
}

/**
 * Note on
 */
inline void Voice::noteOn(byte channel, byte note, byte velocity) {
  
    // generic
    float freq = 440.0 * powf(2.0, (float)(note - 69) * 0.08333333);
    this->last_played = millis();
    this->notePlayed=true;
    this->currentNote = note;
    this->channel = channel;
    this->ampEnv0->noteOn();

    // patch specific
    this->osc2->frequency(freq);
    this->osc3->frequency(freq*0.5);
    //this->waveGenerator->amplitude(1.0);
    //this->waveGenerator->amplitude(float(map(velocity, 0, 127, 0, 100))/100);

}

/**
 * Note off
 */
inline void Voice::noteOff(byte channel, byte note, byte velocity) {
    
    // generic
    this->notePlayed=false;
    this->ampEnv0->noteOff();

    // patch specific
    this->ampTouch->gain(0.0);
    //this->osc2->amplitude(0);
    //this->osc3->amplitude(0);
}

/**
 * Control change
 */
inline void Voice::controlChange(byte channel, byte control, byte value) {

    // timbre
    if (control == 0x4A){
        //Serial.print("CC74: ");
        //Serial.print(value);
        this->osc0->frequency(0.1*powf(2.0, this->timbre_scale*float(value)*0.00787402));
    }

}

/**
 * Pitch change
 */
inline void Voice::pitchChange(byte channel, int pitch){
  float offset = float(pitch) / 171;  // originally 342 for 24 semitones; "342 corresponding to pitch range 24"
  float freq = 440.0 * powf(2.0, (float(this->currentNote) - 69 + offset) * 0.08333333);
  this->osc2->frequency(freq);
  this->osc3->frequency(freq*0.5);
}

/**
 * Aftertouch
 */
inline void Voice::afterTouch(byte channel, byte pressure){
  float pressure_val = float(pressure)*0.00787402;
  this->filter->frequency(1600.0 * powf(2.0, this->filter_touch_octs*pressure_val));
  this->ampTouch->gain(pressure_val);
}

/**
 * Is the voice active
 */
inline bool Voice::isActive(){
  return this->notePlayed;
}

#endif