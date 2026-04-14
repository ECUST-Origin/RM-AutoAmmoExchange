#ifndef __AAE_H__
#define __AAE_H__ 

#include "main.h"

typedef struct{
  uint16_t AddAmmo[2];  // 增加弹丸数 英雄+5 步兵+50
  uint16_t Purchase[2]; // 购买弹丸
  uint16_t Confirm[2];  // 确认购买
}AAE_CursorCoord_t;

typedef struct{
  uint8_t KeyCode1;
  uint8_t KeyCode2;
  uint16_t MouseX;
  uint16_t MouseY;
  uint8_t MouseLeftDown;
  uint8_t MouseRightDown;
  uint8_t DataArr[8];
}AAE_ControllerData_t;

void AAE_BuyAmmoBase_Hero();
void AAE_BuyAmmoBase_Infantry();
void AAE_BuyAmmoRemote_Hero();
void AAE_BuyAmmoRemote_Infantry();
void AAE_BuyHP();

#endif /* __AAE_H__ */