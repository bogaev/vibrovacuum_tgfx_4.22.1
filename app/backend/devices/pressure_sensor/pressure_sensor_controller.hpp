#ifndef _PRESSURE_SENSOR_H_
#define _PRESSURE_SENSOR_H_

#include "main.h"
#include "utility\callback.hpp"
#include "utility\timer_facility.hpp"
#include "periphery_drivers\adc\adc_controller.hpp"
#include "periphery_drivers\interrupt_manager\interrupt_manager.hpp"

#include <math.h>
#include <numeric>

#define THRESHOLD_LOW 1400
#define THRESHOLD_HI 1500
#define THRESHOLD_ROOF 2200
#define RANGE (double)(THRESHOLD_ROOF - THRESHOLD_HI)

namespace sensor {

static const int kBufferSize = 1024;

enum STATES {
  STATE_NORMAL = 0,
  STATE_OVERFLOW,
  STATE_NUM
};

struct PressureInitSettings {
  adc::InitSettings adc;
  uint16_t threshhold;
};

class Pressure {
  const double kVcc = 5.2;
  const int kCountMax = 20;

public:
  enum State {
    DEINIT = 0,
    ABOVE_TH,
    BELOW_TH,
    STATES_NUM
  };

  Pressure(PressureInitSettings settings) :
    adc_(settings.adc),
    threshhold_(settings.threshhold),
    accumulation_timer_(this, 5_ms, AUTORELOAD_ON)
  {
    adc_.SetAdcConversationEndHandler(this, &Pressure::AdcConversationEndHandler);
    accumulation_timer_.SetExpireTimerHandler(this, &Pressure::AccumulationTimerHandler);
  }

  void Start() {
    adc_.Start();
    accumulation_timer_.Start();
  }

  void StartNextMeasure() {
    adc_.Start();
  }

  void Stop() {
    adc_.Stop();
  }

  void SetThresholdHi(uint16_t th) {
    threshhold_ = th;
  }

  void CheckPressureState() {
    if(current_pressure_ > threshhold_) {
      state_ = ABOVE_TH;
      if(state_changed_callback_->isValid())
        state_changed_callback_->execute(state_);
    }
    else {
      state_ = BELOW_TH;
      if(state_changed_callback_->isValid())
        state_changed_callback_->execute(state_);
    }
  }

  uint16_t GetPressure() {
    uint16_t raw_adc_value = adc_.GetValue();

//    double voltage_value =
//      ((double)raw_adc_value / (double)4096) * (double) kVcc;
//
//    double pressure =
//      ( (double) voltage_value - ((double) 0.04 * (double) kVcc) )
//     / ((double) 0.009 * (double) kVcc);

    return (uint16_t) raw_adc_value;
  }

  template<typename T>
  void SetChangePressureStateHandler(T* owner, void (T::*func_ptr)(State));
  void AdcConversationEndHandler();
  void AccumulationTimerHandler();

private:
  adc::IT_Controller adc_;
  uint16_t threshhold_ = 0;

  tim::SW_FreeRTOS_Timer accumulation_timer_;
  std::unique_ptr<_util::GenericCallback<State>> state_changed_callback_;

  uint16_t current_pressure_ = 0;
  size_t pressure_accumulator_ = 0;
  size_t pressure_cnt_ = 0;
  State state_ = DEINIT;
};

template<typename T>
void Pressure::SetChangePressureStateHandler(T* owner, void (T::*func_ptr)(State)) {
  state_changed_callback_ = std::make_unique<_util::Callback<T, State>>(owner, func_ptr);
}

}

#endif // _PRESSURE_SENSOR_H_