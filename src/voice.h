#ifndef Voice_h
#define Voice_h

#include <Audio.h>

/*
 * Voice
 */
class Voice{
  private:
    AudioSynthWaveform       *waveGenerator;
    AudioFilterStateVariable        *filter;
    AudioMixer4              *output;
    AudioConnection          *patchCord1;
    AudioConnection          *patchCord2;
    AudioConnection          *patchCord3;

    bool notePlayed;

  public:
    Voice();

    byte currentNote;
    byte channel;  // for MPE
    unsigned long last_played;
    byte filter_touch_octs{2};
    
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
  this->waveGenerator = new AudioSynthWaveform();
  this->waveGenerator->begin(WAVEFORM_SAWTOOTH_REVERSE);
  this->waveGenerator->amplitude(0.0);

  this->filter = new AudioFilterStateVariable();
  this->filter->frequency(440.0);
  //this->filter->octaveControl(3);
  //this->filter->resonance(0.33);
  //this->filter->inputDrive(1.0);
  //this->filter->passbandGain(0);

  this->output = new AudioMixer4();
  this->output->gain(0, 0.6);

  this->patchCord1 = new AudioConnection(*this->waveGenerator, 0, *this->filter, 0);

  //this->patchCord1 = new AudioConnection(*this->waveGenerator, 0, *this->output, 1);
  this->patchCord2 = new AudioConnection(*this->filter, 0, *this->output, 0);
  this->patchCord3 = new AudioConnection(*this->filter, 0, *this->output, 1);

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
  float freq = 440.0 * powf(2.0, (float)(note - 69) * 0.08333333);
  this->waveGenerator->frequency(freq);
  //this->waveGenerator->amplitude(1.0);
  //this->waveGenerator->amplitude(float(map(velocity, 0, 127, 0, 100))/100);
  this->last_played = millis();
  this->notePlayed=true;
  this->currentNote = note;
  this->channel = channel;
}

/**
 * Note off
 */
inline void Voice::noteOff(byte channel, byte note, byte velocity) {
  this->waveGenerator->amplitude(0);
  this->notePlayed=false;
}

/**
 * Control change
 */
inline void Voice::controlChange(byte channel, byte control, byte value) {
}

/**
 * Pitch change
 */
inline void Voice::pitchChange(byte channel, int pitch){
  float offset = float(pitch) / 171;  // originally 342 for 24 semitones; "342 corresponding to pitch range 24"
  float freq = 440.0 * powf(2.0, (float(this->currentNote) - 69 + offset) * 0.08333333);
  this->waveGenerator->frequency(freq);
}

/**
 * Pitch change
 */
inline void Voice::afterTouch(byte channel, byte pressure){
  this->waveGenerator->amplitude(float(pressure)/127.0);
  this->filter->frequency(440.0 * powf(2.0, this->filter_touch_octs*(float(pressure)/127.0)));
}

/**
 * Is the voice active
 */
inline bool Voice::isActive(){
  return this->notePlayed;
}

#endif