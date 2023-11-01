/**
  ******************************************************************************
  * @file    signal_modulator.h
  * @author  bogaev.s@gmail.com
  * @brief   Файл определяет класс - композицию из 3 сигналов
  *          для вычисления результирующего модулированного сигнала
  ******************************************************************************
  */

#ifndef _SIGNAL_MODULATOR_H_
#define _SIGNAL_MODULATOR_H_

#include <cmath>
#include <utility>
#include <memory>

#include "main.h"
#include "backend\devices\miostim\signal_generator_c_interface\signal_generator_c_interface.h"
#include "backend\devices\miostim\signals\signals.h"
#include "utility\observer.h"
#include "common\common.h"

namespace pwm_gen {
  class PwmGenerator;
}
/**
  * @brief  Вычисление значения модулированного сигнала
  */
class SignalModulator : public _pattern::Observer<tdSignalParams> {
 public:
  friend class pwm_gen::PwmGenerator;

  explicit SignalModulator(uint32_t sample_rate);

  SignalModulator(uint32_t sample_rate,
                  enSignalTypes carrier,
                  enSignalTypes amod = SIG_GEN_TYPE_NONE,
                  enSignalTypes fmod = SIG_GEN_TYPE_NONE);
  SignalModulator(SignalModulator&& other) = default;
  virtual ~SignalModulator();

  void SetSignal(uint8_t signal, uint8_t param, FP_TYPE value);
  void Update(tdSignalParams msg) override;

  FP_TYPE GetValue();
  FP_TYPE GetCarrierAmp() const;
  FP_TYPE GetFreq(enSignals signal = SIG_GEN_CARRIER) const;
  FP_TYPE GetPeriod(enSignals signal = SIG_GEN_CARRIER) const;

 private:
  SignalModulator& GenerateCarrier();
  SignalModulator& AddAmpMod();
  SignalModulator& GenerateFreqMod();
  void Reset();
  uint32_t GetSampleNum() const;

  const uint32_t sample_rate_; /// частота семплирования сигнала
  std::unique_ptr<Signal> carrier_; /// сигнал с параметрами несущего сигнала
  std::unique_ptr<Signal> amod_; /// сигнал с параметрами частотной модуляции
  std::unique_ptr<Signal> fmod_; /// сигнал с параметрами частотной модуляции

  uint8_t amod_depth_percent_ = 100;
  FP_TYPE mod_sig_value_ = 0.0;
  uint32_t sample_ = 0; // номер текущего семпла (точки) для которого вычислется значение
};

#endif // #ifndef _SIGNAL_MODULATOR_H_
