#ifndef _GT811_WS_H_
#define _GT811_WS_H_

#include "stm32f7xx_hal.h"

#include "components/touch/touch_screen_interface.hpp"

#define  GT811_MAX_WIDTH        ((uint16_t)1024)    	//Touchscreen pad max width
#define  GT811_MAX_HEIGHT       ((uint16_t)600)     	//Touchscreen pad max height

#define GT811_CMD_WR            0XBA									//РґКэѕЭГьБо
#define GT811_CMD_RD            0XBB									//¶БКэѕЭГьБо
#define GT811_EXCHG_XY          1											//µчЧЄXYЧш±к

#define GT811_MAX_TOUCH    	5											//µзИЭґҐГюЖБЧоґуЦ§іЦµДµгКэ
#define GT811_READ_XY_REG 	0x721  								//¶БИЎЧш±кјДґжЖч
#define GT811_CONFIG_REG   	0x6A2									//ЕдЦГІОКэЖрКјјДґжЖч

#define GT811_RST_CLK()         __GPIOD_CLK_ENABLE()
#define GT811_RST_PORT          GPIOD
#define GT811_RST_PIN           GPIO_PIN_11

#define GT811_DEBUG             1

typedef struct
{
  volatile uint8_t Flag;
  uint8_t		Count;
  uint8_t		Static;
  uint8_t		TouchPointFlag;
  uint16_t 	x[GT811_MAX_TOUCH];										//ґҐГюXЧш±к
  uint16_t 	y[GT811_MAX_TOUCH];										//ґҐГюYЧш±к
  uint8_t		ppr[GT811_MAX_TOUCH];									//ґҐГюµгµДС№Б¦
}GT811_Device;

extern GT811_Device 	GT811_Dev;

//uint8_t GT811_Init(void);	    										//іхКј»ЇGT811
//void GT811_Scan(void);														//ЙЁГиGT811

class Gt811_WS : public TouchScreen {
 public:
   HAL_StatusTypeDef Init() override;
   bool CheckState(int32_t& x, int32_t& y) override;
   void SetEvent() override;
};


#endif /*_GT811_WS_H_*/
