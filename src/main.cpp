#include <Arduino.h>
#include <PS2KeyAdvanced.h>
#include <PS2KeyMap.h>
#include <BleKeyboard.h>

// GPIO pins
#define PS2_DATA 32
#define PS2_CLK 13
#define BATTERY_IO 35

// extra keybind mappings
#define PS2_CAPSLOCK 0x98
#define PS2_SCROLLLOCK 0x96

// battery percentage scheduling scheduling
#define BATTERY_MONITOR_INTERVEL 10 // Seconds
unsigned long previousMillis = 0;

PS2KeyAdvanced keyboard;
PS2KeyMap keymap;
BleKeyboard bleKeyboard("Model M", "IBM", 100);


// Returns voltage of battery in volts
float readVoltage() {
  int raw = analogRead(BATTERY_IO);
  float voltage = (raw / 4095.0) * 2 * 3.3 * 1.1; // gotta multiply by 2 because of the
                                                  // built in voltage divider on A13
  return voltage;
}

// Returns percentage of battery
float readBatteryPercent() {
  float voltage = readVoltage();
  if (4.2 <= voltage) return 100;
  if (3.2 >= voltage) return 0;
  return (voltage - 3.2)*100;
}

void setup() {
  Serial.begin(115200);
  bleKeyboard.begin();
  delay(500); // half second delay between BLE initialization and keyboard initiallzation
  keyboard.begin(PS2_DATA, PS2_CLK);
  keymap.selectMap((char*)"US");
}

void loop() {
  //Serial.printf("Battery Percent %f\n", readBatteryPercent(readVoltage()));
  unsigned long currentMillis = millis();
  if (BATTERY_MONITOR_INTERVEL*1000 <= currentMillis-previousMillis) {
    previousMillis = currentMillis;
    bleKeyboard.setBatteryLevel(readBatteryPercent());
  }
  if (keyboard.available()) {
    uint16_t raw = keyboard.read();
    uint16_t key = keymap.remapKey(raw);
    Serial.printf("raw: 0x%04X key: 0x%04X code: 0x%02X\n", raw, key, key & 0xFF);
    uint8_t rawCode = raw & 0xFF;
    if (!(raw & PS2_BREAK)) {
      switch (rawCode) {
        case PS2_KEY_UP_ARROW: bleKeyboard.write(KEY_UP_ARROW); break;
        case PS2_KEY_DN_ARROW: bleKeyboard.write(KEY_DOWN_ARROW); break;
        case PS2_KEY_L_ARROW:  bleKeyboard.write(KEY_LEFT_ARROW); break;
        case PS2_KEY_R_ARROW:  bleKeyboard.write(KEY_RIGHT_ARROW); break;
        case PS2_KEY_L_CTRL:   bleKeyboard.press(KEY_LEFT_CTRL); break;
        case PS2_KEY_R_CTRL:   bleKeyboard.press(KEY_RIGHT_CTRL); break;
        case PS2_KEY_L_ALT:    bleKeyboard.press(KEY_LEFT_GUI); break;
        case PS2_KEY_R_ALT:    bleKeyboard.press(KEY_RIGHT_GUI); break;
        case PS2_CAPSLOCK:     bleKeyboard.press(KEY_CAPS_LOCK); break;
        case PS2_SCROLLLOCK:   bleKeyboard.press(KEY_LEFT_ALT); break; // I am maping scroll lock to option
      }
    } else {
      // Release modifiers on key up
      switch (rawCode) {
        case PS2_KEY_L_CTRL:   bleKeyboard.release(KEY_LEFT_CTRL); break;
        case PS2_KEY_R_CTRL:   bleKeyboard.release(KEY_RIGHT_CTRL); break;
        case PS2_KEY_L_ALT:    bleKeyboard.release(KEY_LEFT_GUI); break;
        case PS2_KEY_R_ALT:    bleKeyboard.release(KEY_RIGHT_GUI); break;
        case PS2_CAPSLOCK:     bleKeyboard.release(KEY_CAPS_LOCK); break;
        case PS2_SCROLLLOCK:   bleKeyboard.release(KEY_LEFT_ALT); break; // I am maping scroll lock to option
      }
    }
    if (key > 0 && !(key & PS2_BREAK)) {
      uint8_t code = key & 0xFF;
      Serial.println(code);
      // Special keys
      if (code == 8)   { bleKeyboard.write(KEY_BACKSPACE); return; }
      if (code == 13)  { bleKeyboard.write(KEY_RETURN); return; }
      if (code == 9)   { bleKeyboard.write(KEY_TAB); return; }
      if (code == 27)  { bleKeyboard.write(KEY_ESC); return; }
      if (code == 127) { bleKeyboard.write(KEY_DELETE); return; }
      
      // Regular characters
      if (code >= 32 && code < 128) {
        if (bleKeyboard.isConnected()) {
          bleKeyboard.write(code);
        }
      }
    }
  }
}