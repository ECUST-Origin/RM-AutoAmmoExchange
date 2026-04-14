#include "comm.h"
#include "crc8_crc16.h"
#include "string.h"
#include "usart.h"

#define TX_BUFFER_SIZE 64
#define SOF            0xA5         // 数据帧起始字节

const static uint16_t CMDID = 0x0306;  // 命令ID 键鼠操作

static uint8_t TxBuffer[TX_BUFFER_SIZE] = {0};
static uint8_t FrameCnt = 0;

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
  // 数据帧头
  TxBuffer[0] = SOF;
  TxBuffer[1] = (uint8_t)DataLen;
  TxBuffer[2] = (uint8_t)(DataLen >> 8);
  TxBuffer[3] = FrameCnt;
  append_CRC8_check_sum(TxBuffer, 5);
  // 数据帧体
  memcpy(TxBuffer + 5, &CMDID, sizeof(CMDID));
  memcpy(TxBuffer + 7, pData, DataLen);
  append_CRC16_check_sum(TxBuffer, DataLen + 9);

  Comm_DRV_Transmit(TxBuffer, DataLen + 9);
  FrameCnt++;
}
