#include "lcd_touch.hpp"

#include "components/touch/gt811.h"
#include "components/touch/gt911.h"

#include "tim.h"

//extern GT911_Dev Dev_Now;
static LcdTouch screen;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if(GPIO_Pin == GPIO_PIN_7) {
    screen.TouchCallback();
  }
}

void LCD_TIM_Callback() {
  screen.TouchCheckState();
}

HAL_StatusTypeDef LCD_TouchInit(void) {
  return screen.Init();
}

HAL_StatusTypeDef LcdTouch::Init() {
  touch_ = TouchScreenFactory{}.Init();
  if (!touch_) {
    return HAL_ERROR;
  }
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  HAL_TIM_Base_Start_IT(&htim11);
  return HAL_OK;
}

void LcdTouch::TouchCallback() {
  if (touch_) touch_->SetEvent();
}


void LcdTouch::TouchCheckState() {
  if (touch_) touch_->CheckState();
}
