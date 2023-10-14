#ifndef _LCD_TOUCH_H_
#define _LCD_TOUCH_H_

#include "main.h"
#include "components/touch/touch_screen_factory.hpp"
#include "components/touch/touch_screen_factory.hpp"

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
extern "C" void LCD_TIM_Callback();
extern "C" HAL_StatusTypeDef LCD_TouchInit(void);

class LcdTouch {
 public:
  HAL_StatusTypeDef Init();
  void TouchCallback();
  void TouchCheckState();

 private:
  TouchScreen* touch_ = nullptr;
};

#endif /* _LCD_TOUCH_H_ */
