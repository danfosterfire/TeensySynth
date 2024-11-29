#include <Audio.h>
#include "Synth.h"
#include <Arduino.h>
#include <Wire.h>
#include "effect_platereverb_F32.h"

//#include "USBHost_t36.h"

// Set up USB host midi interface
//USBHost myUSBHost;
//MIDIDevice slaveMidiDevice(myUSBHost);

Synth *synth = new Synth();

// number of samples in each delay line
#define CHORUS_DELAY_LENGTH (16*AUDIO_BLOCK_SAMPLES)
// allocate delay line for mono channel
short delayline[CHORUS_DELAY_LENGTH];

int n_chorus_voice{3};

//AudioOutputI2S           i2s1;           
AudioOutputUSB usb0;
//AudioEffectChorus chorus0;
AudioEffectFreeverbStereo reverb0;
AudioAmplifier reverbSend;
AudioConnection patchCord0(*synth->getOutput(), reverbSend);
AudioConnection patchCord1(reverbSend, 0, reverb0, 0);
AudioMixer4 reverbMixR;
AudioConnection patchCord2(*synth->getOutput(), 0, reverbMixR, 0);
AudioConnection patchCord3(reverb0, 0, reverbMixR, 1);
AudioMixer4 reverbMixL;
AudioConnection patchCord4(*synth->getOutput(), 0, reverbMixL, 0);
AudioConnection patchCord5(reverb0, 1, reverbMixL, 1);

//AudioConnection patchCord1(chorus0, reverb0);
//AudioConnection patchCord2(*synth->getOutput(), 0, usb0, 0);
//AudioConnection patchCord1(*synth->getOutput(), 0, usb0, 1);
AudioConnection patchCord6(reverbMixR, 0,  usb0, 0);
AudioConnection patchCord7(reverbMixL, 0,  usb0, 1);

//AudioControlSGTL5000     sgtl5000_1;

void onNoteOn(byte channel, byte note, byte velocity) {
  synth->noteOn(channel, note, velocity);
  digitalWrite(LED_BUILTIN, HIGH);
}

void onNoteOff(byte channel, byte note, byte velocity) {
  synth->noteOff(channel, note, velocity);
  digitalWrite(LED_BUILTIN, LOW);
}

void onMidiControlChange(byte channel, byte control, byte value){
  synth->controlChange(channel, control, value);
}

void onPitchChange(byte channel, int pitch){
  synth->pitchChange(channel, pitch);
}

void onAfterTouch(byte channel, byte pressure){
  //Serial.println("Received Aftertouch Channel");
  synth->afterTouch(channel, pressure);
}

void onAfterTouchPoly(byte channel, byte note, byte pressure){
  //Serial.println("Received aftertouch Poly");
  synth->afterTouch(channel, pressure);
}

int next_check{millis()+5000};

void setup() {
  Serial.begin(115200);

  //myUSBHost.begin();
	//slaveMidiDevice.setHandleNoteOff(onNoteOff);
	//slaveMidiDevice.setHandleNoteOn(onNoteOn);
	//slaveMidiDevice.setHandleControlChange(onMidiControlChange);
  //slaveMidiDevice.setHandlePitchChange(onPitchChange);
  //slaveMidiDevice.setHandleAfterTouchChannel(onAfterTouch);

  //pinMode(LED_BUILTIN, OUTPUT);

  AudioMemory(120);

  //sgtl5000_1.enable();
  //sgtl5000_1.volume(0.5);

  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleNoteOff(onNoteOff);
  usbMIDI.setHandleControlChange(onMidiControlChange);
  usbMIDI.setHandlePitchChange(onPitchChange);
  usbMIDI.setHandleAfterTouch(onAfterTouch);
  usbMIDI.setHandleAfterTouchPoly(onAfterTouchPoly);
  // hmmm... bug here? https://github.com/PaulStoffregen/cores/blob/058d2808187a24e7db53803b7510e26827064c03/teensy4/usb_midi.h#L321

  //chorus0.begin(delayline, CHORUS_DELAY_LENGTH, n_chorus_voice);
  reverbMixR.gain(0, 0.7);
  reverbMixR.gain(1, 0.3);
  reverbMixL.gain(0, 0.7);
  reverbMixL.gain(1, 0.3);

  reverbSend.gain(0.3);

  reverb0.roomsize(0.5);
  reverb0.damping(0.5);

  // Starting sequence
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("MPE Synth Ready!");

}

void loop() {
    //myUSBHost.Task();
	  //slaveMidiDevice.read();
    usbMIDI.read();

    int now{millis()};

    if (now > next_check){
      Serial.print("%, Total CPU Usage: ");
      Serial.print(AudioProcessorUsageMax());
      Serial.println("%");
      AudioProcessorUsageMaxReset();
      next_check = now + 5000;
    }



}

