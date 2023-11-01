#include "backend\devices\miostim\signal_modulator\signal_modulator.h"
#include "utility\shared_objects.h"

SignalModulator::SignalModulator(uint32_t sample_rate)
      : sample_rate_(sample_rate)
      , carrier_(Signal{sample_rate_}.Create(SIG_GEN_TYPE_SINUS))
      , amod_(Signal{sample_rate_}.Create(SIG_GEN_TYPE_NONE))
      , fmod_(Signal{sample_rate_}.Create(SIG_GEN_TYPE_NONE)) {
  SharedObjects::signal_updater.RegisterObserver(this);
}

SignalModulator::SignalModulator(uint32_t sample_rate,
                                 enSignalTypes carrier,
                                 enSignalTypes amod,
                                 enSignalTypes fmod)
      : sample_rate_(sample_rate)
      , carrier_(Signal{sample_rate_}.Create(carrier))
      , amod_(Signal{sample_rate_}.Create(amod))
      , fmod_(Signal{sample_rate_}.Create(fmod)) {
  SharedObjects::signal_updater.RegisterObserver(this);
}

SignalModulator::~SignalModulator() {
  SharedObjects::signal_updater.RemoveObserver(this);
};

void SignalModulator::Update(tdSignalParams msg) {
  SetSignal(msg.signal, msg.param, msg.value);
}

/**
  * @brief  Вычисляет отдельное значение результирующего
  *         модулированного сигнала
  * @retval Значение сигнала в точке sample_
  */
//uint32_t cycles_num = 0;
FP_TYPE SignalModulator::GetValue() {
  if (carrier_ && amod_ && fmod_) { // если все сигналы != nullptr
    GenerateFreqMod().AddAmpMod();
  } else if (carrier_ && amod_) { // если сигналы carrier_ и amod_ != nullptr
    GenerateCarrier().AddAmpMod();
  } else if (carrier_ && fmod_) { // если сигналы carrier_ и fmod_ != nullptr
    GenerateFreqMod();
  } else if (carrier_) { // если задан только несущий сигнал
//    StartCyclesCounter();
    GenerateCarrier();
//    cycles_num = GetCyclesCounter();
//    StopCyclesCounter();
  } else {
    mod_sig_value_ = 0.0; // если не задано никакого сигнала - выводится 0
  }
  ++sample_;
//  if (sample_ > 0 && (sample_ % amp_mod_period_) < uint32_t(amp_mod_period_ * 0.01)) {
//    SharedObjects::mod_period_end.NotifyObservers();
//  }
  return mod_sig_value_;
}

/**
  * @brief  Вычисляет значение несущего сигнала
  */
SignalModulator& SignalModulator::GenerateCarrier() {
  mod_sig_value_ = carrier_->GetValue(sample_);
  return *this;
}

/**
  * @brief  Добавляет амплитудную модуляцию к несущему сигналу
  *         с учетом глубины модуляции (amod_depth_percent_)
  */
SignalModulator& SignalModulator::AddAmpMod() {
  mod_sig_value_ *= (std::abs(amod_->GetValue(sample_))
            * ((FP_TYPE)amod_depth_percent_ / 100.0)
                + (1.0 - (FP_TYPE)amod_depth_percent_ / 100.0));
  return *this;
}

/**
  * @brief  Вычисляет значение частотно-модулированного сигнала
  */
SignalModulator& SignalModulator::GenerateFreqMod() {
  mod_sig_value_ = carrier_->FreqMod(sample_, *fmod_);
  return *this;
}

/**
  * @brief  Возвращает значение амплитуды несущего сигнала
  * @retval Значение амплитуды несущего сигнала
  */
FP_TYPE SignalModulator::GetCarrierAmp() const {
  // TODO
  return carrier_->GetAmp();
}

/**
  * @brief  Возвращает значение частоты сигнала
  *
  *         Возвращает значение частоты одного из сигнала:
  *           несущего, амплитудной модуляции, частотной модуляции
  * @param  signal сигнал, частота которого возвращается
  * @retval Значение частоты выбранного сигнала
  */
FP_TYPE SignalModulator::GetFreq(enSignals signal) const {
  if (signal == SIG_GEN_CARRIER) {
    return carrier_->GetFreq();
  } else if (signal == SIG_GEN_AMP_MOD) {
    return amod_->GetFreq();
  } else if (signal == SIG_GEN_FREQ_MOD) {
    return fmod_->GetFreq();
  }
  return 0.0;
}

FP_TYPE SignalModulator::GetPeriod(enSignals signal) const {
  return 1.0 / GetFreq(signal);
}

uint32_t SignalModulator::GetSampleNum() const {
  return sample_;
}

/**
  * @brief  Устанавливает значение параметра сигнала
  * @param  signal - сигнал, параметр которого изменяется (см. enSignals)
  * @param  param - параметр, который изменяется (см. enSignalParams)
  * @param  value - новое значение параметра сигнала
  */
void SignalModulator::SetSignal(uint8_t signal, uint8_t param, FP_TYPE value) {
  Reset();
  if (signal == SIG_GEN_CARRIER) {
    if (param == SIG_GEN_PARAM_SIGNAL_TYPE) {
      carrier_ = Signal(sample_rate_).Create(static_cast<enSignalTypes>(value));
      return;
    }
    carrier_->SetParam(param, value);
  } else if (signal == SIG_GEN_AMP_MOD) {
    if (param == SIG_GEN_PARAM_AMP_DEPTH) {
      amod_depth_percent_ = (uint8_t) value;
      return;
    }
    if (param == SIG_GEN_PARAM_SIGNAL_TYPE) {
      amod_ = Signal(sample_rate_).Create(static_cast<enSignalTypes>(value));
      return;
    }
    if (param == SIG_GEN_PARAM_FREQ) {
      amod_->SetParam(param, value / 2.0);
      return;
    }
    amod_->SetParam(param, value);
  } else if (signal == SIG_GEN_FREQ_MOD) {
    if (param == SIG_GEN_PARAM_FREQ_DEPTH) {
      carrier_->SetFmodDepth(value);
      return;
    }
    if (param == SIG_GEN_PARAM_SIGNAL_TYPE) {
      fmod_ = Signal(sample_rate_).Create(static_cast<enSignalTypes>(value));
      return;
    }
    if (param == SIG_GEN_PARAM_FREQ) {
      fmod_->SetParam(param, value);
      return;
    }
    fmod_->SetParam(param, value);
  }
}

void SignalModulator::Reset() {
  sample_ = 0;
  mod_sig_value_ = 0.0;
}
