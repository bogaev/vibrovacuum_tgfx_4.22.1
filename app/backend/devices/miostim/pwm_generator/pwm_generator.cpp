#include "pwm_generator.h"

namespace pwm_gen {

PwmGenerator::PwmGenerator(SignalModulator sig_mod,
                       const PwmGenerator::Settings& duty_cycle_settings,
                       const SignalStabilizer::Settings& stabilizer_settings,
                       uint8_t dead_time_th_percent) 
  : sig_mod_(std::move(sig_mod))
  , min_duty_cycle_(duty_cycle_settings.timer_period * duty_cycle_settings.min_percent / 100.)
  , max_duty_cycle_(duty_cycle_settings.timer_period * duty_cycle_settings.max_percent / 100.)
  , stabilizer_(stabilizer_settings.coeffs ?
                  std::make_unique<SignalStabilizer>(stabilizer_settings) : nullptr)
  , dead_time_th_((FP_TYPE)dead_time_th_percent / 100.0)
{}

/**
  * @brief  Изменение значения параметра сигнала
  */
void PwmGenerator::SetSignal(uint8_t signal, uint8_t param, FP_TYPE value) {
  // Reset();
  if (pwm_value_ != 0.0) {
//    BufferParams(signal, param, value);
    sig_mod_.SetSignal(signal, param, value);
  } else {
    sig_mod_.SetSignal(signal, param, value);
  }
}

/**
  * @brief  Вычисляет значение ШИМ в следующей точке
  * @retval Следующее значение ШИМ-сигнала
  */
uint32_t PwmGenerator::GetValue() {
  pwm_value_ = sig_mod_.GetValue();
  
  if ((pwm_value_ >= 0.0 && is_neg_halfwave_) || (pwm_value_ <= 0.0 && !is_neg_halfwave_)) {
//    while (!buffer_params_.empty()) {
//      CommitChanges(sig_mod_);
//    }
    is_neg_halfwave_ = !is_neg_halfwave_;
  }
  
//  if (sig_mod_.GetCarrierType() == SIG_GEN_TYPE_SINUS) {
//    CheckDeadTimeBasedOnAmpl(pwm_value_);
//  } else {
//    CheckDeadTimeBasedOnPeriod();
//  }
  
  return (uint32_t)(GetMinDutyCycle() + std::abs(GetRangeDutyCycle() * pwm_value_));
}

/**
  * @brief  Возвращает текущую полярность ШИМ-сигнала
  * @retval true - для отрицательной полуволны, false - для положительной
  */
bool PwmGenerator::IsNegHalfwave() const {
  return is_neg_halfwave_;
}

FP_TYPE PwmGenerator::GetMinDutyCycle() const {
  return min_duty_cycle_;
  // * (stabilizer_ ? stabilizer_->GetFreqCoeff(sig_mod_.GetFreq()) : 1.);
}

FP_TYPE PwmGenerator::GetMaxDutyCycle() const {
  return max_duty_cycle_;
  // * (stabilizer_ ? stabilizer_->GetFreqCoeff(sig_mod_.GetFreq()) : 1.);
}

FP_TYPE PwmGenerator::GetRangeDutyCycle() const {
  return (GetMaxDutyCycle() - GetMinDutyCycle());
  // * (stabilizer_ ? stabilizer_->GetAmpCoeff(sig_mod_.GetAmp()) : 1.);
}

//void PwmGenerator::CheckDeadTimeBasedOnPeriod() {
//  uint32_t period = (uint32_t)(1. / sig_mod_.GetFreq() * (FP_TYPE)sample_rate_);
//  uint32_t sample = sig_mod_.GetSampleNum();
//  uint32_t t = sample % period;
//  const FP_TYPE dead_time = dead_time_th_ / 4.0;
//  
//  if (	 (FP_TYPE)t / (FP_TYPE)period < dead_time
//      || (FP_TYPE)t / (FP_TYPE)period > (1. - dead_time)) {
//    pwm_value_ = 0.0;
//  }
//  
//  if (	 (FP_TYPE)t / (FP_TYPE)period > 0.5 - dead_time
//      && (FP_TYPE)t / (FP_TYPE)period < 0.5 + dead_time) {
//    pwm_value_ = 0.0;
//  }
//}
//
//void PwmGenerator::CheckDeadTimeBasedOnAmpl(FP_TYPE value) {
//  if (std::abs(value / sig_mod_.GetAmp()) < dead_time_th_) {
//    pwm_value_ = 0.0;
//  }
//}

/**
  * @brief  Сбрасывает значение текущего сигнала перед сменой параметров
  */
void PwmGenerator::Reset() {
  sig_mod_.Reset();
  is_neg_halfwave_ = false;
}

}
