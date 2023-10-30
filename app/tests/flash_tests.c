#include "flash_tests.h"
#include "components_drivers/flash/n25q128a_qspi.h"
#include "assert.h"

#define COUNTOF(__BUFFER__)        (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
//#define BUFFERSIZE                 (COUNTOF(aTxBuffer) - 1)
#define BUFFERSIZE 256
uint8_t aTxBuffer[BUFFERSIZE];
uint8_t aRxBuffer[BUFFERSIZE];
#define QSPI_END_ADDR              (1 << QSPI_FLASH_SIZE)

HAL_StatusTypeDef ExternalFlashReadWriteTest() {
  HAL_StatusTypeDef ret = HAL_OK;

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
    ret = HAL_ERROR;
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
    ret = HAL_ERROR;
  }
  if (HAL_QSPI_Transmit(&QSPIHandle, aTxBuffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    ret = HAL_ERROR;
  }
  QSPI_AutoPollingMemReady(&QSPIHandle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);


  /* Configure Volatile Configuration register (with new dummy cycles) */
  QSPI_DummyCyclesCfg(&QSPIHandle);
  /* Reading Sequence ------------------------------------------------ */
  sCommand.Instruction = QUAD_OUT_FAST_READ_CMD;
  sCommand.DummyCycles = N25Q128A_DUMMY_CYCLES_READ_QUAD;
  if (HAL_QSPI_Command(&QSPIHandle, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    ret = HAL_ERROR;
  }
  if (HAL_QSPI_Receive(&QSPIHandle, aRxBuffer, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
    ret = HAL_ERROR;
  }


  /* Result comparison ----------------------------------------------- */
  for (uint16_t index = 0; index < BUFFERSIZE; index++) {
    if (aRxBuffer[index] != aTxBuffer[index]) {
//      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
      ret = HAL_ERROR;
    }
  }
  address += QSPI_PAGE_SIZE;
  if(address >= QSPI_END_ADDR) {
    address = 0;
  }

  return ret;
}
