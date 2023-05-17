
#include "Passwords.h"

//=====================================================//
//                                                     //
//               Keyboard codes                        //
//                                                     //
//=====================================================//

// Standard modifiers

#define KEYBOARD_LEFT_CTRL 0b00000001
#define KEYBOARD_LEFT_SHIFT 0b00000010
#define KEYBOARD_LEFT_ALT 0b00000100
#define KEYBOARD_LEFT_GUI 0b00001000
#define KEYBOARD_RIGHT_CTRL 0b00010000
#define KEYBOARD_RIGHT_SHIFT 0b00100000
#define KEYBOARD_RIGHT_ALT 0b01000000

// Letters
#define KEYBOARD_U 0x18
#define KEYBOARD_I 0x0C
#define KEYBOARD_O 0x12
#define KEYBOARD_P 0x13
#define KEYBOARD_J 0x0D
#define KEYBOARD_K 0x0E
#define KEYBOARD_L 0x0F
#define KEYBOARD_M 0x10

// Numbers
#define KEYBOARD_1 0x1E
#define KEYBOARD_2 0x1F
#define KEYBOARD_3 0x20
#define KEYBOARD_4 0x21
#define KEYBOARD_5 0x22
#define KEYBOARD_6 0x23
#define KEYBOARD_7 0x24
#define KEYBOARD_8 0x25
#define KEYBOARD_9 0x26
#define KEYBOARD_0 0x27

// Misc keys (Keyboard code // Keyboard.h code)
#define KEYBOARD_SPACE 0x2C         // 0x20
#define KEYBOARD_UP_ARROW 0x52      // 0xDA
#define KEYBOARD_DOWN_ARROW 0x51    // 0xD9
#define KEYBOARD_LEFT_ARROW 0x50    // 0xD8
#define KEYBOARD_RIGHT_ARROW 0x4F   // 0xD7
#define KEYBOARD_BACKSPACE 0x2A     // 0xB2
#define KEYBOARD_TAB 0x2B           // 0xB3
#define KEYBOARD_RETURN 0x28        // 0xB0
#define KEYBOARD_MENU 0x65          // 0xED
#define KEYBOARD_ESC 0x29           // 0xB1
#define KEYBOARD_INSERT 0x49        // 0xD1
#define KEYBOARD_DELETE 0x4C        // 0xD4
#define KEYBOARD_PAGE_UP 0x4B       // 0xD3
#define KEYBOARD_PAGE_DOWN 0x4E     // 0xD6
#define KEYBOARD_HOME 0x4A          // 0xD2
#define KEYBOARD_END 0x4D           // 0xD5
#define KEYBOARD_CAPS_LOCK 0x39     // 0xC1
#define KEYBOARD_PRINT_SCREEN 0x46  // 0xCE
#define KEYBOARD_SCROLL_LOCK 0x47   // 0xCF
#define KEYBOARD_PAUSE 0x48         // 0xD0

// Numeric keypad
#define KEYBOARD_NUM_LOCK 0x53     // 0xDB
#define KEYBOARD_KP_SLASH 0x54     // 0xDC
#define KEYBOARD_KP_ASTERISK 0x55  // 0xDD
#define KEYBOARD_KP_MINUS 0x56     // 0xDE
#define KEYBOARD_KP_PLUS 0x57      // 0xDF
#define KEYBOARD_KP_ENTER 0x58     // 0xE0
#define KEYBOARD_KP_1 0x59         // 0xE1
#define KEYBOARD_KP_2 0x5A         // 0xE2
#define KEYBOARD_KP_3 0x5B         // 0xE3
#define KEYBOARD_KP_4 0x5C         // 0xE4
#define KEYBOARD_KP_5 0x5D         // 0xE5
#define KEYBOARD_KP_6 0x5E         // 0xE6
#define KEYBOARD_KP_7 0x5F         // 0xE7
#define KEYBOARD_KP_8 0x60         // 0xE8
#define KEYBOARD_KP_9 0x61         // 0xE9
#define KEYBOARD_KP_0 0x62         // 0xEA
#define KEYBOARD_KP_DOT 0x63       // 0xEB

// Function keys
#define KEYBOARD_F1 0x3A   // 0xC2
#define KEYBOARD_F2 0x3B   // 0xC3
#define KEYBOARD_F3 0x3C   // 0xC4
#define KEYBOARD_F4 0x3D   // 0xC5
#define KEYBOARD_F5 0x3E   // 0xC6
#define KEYBOARD_F6 0x3F   // 0xC7
#define KEYBOARD_F7 0x40   // 0xC8
#define KEYBOARD_F8 0x41   // 0xC9
#define KEYBOARD_F9 0x42   // 0xCA
#define KEYBOARD_F10 0x43  // 0xCB
#define KEYBOARD_F11 0x44  // 0xCC
#define KEYBOARD_F12 0x45  // 0xCD

