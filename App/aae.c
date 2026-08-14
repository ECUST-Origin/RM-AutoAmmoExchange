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
/* AAE_CursorCoord_t CursorCoord_hero = {
  .AddAmmo  = {1180, 585},
  .Purchase = {955,  700},
  .Confirm  = {900,  580},
};

AAE_CursorCoord_t CursorCoord_infantry = {
  .AddAmmo  = {1180, 585},
  .Purchase = {955,  700},
  .Confirm  = {900,  580},
}; */

const uint16_t LUT_CursorCoord[11][2] = {
  {700,  585}, // 英雄-10 步兵-100
  {760,  585}, // 英雄-5 步兵-50
  {820,  585}, // 英雄-2 步兵-20
  {880,  585}, // 英雄-1 步兵-10
  {1040, 585}, // 英雄+1 步兵+10
  {1100, 585}, // 英雄+2 步兵+20
  {1160, 585}, // 英雄+5 步兵+50
  {1220, 585}, // 英雄+10 步兵+100
  {960,  685}, // 弹数确认
  {900,  580}, // 确认购买是
  {1020, 580}, // 确认购买否
};


AAE_ControllerData_t CtrlData = {0};

// 制作串口数据帧的data字段
static void LinkData() {
  CtrlData.ByteArr[0] = CtrlData.KeyCode1;
  CtrlData.ByteArr[1] = CtrlData.KeyCode2;
  CtrlData.ByteArr[2] = (uint8_t)CtrlData.MouseX;
  CtrlData.ByteArr[3] = ((uint8_t)(CtrlData.MouseX >> 8)) & 0x0F | ((CtrlData.MouseLeftDown == 1) << 4);
  CtrlData.ByteArr[4] = (uint8_t)CtrlData.MouseY;
  CtrlData.ByteArr[5] = ((uint8_t)(CtrlData.MouseY >> 8)) & 0x0F | ((CtrlData.MouseRightDown == 1) << 4);
  CtrlData.ByteArr[6] = 0x00;
  CtrlData.ByteArr[7] = 0x00;
}

/**
 * @brief 键盘按键操作
 * @param[in] KeyCode1 按键码1
 * @param[in] KeyCode2 按键码2
 * @note 裁判系统键盘机制（以I键为例）：
 * @note I键未按下时，发送带KEYCODE_I的报文，裁判系统判定I键被按下，在下一个报文到来前保持该状态。
 * @note 在此基础上的下一个报文仍然带KEYCODE_I，判定为I键保持按下。
 * @note 若下一报文没有KEYCODE_I，则判定I键被抬起。
 * @note KEYCODE_NONE作为占位符，不表示任何按键。
 */
static void Keyboard_Click(uint8_t KeyCode1, uint8_t KeyCode2) {
  CtrlData.KeyCode1 = KeyCode1;
  CtrlData.KeyCode2 = KeyCode2;
  LinkData();
  Comm_SendFrame(CtrlData.ByteArr, sizeof(CtrlData.ByteArr));
}

/**
 * @brief 移动鼠标指针
 * @param[in] X 目标位置的X坐标（绝对坐标）
 * @param[in] Y 目标位置的Y坐标（绝对坐标）
 * @note 屏幕左上角为原点(0,0)，向右、向下分别为X、Y轴的正方向。
 * @note 比赛选手端屏幕分辨率为1920*1080。
 */
static void Mouse_Move(uint16_t X, uint16_t Y) {
  CtrlData.KeyCode1 = 0;
  CtrlData.KeyCode2 = 0;
  CtrlData.MouseX   = X;
  CtrlData.MouseY   = Y;
  LinkData();
  Comm_SendFrame(CtrlData.ByteArr, sizeof(CtrlData.ByteArr));
  HAL_Delay(INTERVAL_TIME);
}

/**
 * @brief 鼠标在(X,Y)位置单击左键
 * @param[in] X 目标位置的X坐标（绝对坐标）
 * @param[in] Y 目标位置的Y坐标（绝对坐标）
 * @note 屏幕左上角为原点(0,0)，向右、向下分别为X、Y轴的正方向。
 * @note 比赛选手端屏幕分辨率为1920×1080。
 */
static void Mouse_LeftClick(uint16_t X, uint16_t Y) {
  Mouse_Move(X, Y);
  CtrlData.MouseLeftDown = 1;
  LinkData();
  Comm_SendFrame(CtrlData.ByteArr, sizeof(CtrlData.ByteArr));
  // HAL_Delay(INTERVAL_TIME);
  CtrlData.MouseLeftDown = 0;
  LinkData();
  Comm_SendFrame(CtrlData.ByteArr, sizeof(CtrlData.ByteArr));
}

/**
 * @brief 英雄基地买弹，一次买8发
 * @note 该按键宏执行过程中不能移动鼠标，否则会购买失败
 */
void AAE_BuyAmmoBase_Hero() {
  Keyboard_Click(KEYCODE_I, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(LUT_CursorCoord[7][0], LUT_CursorCoord[7][1]);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(LUT_CursorCoord[2][0], LUT_CursorCoord[2][1]);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(LUT_CursorCoord[8][0], LUT_CursorCoord[8][1]);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(LUT_CursorCoord[9][0], LUT_CursorCoord[9][1]);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_I, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE, KEYCODE_NONE);
}

/**
 * @brief 步兵基地买弹，一次买80发
 * @note 该按键宏执行过程中不能移动鼠标，否则会购买失败
 */
void AAE_BuyAmmoBase_Infantry() {
  Keyboard_Click(KEYCODE_O, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(LUT_CursorCoord[7][0], LUT_CursorCoord[7][1]);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(LUT_CursorCoord[2][0], LUT_CursorCoord[2][1]);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(LUT_CursorCoord[8][0], LUT_CursorCoord[8][1]);
  HAL_Delay(INTERVAL_TIME);
  Mouse_LeftClick(LUT_CursorCoord[9][0], LUT_CursorCoord[9][1]);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_O, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE, KEYCODE_NONE);
}

/**
 * @brief 英雄远程买弹
 */
void AAE_BuyAmmoRemote_Hero() {
  Keyboard_Click(KEYCODE_H, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_CTRL, KEYCODE_3);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_Y, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE, KEYCODE_NONE);
}

/**
 * @brief 步兵远程买弹
 */
void AAE_BuyAmmoRemote_Infantry() {
  Keyboard_Click(KEYCODE_H, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_CTRL, KEYCODE_2);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_Y, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE, KEYCODE_NONE);
}

/**
 * @brief 通用远程买血
 */
void AAE_BuyHP() {
  Keyboard_Click(KEYCODE_H, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_CTRL, KEYCODE_1);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_Y, KEYCODE_NONE);
  HAL_Delay(INTERVAL_TIME);
  Keyboard_Click(KEYCODE_NONE, KEYCODE_NONE);
}
