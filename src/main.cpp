#include <Arduino.h>
#include <bluefruit.h>

#define PAUSE_TRACK_PIN 7

BLEDis bledis;
BLEHidAdafruit blehid;

unsigned long lastPressTime = 0;
int clickCount = 0;
const int multiClickTime = 400;

void setup() {
  Serial.begin(115200);
  
  Bluefruit.begin();
  Bluefruit.setName("BT DEVICE");
  Bluefruit.Periph.setConnInterval(9, 16);
  Bluefruit.setTxPower(0);

  bledis.setManufacturer("nor-gate");
  bledis.setModel("ng-v1");
  bledis.begin();

  blehid.begin();

  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addService(blehid);
  Bluefruit.Advertising.addName();

  Serial.println("Advertising");
  Bluefruit.Advertising.start(0);

  pinMode(PAUSE_TRACK_PIN, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(PAUSE_TRACK_PIN) == LOW) {
    unsigned long pressTime = millis();
    
    if (pressTime - lastPressTime > multiClickTime) {
      clickCount = 1;
    } else {
      clickCount++;
    }

    lastPressTime = pressTime;

    while (digitalRead(PAUSE_TRACK_PIN) == LOW);
    delay(50);
  }

  if (millis() - lastPressTime > multiClickTime && clickCount > 0) {
    if (clickCount == 1) {
      Serial.println("Pause");
      blehid.consumerKeyPress(HID_USAGE_CONSUMER_PLAY_PAUSE);
      delay(100);
      blehid.consumerKeyRelease();
    } else if (clickCount == 2) {
      Serial.println("Next Track");
      blehid.consumerKeyPress(HID_USAGE_CONSUMER_SCAN_NEXT);
      delay(100);
      blehid.consumerKeyRelease();
    } else if (clickCount == 3) {
      Serial.println("Previous Track");
      blehid.consumerKeyPress(HID_USAGE_CONSUMER_SCAN_PREVIOUS);
      delay(100);
      blehid.consumerKeyRelease();
    }

    clickCount = 0;
  }

  __WFI();
}
