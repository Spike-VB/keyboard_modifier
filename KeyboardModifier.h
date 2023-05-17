
#include <hidboot.h>
// To work with some keyboards have to modify Usb.cpp USB::InTransfer() method of USB_Host_Shield_Library_2.0.
// Have to remove 241-246 lines.
#include <Keyboard.h>
#include <string.h>

#include "Constants.h"


class StandardKeyMapper : public KeyboardReportParser {

public:
  uint8_t OemToKeyboardHCode(uint8_t key);
  uint8_t OemToAscii(uint8_t mod, uint8_t key);
};


class CustomKeyMapper {

  StandardKeyMapper standardKeyMapper;

public:
  uint8_t Modify(uint8_t customModKey, uint8_t key);
};


class PasswordProcessor {

  bool locked = true;
  String password;

  StandardKeyMapper standardKeyMapper;

public:
  bool unlockingModeActive = false;

  void UnlockPasswordProcessor(uint8_t mod, uint8_t key);
  bool ProcessPassword(uint8_t mod, uint8_t key);
};


class ModifierEngine : public KeyboardReportParser {

  StandardKeyMapper standardKeyMapper;
  CustomKeyMapper customKeyMapper;
  PasswordProcessor passwordProcessor;

public:
  void PrintKey(uint8_t mod, uint8_t key);
  void ProcessCachedKey();

protected:
  void OnKeyDown(uint8_t mod, uint8_t key);
  void OnKeyUp(uint8_t mod, uint8_t key);

  void OnControlKeysChanged(uint8_t before, uint8_t after);
  void SwitchControlKey(uint8_t key);

  void OnRedefinedModifierKeyDown(uint8_t mod, uint8_t key);
  void OnRedefinedModifierKeyUp(uint8_t mod, uint8_t key);

