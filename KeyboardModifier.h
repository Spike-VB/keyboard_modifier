
#include <hidboot.h>
// To work with some keyboards have to modify Usb.cpp USB::InTransfer() method of USB_Host_Shield_Library_2.0.
// Have to remove 241-246 lines.
#include <Keyboard.h>

#include "Constants.h"


class StandardKeyMapper {

public:
  uint8_t OemToKeyboardHCode(uint8_t key);
};


class CustomKeyMapper {

  StandardKeyMapper standardKeyMapper;

public:
  uint8_t Modify(uint8_t customModKey, uint8_t key);
};


class ModifierEngine : public KeyboardReportParser {

  StandardKeyMapper standardKeyMapper;
  CustomKeyMapper customKeyMapper;

public:
  void PrintKey(uint8_t mod, uint8_t key);
  void ProcessCachedKey();

protected:
  void OnKeyDown(uint8_t mod, uint8_t key);
  void OnKeyUp(uint8_t mod, uint8_t key);

  void OnControlKeysChanged(uint8_t before, uint8_t after);
  void SwitchControlKey(uint8_t key);
};


uint8_t StandardKeyMapper::OemToKeyboardHCode(uint8_t key) {

  if (VALUE_WITHIN(key, 0x04, 0x1d)) {  // [a-z]
    return (key - 4 + 'a');
  } else if (VALUE_WITHIN(key, 0x1e, 0x27)) {         // Numbers
    return ((key == 0x27) ? '0' : key - 0x1e + '1');  // 0x27 -> 0-key
  } else if (key == KEYBOARD_SPACE) {                 // Space
    return (0x20);
  }

  uint8_t i = 0;
  uint8_t j = KEY_CODES_NUM - 1;
  uint8_t k;

  while (j - i > 1) {
    k = i + (j - i) / 2;
    if (keysMap.map[k].kbdCode == key) {
      return keysMap.map[k].kbdHCode;
    } else if (keysMap.map[k].kbdCode > key) {
      j = k;
    } else {
      i = k;
    }
    if (j - i == 1) {
      if (keysMap.map[i].kbdCode == key) {
        return keysMap.map[i].kbdHCode;
      } else if (keysMap.map[j].kbdCode == key) {
        return keysMap.map[j].kbdHCode;
      }
    }
  }

  return (0);
}


uint8_t CustomKeyMapper::Modify(uint8_t customModKey, uint8_t key) {

  switch (customModKey) {
    case CUSTOM_MOD_KEY_1:
      for (uint8_t i; i < MODIFIED_KEYS_NUM_1; i++) {
        if (modifiedKeysMap_1.map[i].originalKey == key) {
          return standardKeyMapper.OemToKeyboardHCode(modifiedKeysMap_1.map[i].modifiedKey);
        }
      }
      return standardKeyMapper.OemToKeyboardHCode(key);
  }
}


void ModifierEngine::PrintKey(uint8_t m, uint8_t key) {

  MODIFIERKEYS mod;
  *((uint8_t*)&mod) = m;
  Serial.print((mod.bmLeftCtrl == 1) ? "C" : "_");
  Serial.print((mod.bmLeftShift == 1) ? "S" : "_");
  Serial.print((mod.bmLeftAlt == 1) ? "A" : "_");
  Serial.print((mod.bmLeftGUI == 1) ? "G" : "_");

  Serial.print(" >");
  PrintHex<uint8_t>(key, 0x80);
  Serial.print("< ");

  Serial.print((mod.bmRightCtrl == 1) ? "C" : "_");
  Serial.print((mod.bmRightShift == 1) ? "S" : "_");
  Serial.print((mod.bmRightAlt == 1) ? "A" : "_");
  Serial.println((mod.bmRightGUI == 1) ? "G" : "_");
}


void ModifierEngine::ProcessCachedKey() {
  if (cachedKey.key.key != 0xFF && millis() - cachedKey.key.time >= INTERRUPTION_TIMEOUT) {
    if (cachedKey.key.released) {
      Keyboard.write(customKeyMapper.Modify(customModState.map[cachedKey.key.customModifierIndex].key, cachedKey.key.key));
      cachedKey.key.released = false;
    } else {
      Keyboard.press(customKeyMapper.Modify(customModState.map[cachedKey.key.customModifierIndex].key, cachedKey.key.key));
    }
    cachedKey.key.key = 0xFF;
  }
}


