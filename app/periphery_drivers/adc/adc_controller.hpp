#ifndef _ADC_CONTROLLER_H_
#define _ADC_CONTROLLER_H_

#include "adc.h"
#include "periphery_drivers\interrupt_manager\interrupt_manager.hpp"
#include "utility\callback.hpp"

#include <vector>
#include <numeric>
#include <cassert>
#include <memory>

namespace adc {

struct InitSettings {
  ADC_HandleTypeDef* hadc;
  size_t buffer_size;
};

// ADC BaseController class =============================================

class BaseController : public InterruptListener<ADC_HandleTypeDef*> {
public:
  BaseController(InitSettings settings);
  virtual ~BaseController();
  virtual void Start() = 0;
  void Stop();

  template<typename T>
  void SetAdcConversationEndHandler(T* owner, void (T::*func_ptr)());
  bool ISR_Handler(ADC_HandleTypeDef* hadc) override;

protected:
  std::unique_ptr<_util::GenericCallback<>> measure_end_callback_;
  ADC_HandleTypeDef* hadc_ = nullptr;
};

template<typename T>
void BaseController::SetAdcConversationEndHandler(T* owner, void (T::*func_ptr)()) {
  measure_end_callback_ = std::make_unique<_util::Callback<T>>(owner, func_ptr);
}

// ADC IT_Controller class =============================================

class IT_Controller : public BaseController {
public:
  using BaseController::hadc_;

  IT_Controller(InitSettings settings);
  void Start() override;
  uint16_t GetValue();
};

// ADC DMAController =============================================

class DMAController : public BaseController {
public:
  using BaseController::hadc_;

  DMAController(InitSettings settings);
  void Start() override;
  uint16_t GetValue();

private:
  std::vector<uint16_t> buffer_;
};

} // namespace adc

#endif // #ifndef _ADC_CONTROLLER_H_