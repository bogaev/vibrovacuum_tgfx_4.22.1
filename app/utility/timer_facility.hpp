#ifndef _TIMER_FACILITY_H_
#define _TIMER_FACILITY_H_

#include "argument_user_types.hpp"
#include "callback.hpp"
#include "periphery_drivers\interrupt_manager\interrupt_manager.hpp"

#include "tim.h"

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "timers.h"

#include <cassert>
#include <memory>

extern "C" void vTimerCallback(TimerHandle_t xTimer);

namespace tim {

// TimerInterface class =============================================

class TimerInterface {
public:
  TimerInterface(TimerPeriod period_ms);
  virtual ~TimerInterface() {};
  virtual void Start() = 0;
  virtual void Stop() = 0;
  virtual void Reset() = 0;
  virtual void ChangePeriod(TimerPeriod period_ms) = 0;
  template<typename T>
  void SetExpireTimerHandler(T* owner, void (T::*func_ptr)());

protected:
  TimerPeriod period_ms_;
  std::unique_ptr<_util::GenericCallback<>> expire_callback_;
};

template<typename T>
void TimerInterface::SetExpireTimerHandler(T* owner, void (T::*func_ptr)()) {
  expire_callback_ = std::make_unique<_util::Callback<T>>(owner, func_ptr);
}

// Software OS Timer class =============================================

class SW_FreeRTOS_Timer : public TimerInterface, public InterruptListener<TimerHandle_t> {
public:
  using TimerInterface::period_ms_;
  using TimerInterface::expire_callback_;

  SW_FreeRTOS_Timer(void* owner,
                    TimerPeriod period_ms,
                    enAutoreload auto_reload);
  ~SW_FreeRTOS_Timer() override;
  void Start() override;
  void Stop() override;
  void Reset() override;
  void ChangePeriod(TimerPeriod period_ms) override;
  size_t GetTimeMS();
  uint32_t GetExpiryTimeMS();
  TimerHandle_t GetHandler();
  bool ISR_Handler(TimerHandle_t timer) override;

private:
  void InitFreeRTOS_Timer();

  void* owner_;
  TimerHandle_t tim_handle_;
  enAutoreload auto_reload_;
};

// Hardware STM32 Timer class =============================================

class HW_STM32_BaseTimer : public TimerInterface, public InterruptListener<TIM_HandleTypeDef*> {
public:
  using TimerInterface::period_ms_;
  using TimerInterface::expire_callback_;

  HW_STM32_BaseTimer(TIM_HandleTypeDef* hal_handler,
                     TimerPeriod period_ms);
  ~HW_STM32_BaseTimer();
  void Start() override;
  void Stop() override;
  void Reset() override;
  void ChangePeriod(TimerPeriod period_ms) override;
  TIM_HandleTypeDef* GetHandler();
  bool ISR_Handler(TIM_HandleTypeDef* timer);

private:
  TIM_HandleTypeDef* hal_handler_;
  int id_ = 0;
  size_t counter_;
};

} // namespace tim

#endif // _TIMER_FACILITY_H_