void ModifierEngine::OnKeyDown(uint8_t mod, uint8_t key) {

  if (DEBUG_ENABLED) {
    Serial.print("DN ");
    PrintKey(mod, key);
  }

  // If standard modifier key holded
  for (uint8_t i = 0; i < STANDARD_MODIFIER_KEYS_NUM; i++) {
    if (standardModState.map[i].state == 1) {
      for (uint8_t j = 0; j < CUSTOM_MODIFIER_KEYS_NUM; j++) {
        if (customModState.map[j].state == 1) {
          break;
        }
        Keyboard.press(standardKeyMapper.OemToKeyboardHCode(key));
        return;
      }
      break;
    }
  }

  // If key is custom modifier key
  for (uint8_t i = 0; i < CUSTOM_MODIFIER_KEYS_NUM; i++) {
    if (customModState.map[i].key == key) {
      customModState.map[i].state = 1;
      return;
    }
  }

  // If key isn't custom modifier key, but custom modifier key is pressed
  for (uint8_t i = 0; i < CUSTOM_MODIFIER_KEYS_NUM; i++) {
    if (customModState.map[i].state == 1) {
      customModState.map[i].used = true;
      if (!customModState.map[i].cached) {
        Keyboard.press(customKeyMapper.Modify(customModState.map[i].key, key));
      } else {
        cachedKey.key.key = key;
        cachedKey.key.customModifierIndex = i;
        cachedKey.key.time = millis();
      }
      return;
    }
  }

  Keyboard.press(standardKeyMapper.OemToKeyboardHCode(key));
}


void ModifierEngine::OnKeyUp(uint8_t mod, uint8_t key) {

  if (DEBUG_ENABLED) {
    Serial.print("UP ");
    PrintKey(mod, key);
  }

  // If key is custom modifier key
  for (uint8_t i = 0; i < CUSTOM_MODIFIER_KEYS_NUM; i++) {
    if (customModState.map[i].key == key && customModState.map[i].state == 1) {
      customModState.map[i].state = 0;
      if (customModState.map[i].used) {
        customModState.map[i].used = false;
        if (customModState.map[i].cached && cachedKey.key.key != 0xFF) {
          Keyboard.write(standardKeyMapper.OemToKeyboardHCode(key));
          if (cachedKey.key.released) {
            Keyboard.write(standardKeyMapper.OemToKeyboardHCode(cachedKey.key.key));
            cachedKey.key.released = false;
          } else {
            Keyboard.press(standardKeyMapper.OemToKeyboardHCode(cachedKey.key.key));
          }
          cachedKey.key.key = 0xFF;
        }
      } else {
        Keyboard.write(standardKeyMapper.OemToKeyboardHCode(key));
      }
      return;
    }
  }

  if (cachedKey.key.key == key) {
    cachedKey.key.released = true;
    return;
  }

  // Release key despite modified or not
  for (uint8_t i = 0; i < CUSTOM_MODIFIER_KEYS_NUM; i++) {
    Keyboard.release(customKeyMapper.Modify(customModState.map[i].key, key));
  }
  Keyboard.release(standardKeyMapper.OemToKeyboardHCode(key));
}


void ModifierEngine::OnControlKeysChanged(uint8_t before, uint8_t after) {

  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;

  if (beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl) {
    SwitchControlKey(standardModState.LEFT_CTRL_STATE_INDEX);
  }
  if (beforeMod.bmLeftShift != afterMod.bmLeftShift) {
    SwitchControlKey(standardModState.LEFT_SHIFT_STATE_INDEX);
  }
  if (beforeMod.bmLeftAlt != afterMod.bmLeftAlt) {
    SwitchControlKey(standardModState.LEFT_ALT_STATE_INDEX);
  }
  if (beforeMod.bmLeftGUI != afterMod.bmLeftGUI) {
    SwitchControlKey(standardModState.LEFT_GUI_STATE_INDEX);
  }
  if (beforeMod.bmRightCtrl != afterMod.bmRightCtrl) {
    SwitchControlKey(standardModState.RIGHT_CTRL_STATE_INDEX);
  }
  if (beforeMod.bmRightShift != afterMod.bmRightShift) {
    SwitchControlKey(standardModState.RIGHT_SHIFT_STATE_INDEX);
  }
  if (beforeMod.bmRightAlt != afterMod.bmRightAlt) {
    SwitchControlKey(standardModState.RIGHT_ALT_STATE_INDEX);
  }
  if (beforeMod.bmRightGUI != afterMod.bmRightGUI) {
    SwitchControlKey(standardModState.RIGHT_GUI_STATE_INDEX);
  }
}


void ModifierEngine::SwitchControlKey(uint8_t modKeyIndex) {

  if (standardModState.map[modKeyIndex].state == 0) {
    Keyboard.press(standardModState.map[modKeyIndex].key);
    standardModState.map[modKeyIndex].state = 1;
  } else {
    Keyboard.release(standardModState.map[modKeyIndex].key);
    standardModState.map[modKeyIndex].state = 0;
  }
}