  void ReleaseKeyOnAllLayers(uint8_t key);
  void RefreshAllStates();
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

uint8_t StandardKeyMapper::OemToAscii(uint8_t mod, uint8_t key) {

  uint8_t shift = (mod & 0x22);

  // [a-z]
  if (VALUE_WITHIN(key, 0x04, 0x1d)) {
    // Upper case letters
    if ((kbdLockingKeys.kbdLeds.bmCapsLock == 0 && shift) || (kbdLockingKeys.kbdLeds.bmCapsLock == 1 && shift == 0)) {
      return (key - 4 + 'A');
      // Lower case letters
    } else {
      return (key - 4 + 'a');
    }  // Numbers
  } else if (VALUE_WITHIN(key, 0x1e, 0x27)) {
    if (shift)
      return ((uint8_t)pgm_read_byte(&getNumKeys()[key - 0x1e]));
    else
      return ((key == 0x27) ? '0' : key - 0x1e + '1');
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


bool PasswordProcessor::ProcessPassword(uint8_t mod, uint8_t key) {

  if (locked) {
    return;
  }

  // RIGHT_ALT mapped key (PASSWORDS)
  for (uint8_t i = 0; i < PASSWORDS_NUM; i++) {
    if (passwordsMap.map[i].key == key) {
      // RIGHT_ALT + SHIFT + mapped key
      if ((KEYBOARD_LEFT_SHIFT & mod) == KEYBOARD_LEFT_SHIFT || (KEYBOARD_RIGHT_SHIFT & mod) == KEYBOARD_RIGHT_SHIFT) {
        Keyboard.release(KEY_LEFT_SHIFT);
        Keyboard.release(KEY_RIGHT_SHIFT);
        Keyboard.print(passwordsMap.map[i].password);
      } else {
        // RIGHT_ALT mapped key
        Keyboard.print(passwordsMap.map[i].login);
      }
      return true;
    }
  }
  return false;
}


void PasswordProcessor::UnlockPasswordProcessor(uint8_t mod, uint8_t key) {

  if (KEYBOARD_RETURN == key) {
    if (password.compareTo(SYSTEM_PASSWORD) == 0) {
      locked = false;
      unlockingModeActive = false;
      password = "";
      return;
    } else {
      locked = true;
      unlockingModeActive = false;
      password = "";
      return;
    }
  }

  password = password + (char)standardKeyMapper.OemToAscii(mod, key);
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
      Keyboard.write(customKeyMapper.Modify(customModState.stateSet[cachedKey.key.customModifierIndex].key, cachedKey.key.key));
      cachedKey.key.released = false;
    } else {
      Keyboard.press(customKeyMapper.Modify(customModState.stateSet[cachedKey.key.customModifierIndex].key, cachedKey.key.key));
    }
    cachedKey.key.key = 0xFF;
  }
}


void ModifierEngine::OnKeyDown(uint8_t mod, uint8_t key) {

  if (DEBUG_ENABLED) {
    Serial.print("DN ");
    PrintKey(mod, key);
  }

  if (passwordProcessor.unlockingModeActive) {
    passwordProcessor.UnlockPasswordProcessor(mod, key);
    return;
  }

  // If mod is redefined standard modifier
  for (uint8_t i = 0; i < REDEFINED_MODIFIER_KEYS_NUM; i++) {
    if ((redefinedModState.map[i].key & mod) == redefinedModState.map[i].key) {
      redefinedModState.map[i].state = 1;
      OnRedefinedModifierKeyDown(mod, key);
      return;
    }
  }

  // If standard modifier key holded
  for (uint8_t i = 0; i < STANDARD_MODIFIER_KEYS_NUM; i++) {
    if (standardModState.map[i].state == 1) {
      for (uint8_t j = 0; j < CUSTOM_MODIFIER_KEYS_NUM; j++) {
        if (customModState.stateSet[j].state == 1) {
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
    if (customModState.stateSet[i].key == key) {
      customModState.stateSet[i].state = 1;
      return;
    }
  }

  // If key isn't custom modifier key, but custom modifier key is pressed
  for (uint8_t i = 0; i < CUSTOM_MODIFIER_KEYS_NUM; i++) {
    if (customModState.stateSet[i].state == 1) {
      customModState.stateSet[i].used = true;
      if (!customModState.stateSet[i].cached) {
        Keyboard.press(customKeyMapper.Modify(customModState.stateSet[i].key, key));
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

  if (passwordProcessor.unlockingModeActive) {
    return;
  }

  // If mod is redefined standard modifier
  for (uint8_t i = 0; i < REDEFINED_MODIFIER_KEYS_NUM; i++) {
    if ((redefinedModState.map[i].key & mod) == redefinedModState.map[i].key) {
      redefinedModState.map[i].state = 0;
      // OnRedefinedModifierKeyUp(redefinedModState.map[i].key, key);
      OnRedefinedModifierKeyUp(mod, key);
      return;
    }
  }

  // If key is custom modifier key
  for (uint8_t i = 0; i < CUSTOM_MODIFIER_KEYS_NUM; i++) {
    if (customModState.stateSet[i].key == key && customModState.stateSet[i].state == 1) {
      customModState.stateSet[i].state = 0;
      if (customModState.stateSet[i].used) {
        customModState.stateSet[i].used = false;
        if (customModState.stateSet[i].cached && cachedKey.key.key != 0xFF) {
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

  ReleaseKeyOnAllLayers(key);
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
    // SwitchControlKey(standardModState.RIGHT_ALT_STATE_INDEX);
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


void ModifierEngine::OnRedefinedModifierKeyDown(uint8_t mod, uint8_t key) {

  // RIGHT_ALT Down
  if ((KEYBOARD_RIGHT_ALT & mod) == KEYBOARD_RIGHT_ALT) {

    // RIGHT_ALT + BACKSPACE
    if (key == KEYBOARD_BACKSPACE) {
      RefreshAllStates();
      return;
    }

    // RIGHT_ALT + ESC
    if (key == KEYBOARD_ESC) {
      passwordProcessor.unlockingModeActive = true;
      return;
    }

    // Process passwords
    if (passwordProcessor.ProcessPassword(mod, key)) {
      return;
    }

    // RIGHT_ALT + mapped key (L_ALT_MODIFIED_KEYS_MAP)
    for (uint8_t i = 0; i < L_ALT_MODIFIED_KEYS_NUM; i++) {
      if (lAltModifiedKeysMap.map[i].originalKey == key) {
        Keyboard.press(standardKeyMapper.OemToKeyboardHCode(lAltModifiedKeysMap.map[i].modifiedKey));
        return;
      }
    }

    // RIGHT_ALT + not mapped key
    Keyboard.press(standardKeyMapper.OemToKeyboardHCode(key));
  }
}


void ModifierEngine::OnRedefinedModifierKeyUp(uint8_t mod, uint8_t key) {

  // RIGHT_ALT Up
  if ((KEYBOARD_RIGHT_ALT & mod) == KEYBOARD_RIGHT_ALT) {

    // RIGHT_ALT + mapped key
    for (uint8_t i = 0; i < L_ALT_MODIFIED_KEYS_NUM; i++) {
      if (lAltModifiedKeysMap.map[i].originalKey == key) {
        Keyboard.release(standardKeyMapper.OemToKeyboardHCode(lAltModifiedKeysMap.map[i].modifiedKey));
        return;
      }
    }

    // RIGHT_ALT + not mapped key
    Keyboard.release(standardKeyMapper.OemToKeyboardHCode(key));
  }
}


void ModifierEngine::ReleaseKeyOnAllLayers(uint8_t key) {

  for (uint8_t i = 0; i < CUSTOM_MODIFIER_KEYS_NUM; i++) {
    Keyboard.release(customKeyMapper.Modify(customModState.stateSet[i].key, key));
  }
  for (uint8_t i = 0; i < REDEFINED_MODIFIER_KEYS_NUM; i++) {
    for (uint8_t j = 0; j < L_ALT_MODIFIED_KEYS_NUM; j++) {
      if (lAltModifiedKeysMap.map[j].originalKey == key) {
        Keyboard.release(standardKeyMapper.OemToKeyboardHCode(lAltModifiedKeysMap.map[j].modifiedKey));
      }
    }
  }
  Keyboard.release(standardKeyMapper.OemToKeyboardHCode(key));
}


void ModifierEngine::RefreshAllStates() {

  Keyboard.releaseAll();
  for (uint8_t i; i < STANDARD_MODIFIER_KEYS_NUM; i++) {
    standardModState.map[i].state = 0;
  }
  for (uint8_t i; i < CUSTOM_MODIFIER_KEYS_NUM; i++) {
    customModState.stateSet[i].state = 0;
  }
  for (uint8_t i; i < REDEFINED_MODIFIER_KEYS_NUM; i++) {
    redefinedModState.map[i].state = 0;
  }
  cachedKey.key.key = 0xFF;
  cachedKey.key.released = false;
  passwordProcessor.unlockingModeActive = false;
}
