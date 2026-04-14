#ifndef __KEY_H__
#define __KEY_H__ 

#include "main.h"

typedef enum{
  KEY_UP = 0,
  KEY_CONFIRM_DOWN,
  KEY_DOWN,
  KEY_CONFIRM_UP,
}Key_State_t;

typedef struct{
  GPIO_TypeDef* KeyGpioPort;
  uint16_t KeyGpioPin;
  void (*KeyDownCallback)(void);
  Key_State_t KeyState;
  uint8_t DebounceCnt;
}Key_Handle_t;

void Key_Init(Key_Handle_t* hKey);
void Key_Scan(Key_Handle_t* hKey);

#endif /* __KEY_H__ */