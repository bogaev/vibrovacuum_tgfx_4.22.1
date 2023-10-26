#include "interrupt_manager.hpp"

#include "common\common.h"
#include "backend\api.hpp"
#include "components\lcd_touch\lcd_touch.hpp"

InterruptManager<uint16_t> exti_interrupt_manager;
InterruptManager<TimerHandle_t> os_tim_interrupt_manager;
InterruptManager<ADC_HandleTypeDef*> adc_interrupt_manager;
InterruptManager<TIM_HandleTypeDef*> hw_tim_interrupt_manager;

extern LcdTouch screen;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if(GPIO_Pin == GPIO_PIN_7) {
    screen.TouchCallback();
  }
  exti_interrupt_manager.HandleInterrupt(GPIO_Pin);
}

void vTimerCallback(TimerHandle_t xTimer) {
  os_tim_interrupt_manager.HandleInterrupt(xTimer);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  adc_interrupt_manager.HandleInterrupt(hadc);
}

void HW_TimBaseCallback(TIM_HandleTypeDef* htim) {
  hw_tim_interrupt_manager.HandleInterrupt(htim);
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef* hadc) {
}
