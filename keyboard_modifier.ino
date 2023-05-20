
#include <SPI.h>
#include <MsTimer2.h>

#include "KeyboardModifier.h"


USB usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> hidKeyboard(&usb);
ModifierEngine engine;

void setup() {

  if (DEBUG_ENABLED) {
    Serial.begin(115200);

#if !defined(__MIPSEL__)
    while (!Serial)
      ;
#endif

    Serial.println("Start");

    if (usb.Init() == -1)
      Serial.println("OSC did not start.");

    delay(200);
  } else {
    usb.Init();
  }

  hidKeyboard.SetReportParser(0, &engine);

  MsTimer2::set(5, timerInterupt);
  MsTimer2::start();

  Keyboard.releaseAll();

  pinMode(LOCKED_LED, OUTPUT);
  pinMode(OPEND_LED, OUTPUT);
  digitalWrite(LOCKED_LED, LOW);
  digitalWrite(OPEND_LED, LOW);
}

void loop() {

  usb.Task();
}

void timerInterupt() {
  engine.ProcessCachedKey();
  engine.ControlLeds();
}
