#ifndef __GT911_H_
#define __GT911_H_

#include "stm32f7xx_hal.h"
#include "main.h"

#include "components/touch/touch_screen_interface.hpp"

#define GT911_MAX_WIDTH         1024    	//Touchscreen pad max width
#define GT911_MAX_HEIGHT        600			//Touchscreen pad max height

#define CT_CMD_WR               0XBA					//РґКэѕЭГьБо
#define CT_CMD_RD               0XBB					//¶БКэѕЭГьБо

#define CT_MAX_TOUCH            5						//µзИЭґҐГюЖБЧоґуЦ§іЦµДµгКэ

#define GT911_COMMAND_REG               0x8040	/* КµК±ГьБо */
#define GT911_CONFIG_REG                0x8047	/* ЕдЦГІОКэјДґжЖч */
#define GT911_PRODUCT_ID_REG            0x8140 	/* Product ID */
#define GT911_FIRMWARE_VERSION_REG      0x8144  /* №Мјю°ж±ѕєЕ */
#define GT911_READ_XY_REG               0x814E	/* Чш±кјДґжЖч */

#define GT911_RST_CLK()			__GPIOD_CLK_ENABLE()
#define GT911_RST_PORT			GPIOD
#define GT911_RST_PIN                   GPIO_PIN_11

#define GT911_INT_CLK()			__GPIOD_CLK_ENABLE()
#define GT911_INT_PORT			GPIOD
#define GT911_INT_PIN                   GPIO_PIN_7

typedef struct
{
  uint8_t Touch;
  uint8_t TouchpointFlag;
  uint8_t TouchCount;

  uint8_t Touchkeytrackid[CT_MAX_TOUCH];
  uint16_t X[CT_MAX_TOUCH];
  uint16_t Y[CT_MAX_TOUCH];
  uint16_t S[CT_MAX_TOUCH];
}
GT911_Dev;

uint16_t GT911_TEST(void);
void GT911_Init(void);
void GT911_Scan(void);

class Gt911 : public TouchScreen {
 public:
   HAL_StatusTypeDef Init() override;
   void CheckState() override;
   void SetEvent() override;
   Coord GetCoordinates() override {
     return{};
   }
};

#endif /*__GT911_H_*/
