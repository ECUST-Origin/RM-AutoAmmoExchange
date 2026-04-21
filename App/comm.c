#include "comm.h"
#include "crc8_crc16.h"
#include "string.h"
#include "usart.h"

#define TX_BUFFER_SIZE  64
#define SOF             0xA5  // 数据帧起始字节
#define MIN_TX_INTERVAL 40    // 两次串口发送的最短间隔(ms)

#define USE_DTB  // 使用双缓冲区DMA进行串口发送



const static uint16_t CMDID = 0x0306;  // 命令ID 键鼠操作


#ifndef USE_DTB
/* ========== 使用单缓冲区阻塞式发送 ========== */
static uint8_t TxBuffer[TX_BUFFER_SIZE] = {0};

/* ---------- 硬件驱动层函数 ---------- */
__STATIC_FORCEINLINE void Comm_DRV_Transmit(uint8_t* pData, uint16_t Len) {
  HAL_UART_Transmit(&huart3, pData, Len, HAL_MAX_DELAY);
}

/* ---------- 应用层函数 ---------- */
/**
 *  @brief 以裁判系统串口通信帧格式发送数据帧
 *  @param pData 数据指针
 */
void Comm_SendFrame(uint8_t* pData, uint16_t DataLen) {
  static uint8_t frameCnt = 0;
  // 数据帧头
  TxBuffer[0] = SOF;
  TxBuffer[1] = (uint8_t)DataLen;
  TxBuffer[2] = (uint8_t)(DataLen >> 8);
  TxBuffer[3] = frameCnt;
  append_CRC8_check_sum(TxBuffer, 5);
  // 数据帧体
  memcpy(TxBuffer + 5, &CMDID, sizeof(CMDID));
  memcpy(TxBuffer + 7, pData, DataLen);
  append_CRC16_check_sum(TxBuffer, DataLen + 9);

  Comm_DRV_Transmit(TxBuffer, DataLen + 9);
  frameCnt++;
}


#else
/* ========== 使用双缓冲区DMA发送 ========== */
static uint8_t TxBuffer1[TX_BUFFER_SIZE] = {0};
static uint8_t TxBuffer2[TX_BUFFER_SIZE] = {0};

static void Comm_DRV_Transmit(uint8_t* pData, uint16_t Len) {
  while(HAL_UART_GetState(&huart3) != HAL_UART_STATE_READY);  // 等待串口空闲
  HAL_UART_Transmit_DMA(&huart3, pData, Len);
}

void Comm_SendFrame(uint8_t* pData, uint16_t DataLen) {
  static uint8_t* currentBuffer   = TxBuffer1;  // 当前写入的缓冲区
  static uint8_t  frameCnt        = 0;          // 帧序列号，每发一帧序列号+1
  static uint32_t prevTxTimestamp = 0;          // 上次执行串口发送的时间戳
  // 数据帧头
  currentBuffer[0] = SOF;
  currentBuffer[1] = (uint8_t)DataLen;
  currentBuffer[2] = (uint8_t)(DataLen >> 8);
  currentBuffer[3] = frameCnt;
  append_CRC8_check_sum(currentBuffer, 5);
  // 数据帧体
  memcpy(currentBuffer + 5, &CMDID, sizeof(CMDID));
  memcpy(currentBuffer + 7, pData, DataLen);
  append_CRC16_check_sum(currentBuffer, DataLen + 9);

  while(HAL_GetTick() - prevTxTimestamp < MIN_TX_INTERVAL);  // 防止发送过快导致丢包
  Comm_DRV_Transmit(currentBuffer, DataLen + 9);
  // 切换缓冲区
  if(currentBuffer == TxBuffer1) {
    currentBuffer = TxBuffer2;
  }
  else {
    currentBuffer = TxBuffer1;
  }

  frameCnt++;
  prevTxTimestamp = HAL_GetTick();
}

#endif
