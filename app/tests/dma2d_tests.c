#include "dma2d_tests.h"

#include "dma2d.h"
#include "ltdc.h"

#include "RGB565_240x130_1.h"
#include "RGB565_240x130_2.h"

#define W 1024
#define H 600

static uint32_t dma2d_duration_debug = 0;

HAL_StatusTypeDef Dma2dTest() {
  HAL_StatusTypeDef ret = HAL_OK;

  int begin = HAL_GetTick();
  HAL_StatusTypeDef ConfigLayerRet = HAL_DMA2D_ConfigLayer(&hdma2d, 0);
  HAL_StatusTypeDef DMA2D_StartRet = HAL_DMA2D_Start(&hdma2d, 0xFFFFFF, 0xD0000000, W, H);
  HAL_StatusTypeDef PollForTransferRet = HAL_DMA2D_PollForTransfer(&hdma2d, 10); // Ожидаем завершения операции DMA2D
  int end = HAL_GetTick();
  dma2d_duration_debug = end - begin;

  HAL_Delay(1000);

  ConfigLayerRet = HAL_DMA2D_ConfigLayer(&hdma2d, 0);
  DMA2D_StartRet = HAL_DMA2D_Start(&hdma2d, 0xFF0000, 0xD0000000, W, H);
  PollForTransferRet = HAL_DMA2D_PollForTransfer(&hdma2d, 10); // Ожидаем завершения операции DMA2D

  HAL_Delay(1000);

  ConfigLayerRet = HAL_DMA2D_ConfigLayer(&hdma2d, 0);
  DMA2D_StartRet = HAL_DMA2D_Start(&hdma2d, 0x00FF00, 0xD0000000, W, H);
  PollForTransferRet = HAL_DMA2D_PollForTransfer(&hdma2d, 10); // Ожидаем завершения операции DMA2D

  HAL_Delay(1000);

  ConfigLayerRet = HAL_DMA2D_ConfigLayer(&hdma2d, 0);
  DMA2D_StartRet = HAL_DMA2D_Start(&hdma2d, 0x0000FF, 0xD0000000, W, H);
  PollForTransferRet = HAL_DMA2D_PollForTransfer(&hdma2d, 10); // Ожидаем завершения операции DMA2D

  ret = ConfigLayerRet || DMA2D_StartRet || PollForTransferRet;

  return ret;
}
