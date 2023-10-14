#ifndef _CONFIG_TESTS_H_
#define _CONFIG_TESTS_H_

#define LTDC_TEST_ON
#define SDRAM_TEST_ON
#define FLASH_TEST_ON
#define DMA2D_TEST_ON
#define TOUCH_TEST_ON

#ifdef LTDC_TEST_ON
  #include "ltdc_tests.h"
#endif
#ifdef SDRAM_TEST_ON
  #include "sdram_tests.h"
#endif
#ifdef FLASH_TEST_ON
  #include "flash_tests.h"
#endif
#ifdef DMA2D_TEST_ON
  #include "dma2d_tests.h"
#endif

#ifdef TOUCH_TEST_ON
//  #define TOUCH_DEBUG_ON
#endif

#endif /* _CONFIG_TESTS_H_ */
