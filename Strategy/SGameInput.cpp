#include "SGameInput.hpp"

void SGameInput::reset()
{
  mouseWheelScroll = 0;
  mouseX = -1;
  mouseY = -1;
  mouseClickedButtons.clear();
  keyboardPressedKeys.clear();
  keyboardReleasedKeys.clear();
}

void SGameInput::activateKeyboardKeys(std::unordered_set<KeyboardKey> keys)
{
  keyboardPressedKeys.insert(keys.begin(), keys.end());
  keyboardActiveKeys.insert(keys.begin(), keys.end());
}

void SGameInput::pressKeyboardKey(KeyboardKey key)
{
  keyboardPressedKeys.insert(key);
  keyboardActiveKeys.insert(key);
}

void SGameInput::releaseKeyboardKeys(std::unordered_set<KeyboardKey> keys)
{
  keyboardReleasedKeys.insert(keys.begin(), keys.end());
  keyboardActiveKeys.erase(keys.begin(), keys.end());
}

void SGameInput::releaseKeyboardKey(KeyboardKey key)
{
  keyboardReleasedKeys.insert(key);
  keyboardActiveKeys.erase(key);
}

bool SGameInput::bShiftDown()
{
  return bRightShiftDown or bLeftShiftDown;
}

void SGameInput::clickMouseButton(MouseButton button)
{
  mouseClickedButtons.insert(button);
}
