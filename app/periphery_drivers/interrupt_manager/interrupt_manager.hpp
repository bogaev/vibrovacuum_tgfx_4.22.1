#ifndef _INTERRUPT_MANAGER_H_
#define _INTERRUPT_MANAGER_H_

#include "main.h"
#include "cmsis_os.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include <vector>
#include <algorithm>
  
extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
extern "C" void vTimerCallback(TimerHandle_t xTimer);
extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc1);
extern "C" void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
extern "C" void HW_TimBaseCallback(TIM_HandleTypeDef* htim);

template<typename T>
class InterruptListener {
public:
  virtual bool ISR_Handler(T value) { 
    return false;
  }
};

template<typename T>
class InterruptManager {
public:
  InterruptManager() {
    listeners_.reserve(4);
  }
  
  void HandleInterrupt(T value) {
    for(auto& listener : listeners_) {
      listener->ISR_Handler(value);
    }
  }
  
  void SetListener(InterruptListener<T>* listener) {
    listeners_.push_back(listener);
  }
  
  void RemoveListener(InterruptListener<T>* listener) {
    listeners_.erase(std::remove(listeners_.begin(), listeners_.end(), listener), listeners_.end());
  }
  
private:
  std::vector<InterruptListener<T>*> listeners_;
};

extern InterruptManager<uint16_t> exti_interrupt_manager;
extern InterruptManager<TimerHandle_t> os_tim_interrupt_manager;
extern InterruptManager<ADC_HandleTypeDef*> adc_interrupt_manager;
extern InterruptManager<TIM_HandleTypeDef*> hw_tim_interrupt_manager;

#endif /* _INTERRUPT_MANAGER_H_ */
