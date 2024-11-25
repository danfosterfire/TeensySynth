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


static uint32_t next;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  AudioMemory(15);

  next = millis() + 1000;

  AudioNoInterrupts();

  sine1.amplitude(0.5);
  sine1.frequency(440.0);

  amp1.gain(1.0);
  AudioInterrupts();

}

void loop() {
  // put your main code here, to run repeatedly
  delay(1000);
  sine1.amplitude(0.0);
  delay(1000);
  sine1.amplitude(0.5);
  sine1.frequency(220.0);

}