// Symbols
#define KEYBOARD_APOSTROPHE 0x35    // 0x60
#define KEYBOARD_MINUS 0x2D         // 0x2D
#define KEYBOARD_EQUALS 0x2E        // 0x3D
#define KEYBOARD_L_SQ_BRACKET 0x2F  // 0x5B
#define KEYBOARD_R_SQ_BRACKET 0x30  // 0x5D
#define KEYBOARD_BACK_SLASH 0x31    // 0x5C
#define KEYBOARD_SEMICOLON 0x33     // 0x3B
#define KEYBOARD_SINGLE_QUOTE 0x34  // 0x27
#define KEYBOARD_COMMA 0x36         // 0x2C
#define KEYBOARD_DOT 0x37           // 0x2E
#define KEYBOARD_SLASH 0x38         // 0x2F

//=====================================================//

// Enable debug (serial print)
bool DEBUG_ENABLED = false;

// Custom modifier keys
#define CUSTOM_MOD_KEY_1 KEYBOARD_SPACE

// Refresh key
#define REFRESH_KEY KEYBOARD_BACKSPACE

// Unlocking mode key
#define UNLOCKING_MODE_KEY KEYBOARD_ESC

// Number of keys modified by 1st modifier
#define MODIFIED_KEYS_NUM_1 23

// Number of keys modified by left Alt
#define L_ALT_MODIFIED_KEYS_NUM 0

// Number of key pairs if struct KEYSMAP
#define KEY_CODES_NUM 60

// Number of custom modifier keys
#define CUSTOM_MODIFIER_KEYS_NUM 1

// Number of standard modifier keys
#define STANDARD_MODIFIER_KEYS_NUM 8

// Number of redefined standard modifier keys
#define REDEFINED_MODIFIER_KEYS_NUM 1

// Number of passwords
#define PASSWORDS_NUM 1

// Interruption timeout
#define INTERRUPTION_TIMEOUT 100


// Map of keys modified by custom modifier 1
struct MODIFIED_KEYS_MAP_1 {

  struct ORIGINAL_MODIFIED_KEY_PAIR {
    uint8_t originalKey;
    uint8_t modifiedKey;
  };

  ORIGINAL_MODIFIED_KEY_PAIR map[MODIFIED_KEYS_NUM_1] = {
    OriginalModifiedKeyPair(KEYBOARD_U, KEYBOARD_HOME),
    OriginalModifiedKeyPair(KEYBOARD_I, KEYBOARD_UP_ARROW),
    OriginalModifiedKeyPair(KEYBOARD_O, KEYBOARD_END),
    OriginalModifiedKeyPair(KEYBOARD_P, KEYBOARD_PAGE_UP),
    OriginalModifiedKeyPair(KEYBOARD_J, KEYBOARD_LEFT_ARROW),
    OriginalModifiedKeyPair(KEYBOARD_K, KEYBOARD_DOWN_ARROW),
    OriginalModifiedKeyPair(KEYBOARD_L, KEYBOARD_RIGHT_ARROW),
    OriginalModifiedKeyPair(KEYBOARD_SEMICOLON, KEYBOARD_PAGE_DOWN),
    OriginalModifiedKeyPair(KEYBOARD_M, KEYBOARD_DELETE),
    OriginalModifiedKeyPair(KEYBOARD_BACKSPACE, KEYBOARD_SPACE),
    OriginalModifiedKeyPair(KEYBOARD_ESC, KEYBOARD_APOSTROPHE),
    OriginalModifiedKeyPair(KEYBOARD_1, KEYBOARD_F1),
    OriginalModifiedKeyPair(KEYBOARD_2, KEYBOARD_F2),
    OriginalModifiedKeyPair(KEYBOARD_3, KEYBOARD_F3),
    OriginalModifiedKeyPair(KEYBOARD_4, KEYBOARD_F4),
    OriginalModifiedKeyPair(KEYBOARD_5, KEYBOARD_F5),
    OriginalModifiedKeyPair(KEYBOARD_6, KEYBOARD_F6),
    OriginalModifiedKeyPair(KEYBOARD_7, KEYBOARD_F7),
    OriginalModifiedKeyPair(KEYBOARD_8, KEYBOARD_F8),
    OriginalModifiedKeyPair(KEYBOARD_9, KEYBOARD_F9),
    OriginalModifiedKeyPair(KEYBOARD_0, KEYBOARD_F10),
    OriginalModifiedKeyPair(KEYBOARD_MINUS, KEYBOARD_F11),
    OriginalModifiedKeyPair(KEYBOARD_EQUALS, KEYBOARD_F12)
  };

private:
  ORIGINAL_MODIFIED_KEY_PAIR OriginalModifiedKeyPair(uint8_t originalKey, uint8_t modifiedKey) {
    ORIGINAL_MODIFIED_KEY_PAIR pair{};
    pair.originalKey = originalKey;
    pair.modifiedKey = modifiedKey;
    return pair;
  }
};


