#ifndef _TOUCH_SCREEN_CREATE_H_
#define _TOUCH_SCREEN_CREATE_H_

#include "components_drivers/touch/gt811.h"
#include "components_drivers/touch/gt811_waveshare.h"
#include "components_drivers/touch/gt911.h"

class TouchScreenFactory {
 public:
  TouchScreen* Init() {
      TouchScreen* is_init = nullptr;
//      HAL_GPIO_WritePin(LCD_BCK_LGHT_GPIO_Port, LCD_BCK_LGHT_Pin, GPIO_PIN_SET);
      HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);

      is_init = TryChip(new Gt811());
      if (is_init) {
        return is_init;
      }

      is_init = TryChip(new Gt811_WS());
      if (is_init) {
        return is_init;
      }

      is_init = TryChip(new Gt911());
      if (is_init) {
        return is_init;
      }

      HAL_GPIO_WritePin(LCD_BCK_LGHT_GPIO_Port, LCD_BCK_LGHT_Pin, GPIO_PIN_RESET);
      return nullptr;
  }

 private:
  TouchScreen* TryChip(TouchScreen* touch) {
    if (touch && touch->Init() == HAL_OK) {
      return touch;
    }
    delete touch;
    return nullptr;
  }
};

#endif /*_TOUCH_SCREEN_CREATE_H_*/
