#pragma once

#include <unordered_set>

enum class MouseButton
{
  BUTTON_NONE,
  BUTTON_LEFT,
  BUTTON_RIGHT
};

enum class KeyboardKey
{
  KEY_ESCAPE,
  KEY_ENTER,
  KEY_RIGHT_SHIFT,
  KEY_LEFT_SHIFT
};

struct SGameInput
{
  void reset();

  void activateKeyboardKeys(std::unordered_set<KeyboardKey> keys);
  void releaseKeyboardKeys(std::unordered_set<KeyboardKey> keys);

  bool bShiftDown();

  void pressKeyboardKey(KeyboardKey key);
  void releaseKeyboardKey(KeyboardKey key);

  void clickMouseButton(MouseButton button);

  std::unordered_set<MouseButton> mouseClickedButtons = {};
  std::unordered_set<KeyboardKey> keyboardPressedKeys = {};
  std::unordered_set<KeyboardKey> keyboardReleasedKeys = {};
  std::unordered_set<KeyboardKey> keyboardActiveKeys = {};

  bool bRightShiftDown = false;
  bool bLeftShiftDown = false;

  int mouseWheelScroll = 0;

  int mouseX = -1;
  int mouseY = -1;
};