// Map of keys modified by left Alt
struct L_ALT_MODIFIED_KEYS_MAP {

  struct ORIGINAL_MODIFIED_KEY_PAIR {
    uint8_t originalKey;
    uint8_t modifiedKey;
  };

  ORIGINAL_MODIFIED_KEY_PAIR map[L_ALT_MODIFIED_KEYS_NUM] = {};

private:
  ORIGINAL_MODIFIED_KEY_PAIR OriginalModifiedKeyPair(uint8_t originalKey, uint8_t modifiedKey) {
    ORIGINAL_MODIFIED_KEY_PAIR pair{};
    pair.originalKey = originalKey;
    pair.modifiedKey = modifiedKey;
    return pair;
  }
};


// Map of pairs of keyboard key code and key code for <Keyboard.h> library
struct KEYSMAP {

  struct KEYS_PAIR {
    uint8_t kbdCode;
    uint8_t kbdHCode;
  };

  // this map has to be sorted by kbdCode to provide binary search
  KEYS_PAIR map[KEY_CODES_NUM] = {
    KeysPair(0x28, 0xB0),
    KeysPair(0x29, 0xB1),
    KeysPair(0x2A, 0xB2),
    KeysPair(0x2B, 0xB3),
    KeysPair(0x2D, 0x2D),
    KeysPair(0x2E, 0x3D),
    KeysPair(0x2F, 0x5B),
    KeysPair(0x30, 0x5D),
    KeysPair(0x31, 0x5C),
    KeysPair(0x33, 0x3B),
    KeysPair(0x34, 0x27),
    KeysPair(0x35, 0x60),
    KeysPair(0x36, 0x2C),
    KeysPair(0x37, 0x2E),
    KeysPair(0x38, 0x2F),
    KeysPair(0x39, 0xC1),
    KeysPair(0x3A, 0xC2),
    KeysPair(0x3B, 0xC3),
    KeysPair(0x3C, 0xC4),
    KeysPair(0x3D, 0xC5),
    KeysPair(0x3E, 0xC6),
    KeysPair(0x3F, 0xC7),
    KeysPair(0x40, 0xC8),
    KeysPair(0x41, 0xC9),
    KeysPair(0x42, 0xCA),
    KeysPair(0x43, 0xCB),
    KeysPair(0x44, 0xCC),
    KeysPair(0x45, 0xCD),
    KeysPair(0x46, 0xCE),
    KeysPair(0x47, 0xCF),
    KeysPair(0x48, 0xD0),
    KeysPair(0x49, 0xD1),
    KeysPair(0x4A, 0xD2),
    KeysPair(0x4B, 0xD3),
    KeysPair(0x4C, 0xD4),
    KeysPair(0x4D, 0xD5),
    KeysPair(0x4E, 0xD6),
    KeysPair(0x4F, 0xD7),
    KeysPair(0x50, 0xD8),
    KeysPair(0x51, 0xD9),
    KeysPair(0x52, 0xDA),
    KeysPair(0x53, 0xDB),
    KeysPair(0x54, 0xDC),
    KeysPair(0x55, 0xDD),
    KeysPair(0x56, 0xDE),
    KeysPair(0x57, 0xDF),
    KeysPair(0x58, 0xE0),
    KeysPair(0x59, 0xE1),
    KeysPair(0x5A, 0xE2),
    KeysPair(0x5B, 0xE3),
    KeysPair(0x5C, 0xE4),
    KeysPair(0x5D, 0xE5),
    KeysPair(0x5E, 0xE6),
    KeysPair(0x5F, 0xE7),
    KeysPair(0x60, 0xE8),
    KeysPair(0x61, 0xE9),
    KeysPair(0x62, 0xEA),
    KeysPair(0x63, 0xEB),
    KeysPair(0x65, 0xED)
  };

private:
  KEYS_PAIR KeysPair(uint8_t kbdCode, uint8_t kbdHCode) {
    KEYS_PAIR pair{};
    pair.kbdCode = kbdCode;
    pair.kbdHCode = kbdHCode;
    return pair;
  }
};


