#include "hw_components_tests.h"

#include "components/n25q128a_qspi.h"

#include "assert.h"

#define W 1024
#define H 600
#define SIZE 1000

volatile uint16_t *framebuffer = (uint16_t*) 0xD0000000;
uint32_t sourcedata[SIZE];

void External_SDRAM_ReadWriteTest() {
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
      assert(sourcedata[i] == i);
  }
  int end_read = HAL_GetTick();
}

uint8_t r = 0x00, g = 0x00, b = 0x00;
uint16_t col = 0;
uint8_t color = 1;

void HAL_LTDC_LineEventCallback(LTDC_HandleTypeDef* hltdc) {
  static int count = 0;
  count++;
  if (count >= 30) {
    count = 0;
    switch (color) {
      case 1:
        r = 0xFF; g = 0x00; b = 0x00;
        color = 2;
        break;
      case 2:
        r = 0x00; g = 0xFF; b = 0x00;
        color = 3;
        break;
      case 3:
        r = 0x00; g = 0x00; b = 0xFF;
        color = 1;
        break;
      default:
        break;
    }
    col = ((r>>3)<<11) | ((g>>2)<<5) | (b>>3);  // Convert colors to RGB565
    // Put colors into the framebuffer
    for(int i = 0; i < W*H; i++) {
      framebuffer[i] = col;
    }
  }
}

#define COUNTOF(__BUFFER__)        (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
//#define BUFFERSIZE                 (COUNTOF(aTxBuffer) - 1)
#define BUFFERSIZE 200
uint8_t aTxBuffer[BUFFERSIZE];
uint8_t aRxBuffer[BUFFERSIZE];
#define QSPI_END_ADDR              (1 << QSPI_FLASH_SIZE)

void ExternalFlashReadWriteTest() {
  QSPI_CommandTypeDef sCommand;
  uint32_t address = 0;


  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.AddressSize       = QSPI_ADDRESS_24_BITS;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
  /* Initialize Reception buffer --------------------------------------- */
  for (uint16_t index = 0; index < BUFFERSIZE; index++) {
    aTxBuffer[index] = index;
    aRxBuffer[index] = 0;
  }
  /* Enable write operations ------------------------------------------- */
  QSPI_WriteEnable(&QSPIHandle);
  /* Erasing Sequence -------------------------------------------------- */
  sCommand.Instruction = SECTOR_ERASE_CMD;
  sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
  sCommand.Address     = address;
  sCommand.DataMode    = QSPI_DATA_NONE;
  sCommand.DummyCycles = 0;
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  /* Configure automatic polling mode to wait for end of erase ------- */
  QSPI_AutoPollingMemReady(&QSPIHandle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);


  /* Enable write operations ----------------------------------------- */
  QSPI_WriteEnable(&QSPIHandle);
  /* Writing Sequence ------------------------------------------------ */
  sCommand.Instruction = QUAD_IN_FAST_PROG_CMD;
  sCommand.AddressMode = QSPI_ADDRESS_1_LINE;
  sCommand.DataMode    = QSPI_DATA_4_LINES;
  sCommand.NbData      = BUFFERSIZE;
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_QSPI_Transmit(&QSPIHandle, aTxBuffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    Error_Handler();
  }
  QSPI_AutoPollingMemReady(&QSPIHandle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);


  /* Configure Volatile Configuration register (with new dummy cycles) */
  QSPI_DummyCyclesCfg(&QSPIHandle);
  /* Reading Sequence ------------------------------------------------ */
  sCommand.Instruction = QUAD_OUT_FAST_READ_CMD;
  sCommand.DummyCycles = N25Q128A_DUMMY_CYCLES_READ_QUAD;
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_QSPI_Receive(&QSPIHandle, aRxBuffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    Error_Handler();
  }


  /* Result comparison ----------------------------------------------- */
  for (uint16_t index = 0; index < BUFFERSIZE; index++) {
    if (aRxBuffer[index] != aTxBuffer[index]) {
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
    }
  }
  address += QSPI_PAGE_SIZE;
  if(address >= QSPI_END_ADDR) {
    address = 0;
  }
}
