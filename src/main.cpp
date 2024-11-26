#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=55,73
AudioAmplifier           amp1;           //xy=129,155
AudioOutputUSB           usb1;           //xy=328,195
AudioConnection          patchCord1(sine1, amp1);
AudioConnection          patchCord2(amp1, 0, usb1, 0);
// GUItool: end automatically generated code
// put function declarations here:

int led_pin{13};

float midiToFreq(byte note){
  return 440.0 * powf(2.0, (float)(note-69)*0.08333333);
}

void myNoteOn(byte channel, byte note, byte velocity){
  // AudioNoInterrupts();
  sine1.frequency(midiToFreq(note));
  sine1.amplitude(1.0);
  // AudioInterrupts();
  digitalWrite(led_pin, HIGH);
}

void myNoteOff(byte channel, byte note, byte velocity){
  sine1.amplitude(0.0);

  digitalWrite(led_pin, LOW);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(led_pin, OUTPUT);

  AudioMemory(15);

  //AudioNoInterrupts();
  amp1.gain(1.0);
  //AudioInterrupts();

  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);

}

void loop() {
  // put your main code here, to run repeatedly
  usbMIDI.read();

}

