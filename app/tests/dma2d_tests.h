#ifndef _DMA2D_TESTS_H_
#define _DMA2D_TESTS_H_

#include "main.h"

HAL_StatusTypeDef Dma2dTest();
void DMA2D_XferCpltCallback(DMA2D_HandleTypeDef* handle);

#endif /* _DMA2D_TESTS_H_ */