// State of standard modifier keys
struct STANDARD_MODIFIER_KEYS_STATE {

  struct KEY_STATE {
    uint8_t key;
    uint8_t state;
  };

  // Modifier keys indices
  uint8_t LEFT_CTRL_STATE_INDEX = 0x00;
  uint8_t LEFT_SHIFT_STATE_INDEX = 0x01;
  uint8_t LEFT_ALT_STATE_INDEX = 0x02;
  uint8_t LEFT_GUI_STATE_INDEX = 0x03;
  uint8_t RIGHT_CTRL_STATE_INDEX = 0x04;
  uint8_t RIGHT_SHIFT_STATE_INDEX = 0x05;
  uint8_t RIGHT_ALT_STATE_INDEX = 0x06;
  uint8_t RIGHT_GUI_STATE_INDEX = 0x07;

  KEY_STATE map[STANDARD_MODIFIER_KEYS_NUM] = {
    KeyState(KEY_LEFT_CTRL),
    KeyState(KEY_LEFT_SHIFT),
    KeyState(KEY_LEFT_ALT),
    KeyState(KEY_LEFT_GUI),
    KeyState(KEY_RIGHT_CTRL),
    KeyState(KEY_RIGHT_SHIFT),
    KeyState(KEY_RIGHT_ALT),
    KeyState(KEY_RIGHT_GUI)
  };

private:
  KEY_STATE KeyState(uint8_t key) {
    KEY_STATE pair{};
    pair.key = key;
    pair.state = 0;
    return pair;
  }
};


// State of custom modifier keys
struct CUSTOM_MODIFIER_KEYS_STATE {

  struct KEY_STATE_SET {
    uint8_t key;
    bool cached;
    uint8_t state;
    bool used;
  };

  KEY_STATE_SET stateSet[CUSTOM_MODIFIER_KEYS_NUM] = {
    KeyStateSet(CUSTOM_MOD_KEY_1, true)
  };

private:
  KEY_STATE_SET KeyStateSet(uint8_t key, bool cached) {
    KEY_STATE_SET set{};
    set.key = key;
    set.cached = cached;
    set.state = 0;
    set.used = false;
    return set;
  }
};


// State of redefined modifier keys
struct REDEFINED_MODIFIER_KEYS_STATE {

  struct KEY_STATE_SET {
    uint8_t key;
    uint8_t state;
  };

  KEY_STATE_SET map[REDEFINED_MODIFIER_KEYS_NUM] = {
    KeyStateSet(KEYBOARD_RIGHT_ALT)
  };

private:
  KEY_STATE_SET KeyStateSet(uint8_t key) {
    KEY_STATE_SET set{};
    set.key = key;
    set.state = 0;
    return set;
  }
};


// Cached key
struct CACHED_KEY {

  struct KEY {
    uint8_t key;  // 0xFF - key absent
    uint8_t customModifierIndex;
    bool released;
    uint32_t time;
  };

  KEY key = SetKey();

private:
  KEY SetKey() {
    KEY key{};
    key.key = 0xFF;
    key.customModifierIndex = 0;
    key.released = false;
    key.time = 0;
  }
};


struct PASSWORDS_MAP {

  struct LOGIN_PASSWORD_PAIR {
    uint8_t key;
    String login;
    String password;
  };

  LOGIN_PASSWORD_PAIR map[PASSWORDS_NUM] = {
    LoginPasswordPair(KEYBOARD_1, JOB_MAIN_LOGIN, JOB_MAIN_PASSWORD)
  };

private:
  LOGIN_PASSWORD_PAIR LoginPasswordPair(uint8_t key, String login, String password) {
    LOGIN_PASSWORD_PAIR pair{};
    pair.key = key;
    pair.login = login;
    pair.password = password;
    return pair;
  }
};


struct MODIFIED_KEYS_MAP_1 modifiedKeysMap_1;            // Map of keys modified by custom modifier 1
struct L_ALT_MODIFIED_KEYS_MAP lAltModifiedKeysMap;      // Map of keys modified by left Alt
struct KEYSMAP keysMap;                                  // Map of pairs of keyboard key code and key code for <Keyboard.h> library
struct STANDARD_MODIFIER_KEYS_STATE standardModState;    // State of standard modifier keys
struct CUSTOM_MODIFIER_KEYS_STATE customModState;        // State of custom modifier keys
struct REDEFINED_MODIFIER_KEYS_STATE redefinedModState;  // State of redefined modifier keys
struct CACHED_KEY cachedKey;                             // Cached key
struct PASSWORDS_MAP passwordsMap;                       // Map of key-login-password
