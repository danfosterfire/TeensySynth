#include <Audio.h>
#include "Synth.h"
#include "USBHost_t36.h"

// Set up USB host midi interface
USBHost myUSBHost;
MIDIDevice slaveMidiDevice(myUSBHost);

Synth *synth = new Synth();

AudioOutputI2S           i2s1;           
AudioConnection          patchCord1(*synth->getOutput(), 0, i2s1, 0);
AudioConnection          patchCord2(*synth->getOutput(), 0, i2s1, 1);

AudioControlSGTL5000     sgtl5000_1;

void setup() {
  Serial.begin(9600);

  myUSBHost.begin();
	slaveMidiDevice.setHandleNoteOff(onNoteOff);
	slaveMidiDevice.setHandleNoteOn(onNoteOn);
	slaveMidiDevice.setHandleControlChange(onMidiControlChange);
  slaveMidiDevice.setHandlePitchChange(onPitchChange);
  slaveMidiDevice.setHandleAfterTouchChannel(onAfterTouch);

  pinMode(LED_BUILTIN, OUTPUT);

  AudioMemory(20);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleNoteOff(onNoteOff);
  usbMIDI.setHandleControlChange(onMidiControlChange);
  usbMIDI.setHandlePitchChange(onPitchChange);
  usbMIDI.setHandleAfterTouch(onAfterTouch);

  // Starting sequence
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("MPE Synth Ready!");
}

void loop() {
    myUSBHost.Task();
	  slaveMidiDevice.read();
    usbMIDI.read();
}

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
  synth->afterTouch(channel, pressure);
}