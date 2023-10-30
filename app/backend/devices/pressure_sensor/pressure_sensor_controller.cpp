#include "pressure_sensor_controller.hpp"

namespace sensor {

void Pressure::AdcConversationEndHandler() {
    pressure_accumulator_ += adc_.GetValue();
}

void Pressure::AccumulationTimerHandler() {
  if (pressure_cnt_ == kCountMax) {
    current_pressure_ = (uint16_t)
      ((double)pressure_accumulator_ / (double)pressure_cnt_);
    pressure_accumulator_ = 0;
    pressure_cnt_ = 0;
    CheckPressureState();
  }
  StartNextMeasure();
  pressure_cnt_ += 1;
}

}
