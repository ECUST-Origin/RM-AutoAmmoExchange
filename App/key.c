#include "key.h"

#define KEY_CONFIRM_TIME 10

#define __READKEY(KeyGpioPort, KeyGpioPin) (!HAL_GPIO_ReadPin((KeyGpioPort), (KeyGpioPin)))

void Key_Init(Key_Handle_t* hKey) {
  if(__READKEY(hKey->KeyGpioPort, hKey->KeyGpioPin)) {
    hKey->KeyState = KEY_UP;
  }
  else {
    hKey->KeyState = KEY_DOWN;
  }
}

void Key_Scan(Key_Handle_t* hKey) {
  switch(hKey->KeyState) {
    case KEY_UP:
      if(__READKEY(hKey->KeyGpioPort, hKey->KeyGpioPin)) {
        hKey->KeyState    = KEY_CONFIRM_DOWN;
        hKey->DebounceCnt = KEY_CONFIRM_TIME;
      }
      break;

    case KEY_CONFIRM_DOWN:
      if(__READKEY(hKey->KeyGpioPort, hKey->KeyGpioPin)) {
        hKey->DebounceCnt--;
        if(hKey->DebounceCnt == 0) {
          hKey->KeyState = KEY_DOWN;
          hKey->KeyDownCallback();
        }
      }
      else {
        hKey->KeyState = KEY_UP;
      }
      break;

    case KEY_DOWN:
      if(!__READKEY(hKey->KeyGpioPort, hKey->KeyGpioPin)) {
        hKey->KeyState    = KEY_CONFIRM_UP;
        hKey->DebounceCnt = KEY_CONFIRM_TIME;
      }
      break;

    case KEY_CONFIRM_UP:
      if(!__READKEY(hKey->KeyGpioPort, hKey->KeyGpioPin)) {
        hKey->DebounceCnt--;
        if(hKey->DebounceCnt == 0) {
          hKey->KeyState = KEY_UP;
        }
      }
      else {
        hKey->KeyState = KEY_DOWN;
      }

    default:
      break;
  }
}
