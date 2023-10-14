#include "sdram_tests.h"

#include "assert.h"

#define SIZE 1000
uint32_t sourcedata[SIZE];

HAL_StatusTypeDef ExternalSdramReadWriteTest() {
  HAL_StatusTypeDef ret = HAL_OK;

  HAL_Delay(10);
  volatile uint32_t *externalRAM = (uint32_t*) 0xD0000000;
  for(int i = 0; i < SIZE; i++) {
      sourcedata[i] = i;
  }

  int begin_write = HAL_GetTick();
  //write external RAM
  for(int i = 0; i < SIZE; i++) {
      externalRAM[i] = sourcedata[i];
  }
  int end_write = HAL_GetTick();

  //read external RAM
  int begin_read = HAL_GetTick();
  //Read external RAM
  for(int i = 0; i < SIZE; i++) {
      sourcedata[i] = externalRAM[i];
//      assert(sourcedata[i] == i);
      if (sourcedata[i] != i) ret = HAL_ERROR;
  }
  int end_read = HAL_GetTick();

  return ret;
}
