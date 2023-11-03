#include "timer_facility.hpp"

#define pdTICKS_TO_MS( xTicks ) ( ( uint32_t ) ( ( ( uint32_t ) ( xTicks ) * ( uint32_t ) 1000 ) / ( uint32_t ) configTICK_RATE_HZ ) )

#define IS_IRQ_MODE()             (__get_IPSR() != 0U)
#define IS_IRQ_MASKED()           ((__get_PRIMASK() != 0U) || (__get_BASEPRI() != 0U))
extern osKernelState_t KernelState;
#define IS_IRQ()                  (IS_IRQ_MODE() || (IS_IRQ_MASKED() && (KernelState == osKernelRunning)))

namespace tim {

// TimerInterface class =============================================

TimerInterface::TimerInterface(TimerPeriod period_ms) :
  period_ms_(period_ms)
{}

// Software OS Timer class =============================================

SW_FreeRTOS_Timer::SW_FreeRTOS_Timer(void* owner,
                                        TimerPeriod period_ms,
                                        enAutoreload auto_reload) :
  owner_(owner),
  TimerInterface(period_ms),
  auto_reload_(auto_reload)
{
  InitFreeRTOS_Timer();
  os_tim_interrupt_manager.SetListener(this);
}

SW_FreeRTOS_Timer::~SW_FreeRTOS_Timer() {
  Stop();
  xTimerDelete(tim_handle_, osWaitForever);
}

void SW_FreeRTOS_Timer::Start() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if (!IS_IRQ()) {
    xTimerStart(tim_handle_, osWaitForever);
  } else {
    if (xTimerStartFromISR(tim_handle_, &xHigherPriorityTaskWoken) != pdTRUE) {
      Error_Handler();
    }
    portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
  }
}

void SW_FreeRTOS_Timer::Stop() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if (!IS_IRQ()) {
    xTimerStop(tim_handle_, osWaitForever);
  } else {
    if (xTimerStopFromISR(tim_handle_, &xHigherPriorityTaskWoken) != pdTRUE) {
      Error_Handler();
    }
    portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
  }
}

void SW_FreeRTOS_Timer::Reset() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if (!IS_IRQ()) {
    xTimerReset(tim_handle_, osWaitForever);
  } else {
    if (xTimerResetFromISR(tim_handle_, &xHigherPriorityTaskWoken) != pdTRUE) {
      Error_Handler();
    }
    portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
  }
}

void SW_FreeRTOS_Timer::ChangePeriod(TimerPeriod period_ms) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  period_ms_ = period_ms;

  if (!IS_IRQ()) {
    xTimerChangePeriod(tim_handle_, pdMS_TO_TICKS(period_ms_), osWaitForever);
    xTimerStop(tim_handle_, osWaitForever);
  } else {
    if (xTimerChangePeriodFromISR(tim_handle_, pdMS_TO_TICKS(period_ms_), &xHigherPriorityTaskWoken) != pdTRUE) {
      Error_Handler();
    }
    if (xTimerStopFromISR(tim_handle_, &xHigherPriorityTaskWoken) != pdTRUE) {
      Error_Handler();
    }
    portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
  }

//  if (xTimerIsTimerActive(tim_handle_) == pdFALSE) {
//    xTimerChangePeriod(tim_handle_, pdMS_TO_TICKS(period_ms_), osWaitForever);
//    xTimerStop(tim_handle_, osWaitForever);
//  } else {
//    xTimerChangePeriod(tim_handle_, pdMS_TO_TICKS(period_ms_), osWaitForever);
//  }
}

TimerHandle_t SW_FreeRTOS_Timer::GetHandler() {
  return tim_handle_;
}

size_t SW_FreeRTOS_Timer::GetTimeMS() {
  return (size_t) period_ms_;
}

uint32_t SW_FreeRTOS_Timer::GetExpiryTimeMS() {
  TickType_t ticks;
  if (xTimerIsTimerActive(tim_handle_) == pdFALSE) {
    ticks = xTimerGetPeriod(tim_handle_);
  } else {
    ticks = xTimerGetExpiryTime(tim_handle_) - xTaskGetTickCount();
  }
  return pdTICKS_TO_MS(ticks);
}

void SW_FreeRTOS_Timer::InitFreeRTOS_Timer() {
     tim_handle_ = xTimerCreate
               ( /* Just a text name, not used by the RTOS
                 kernel. */
                 "Timer",
                 /* The timer period in ticks, must be
                 greater than 0. */
                 pdMS_TO_TICKS(period_ms_),
                 /* The timers will auto-reload themselves
                 when they expire. */
                 auto_reload_,
                 /* The ID is used to store a count of the
                 number of times the timer has expired, which
                 is initialised to 0. */
                 ( void * ) owner_,
                 /* Each timer calls the same expire_callback when
                 it expires. */
                 vTimerCallback
               );

     if( tim_handle_ == NULL ) {
         /* The timer was not created. */
     }
}

bool SW_FreeRTOS_Timer::ISR_Handler(TimerHandle_t timer) {
  if (tim_handle_ == timer) {
    if (expire_callback_->isValid()) {
      expire_callback_->execute();
    }
    return true;
  }
  return false;
}

// Hardware STM32 Timer class =============================================

HW_STM32_BaseTimer::HW_STM32_BaseTimer(TIM_HandleTypeDef* hal_handler,
                                       TimerPeriod period_ms) :
  hal_handler_(hal_handler),
  TimerInterface(period_ms),
  counter_(0)
{
  hw_tim_interrupt_manager.SetListener(this);
  HAL_TIM_RegisterCallback(hal_handler_, HAL_TIM_PERIOD_ELAPSED_CB_ID, HW_TimBaseCallback);
}

HW_STM32_BaseTimer::~HW_STM32_BaseTimer() {
  Stop();
  hw_tim_interrupt_manager.RemoveListener(this);
}

void HW_STM32_BaseTimer::Start() {
  HAL_TIM_StateTypeDef tim_state = HAL_TIM_Base_GetState(hal_handler_);
  if(tim_state == HAL_TIM_STATE_READY) {
    HAL_TIM_Base_Start_IT(hal_handler_);
  }
  else if (tim_state == HAL_TIM_STATE_BUSY) {
  }
  else {
    assert(false);
  }
}

void HW_STM32_BaseTimer::Stop() {
  HAL_TIM_Base_Stop_IT(hal_handler_);
}

void HW_STM32_BaseTimer::Reset() {
  Stop();
  Start();
}

void HW_STM32_BaseTimer::ChangePeriod(TimerPeriod period_ms) {
  period_ms_ = period_ms;
}

TIM_HandleTypeDef* HW_STM32_BaseTimer::GetHandler() {
  return hal_handler_;
}

bool HW_STM32_BaseTimer::ISR_Handler(TIM_HandleTypeDef* timer) {
  if(hal_handler_ == timer) {
    ++counter_;
    if(counter_ >= period_ms_ && expire_callback_->isValid()) {
      expire_callback_->execute();
      counter_ = 0;
    }
    Start();
    return true;
  }
  return false;
}

} // namespace tim
