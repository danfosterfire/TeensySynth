#include <Audio.h>
#include "Synth.h"
#include <Arduino.h>
#include <Wire.h>
#include "effect_platereverb_i16.h"

//#include "USBHost_t36.h"

// Set up USB host midi interface
//USBHost myUSBHost;
//MIDIDevice slaveMidiDevice(myUSBHost);

Synth *synth = new Synth();

AudioOutputI2S           i2s1;           
AudioOutputUSB usb0;

// **** CHANGES 1/2 ************************************************************

// Option A (works if *synth->reverb is AudioAmplifier or AudioEffectPlateReverb,
// fails if *synth->reverb is AudioEffectPlateReverb_i16)
AudioConnection patchCordOutL(*synth->getOutputL(), 0, usb0, 0);
AudioConnection patchCordOutR(*synth->getOutputR(), 0, usb0, 1);

// Option B (works; tested with *synth->reverb is AudioAmplifier)
//AudioEffectPlateReverb_i16 reverb;
//AudioConnection patchCordReverbL(*synth->getOutputL(), 0, reverb, 0);
//AudioConnection patchCordReverbR(*synth->getOutputR(), 0, reverb, 1);
//AudioConnection patchCordOutL(reverb, 0, usb0, 0);
//AudioConnection patchCordOutR(reverb, 0, usb0, 1);

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

  AudioMemory(225);

  //sgtl5000_1.enable();
  //sgtl5000_1.volume(0.5);

  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleNoteOff(onNoteOff);
  usbMIDI.setHandleControlChange(onMidiControlChange);
  usbMIDI.setHandlePitchChange(onPitchChange);
  usbMIDI.setHandleAfterTouch(onAfterTouch);
  usbMIDI.setHandleAfterTouchPoly(onAfterTouchPoly);
  

  // **** Changes 2/2 **********************************************************

  // Option A (nothing)

  // Option B (Works) 
  /*
  reverb.size(1.0);     // max reverb length
  reverb.lowpass(0.3);  // sets the reverb master lowpass filter
  reverb.lodamp(0.1);   // amount of low end loss in the reverb tail
  reverb.hidamp(0.2);   // amount of treble loss in the reverb tail
  reverb.diffusion(1.0);  // 1.0 is the detault setting, lower it to create more "echoey" reverb
  reverb.wet_level(1.0f);
  reverb.bypass_set(false);
  */

  // **** END CHANGES 2/2 ******************************************************

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

      Serial.print("Total memory Usage: ");
      Serial.print(AudioMemoryUsageMax());
      Serial.println("");
      AudioProcessorUsageMaxReset();
      AudioMemoryUsageMaxReset();
      next_check = now + 5000;
    }



}

