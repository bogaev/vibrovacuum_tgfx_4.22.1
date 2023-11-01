/**
  ******************************************************************************
  * @file    pwm_generator.h
  * @author  bogaev.s@gmail.com
  * @brief   Файл определяет классы для вычисления ШИМ на основе
  *          модулированного сигнала
  ******************************************************************************
  */

#ifndef _PWM_GENERATOR_H_
#define _PWM_GENERATOR_H_

#include <cmath>
#include <memory>
#include <queue>

#include "main.h"
#include "backend\devices\miostim\signal_generator_c_interface\signal_generator_c_interface.h"
#include "backend\devices\miostim\signal_modulator\signal_modulator.h"

namespace pwm_gen {

const FP_TYPE amp_coeff[3][2] = {
  { 1.0,  0.15 },
  { 0.6,  1.7  },
  { 0.45, 1.7  }
};

class SignalStabilizer final {
 public:
  struct Settings {
    const SIG_GEN_CoeffsInitStruct* coeffs = nullptr;
  };

  SignalStabilizer(const SignalStabilizer::Settings& settings)
    : coeffs_(settings.coeffs),
      max_amp_(coeffs_->amp_array[coeffs_->freq_array_size - 1].to),
      max_freq_(coeffs_->freq_array[coeffs_->freq_array_size - 1].to)
  {}

  FP_TYPE GetAmpCoeff(FP_TYPE current_amp) const {
    return GetCoeff(current_amp,
                    coeffs_->amp_array,
                    coeffs_->amp_array_size);
  }

  FP_TYPE GetFreqCoeff(FP_TYPE current_freq) const {
    return GetCoeff(current_freq,
                    coeffs_->freq_array,
                    coeffs_->freq_array_size);
  }

 private:
  FP_TYPE GetCoeff(FP_TYPE value, SIG_GEN_RangeCoeff* array, uint32_t size) const {
    for (uint32_t i = 0; i < size; ++i) {
      if (value >= array[i].from && value < array[i].to) {
        return array[i].coeff / 100.0;
      }
      if (i == size-1 && value >= array[i].from && value <= array[i].to) {
        return array[i].coeff / 100.0;
      }
    }
    return 1.0;
  }

  const SIG_GEN_CoeffsInitStruct* coeffs_;
  const FP_TYPE max_amp_;
  const FP_TYPE max_freq_;
};

/**
  * @brief  Вычисляет ШИМ на основе модулированного сигнала
  */
class PwmGenerator final {
 public:
  struct Settings {
    FP_TYPE min_percent;
    FP_TYPE max_percent;
    FP_TYPE timer_period;
  };

  PwmGenerator(SignalModulator sig_mod,
               const PwmGenerator::Settings& duty_cycle_settings,
               const SignalStabilizer::Settings& stabilizer_settings,
               uint8_t dead_time_th_percent);

  void SetSignal(uint8_t signal, uint8_t param, FP_TYPE value);
  uint32_t GetValue();
  bool IsNegHalfwave() const;
//  const SignalModulator& SigGen() {
//    return sig_mod_;
//  }

 private:
  struct tdSignalParamsBuffer {
    uint8_t signal = 0;
    uint8_t param = 0;
    FP_TYPE value = 0.0;
  };

  void CommitChanges(SignalModulator& sig_gen) {
    auto p = buffer_params_.front();
    sig_gen.SetSignal(p.signal, p.param, p.value);
    buffer_params_.pop();
  }

  void BufferParams(uint8_t signal, uint8_t param, FP_TYPE value) {
    buffer_params_.push({ signal, param, value });
  }

  FP_TYPE GetMinDutyCycle() const;
  FP_TYPE GetMaxDutyCycle() const;
  FP_TYPE GetRangeDutyCycle() const;
  void CheckDeadTimeBasedOnPeriod();
  void CheckDeadTimeBasedOnAmpl(FP_TYPE value);
  void Reset();

  SignalModulator sig_mod_; /// результирующий модулированный сигнал для вычисления ШИМ
  const FP_TYPE min_duty_cycle_;
  const FP_TYPE max_duty_cycle_;
  const std::shared_ptr<SignalStabilizer> stabilizer_;
  const FP_TYPE dead_time_th_ = 0.0;
//  const uint32_t sample_rate_ = 0;
  bool is_neg_halfwave_ = false;
  FP_TYPE pwm_value_ = 0.0;
  std::queue<tdSignalParamsBuffer> buffer_params_;
};

}

#endif // #ifndef _PWM_GENERATOR_H_
