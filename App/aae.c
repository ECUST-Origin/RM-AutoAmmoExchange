#include "aae.h"
#include "comm.h"

#define KEYCODE_NONE 0
#define KEYCODE_CTRL 17
#define KEYCODE_1    49
#define KEYCODE_2    50
#define KEYCODE_3    51
#define KEYCODE_H    72
#define KEYCODE_I    73
#define KEYCODE_N    78
#define KEYCODE_O    79
#define KEYCODE_Y    89

#define PRESS_TIME    20  // 键鼠按下状态持续时间
#define INTERVAL_TIME 30  // 键鼠操作间隔时间

/* 鼠标指针点击坐标 */
AAE_CursorCoord_t CursorCoord_hero = {
  .AddAmmo  = {1180, 585},
  .Purchase = {955, 700},  
  .Confirm  = {900, 580},
};

AAE_CursorCoord_t CursorCoord_infantry = {
  .AddAmmo  = {1180, 585},
  .Purchase = {955, 700},  
  .Confirm  = {900, 580},
};


AAE_ControllerData_t CtrlData = {0};

// 制作串口数据帧的data字段
static void PackData() {
  CtrlData.ByteArr[0] = CtrlData.KeyCode1;
  CtrlData.ByteArr[1] = CtrlData.KeyCode2;
  CtrlData.ByteArr[2] = (uint8_t)CtrlData.MouseX;
  CtrlData.ByteArr[3] = ((uint8_t)(CtrlData.MouseX >> 8)) & 0x0F | ((CtrlData.MouseLeftDown == 1) << 4);
  CtrlData.ByteArr[4] = (uint8_t)CtrlData.MouseY;
  CtrlData.ByteArr[5] = ((uint8_t)(CtrlData.MouseY >> 8)) & 0x0F | ((CtrlData.MouseRightDown == 1) << 4);
  CtrlData.ByteArr[6] = 0x00;
  CtrlData.ByteArr[7] = 0x00;
}

/* 裁判系统键盘机制（以I键为例）：
 * I键未按下时，发送带KEYCODE_I的报文，裁判系统判定I键被按下，在下一个报文到来前保持该状态
 * 在此基础上的下一个报文仍然带KEYCODE_I，判定为I键保持按下
 * 若下一报文没有KEYCODE_I，则判定I键被抬起
 * KEYCODE_NONE作为占位符，不表示任何按键
*/
// 单击键盘按键
static void Keyboard_Click(uint8_t KeyCode1, uint8_t KeyCode2) {
  CtrlData.KeyCode1 = KeyCode1;
  CtrlData.KeyCode2 = KeyCode2;
  PackData();
  Comm_SendFrame(CtrlData.ByteArr, sizeof(CtrlData.ByteArr));
}

// 移动鼠标指针至坐标(X,Y)
static void Mouse_Move(uint16_t X, uint16_t Y) {
  CtrlData.KeyCode1 = 0;
  CtrlData.KeyCode2 = 0;
  CtrlData.MouseX   = X;
  CtrlData.MouseY   = Y;
  PackData();
  Comm_SendFrame(CtrlData.ByteArr, sizeof(CtrlData.ByteArr));
  HAL_Delay(INTERVAL_TIME);
}

// 鼠标在(X,Y)位置单击
static void Mouse_LeftClick(uint16_t X, uint16_t Y) {
  Mouse_Move(X, Y);
  CtrlData.MouseLeftDown = 1;
  PackData();
  Comm_SendFrame(CtrlData.ByteArr, sizeof(CtrlData.ByteArr));
  HAL_Delay(INTERVAL_TIME);
  CtrlData.MouseLeftDown = 0;
  PackData();
  Comm_SendFrame(CtrlData.ByteArr, sizeof(CtrlData.ByteArr));
}

// 英雄基地买弹
void AAE_BuyAmmoBase_Hero() {
  Keyboard_Click(KEYCODE_I, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(CursorCoord_hero.AddAmmo[0], CursorCoord_hero.AddAmmo[1]);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(CursorCoord_hero.Purchase[0], CursorCoord_hero.Purchase[1]);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(CursorCoord_hero.Confirm[0], CursorCoord_hero.Confirm[1]);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_I, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE, KEYCODE_NONE);
}

// 步兵基地买弹
void AAE_BuyAmmoBase_Infantry() {
  Keyboard_Click(KEYCODE_O, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(CursorCoord_infantry.AddAmmo[0], CursorCoord_infantry.AddAmmo[1]);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(CursorCoord_infantry.Purchase[0], CursorCoord_infantry.Purchase[1]);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(CursorCoord_infantry.Confirm[0], CursorCoord_infantry.Confirm[1]);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_O, KEYCODE_NONE);
}

// 英雄远程买弹
void AAE_BuyAmmoRemote_Hero(){
  Keyboard_Click(KEYCODE_H, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_CTRL, KEYCODE_3);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_Y, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE,KEYCODE_NONE);
}

// 步兵远程买弹
void AAE_BuyAmmoRemote_Infantry(){
  Keyboard_Click(KEYCODE_H, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_CTRL, KEYCODE_2);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_Y, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE,KEYCODE_NONE);
}

// 通用远程买血
void AAE_BuyHP(){
  Keyboard_Click(KEYCODE_H, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_CTRL, KEYCODE_1);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_Y, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE,KEYCODE_NONE);
}
