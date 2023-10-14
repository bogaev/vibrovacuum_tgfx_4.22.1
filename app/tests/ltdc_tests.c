#include "sdram_tests.h"

#include "config_tests.h"
#include "assert.h"

#define W 2048
#define H 1024
static volatile uint32_t *framebuffer = (uint32_t*) 0xD0000000;
static uint8_t r = 0x00, g = 0x00, b = 0x00;
static uint32_t col = 0;
static uint8_t color = 1;
static uint32_t ltdc_duration_debug = 0;

void LtdcSdramItTest() {
  LTDC->IER |= LTDC_IER_LIE; // Enable LTDC interrupts
  HAL_Delay(4000);
  LTDC->IER &= ~LTDC_IER_LIE;
}

#ifdef LTDC_TEST_ON
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
//    col = ((r>>3)<<11) | ((g>>2)<<5) | (b>>3);  // Convert colors to RGB565
    col = 0xFF << 24 | r << 16 | g << 8 | b;  // Convert colors to ARGB888
    int begin = HAL_GetTick();
    for(int i = 0; i < W*H; i++) {
      framebuffer[i] = col;
    }
    int end = HAL_GetTick();
    ltdc_duration_debug = end - begin;
  }
}
#endif
