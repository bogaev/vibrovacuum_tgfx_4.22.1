#include "signals.h"

// class Signal --------------------------------------------------------------

Signal::Signal(uint32_t sample_timer_period)
  : sample_rate_((FP_TYPE) sample_timer_period)
{}

void Signal::SetParam(uint8_t param, FP_TYPE value) {
  if (param == (uint8_t)SIG_GEN_PARAM_AMP) {
    SetAmp(value);
  }
  if (param == (uint8_t)SIG_GEN_PARAM_FREQ) {
    SetFreq(value);
  }
}

Signal& Signal::SetAmp(FP_TYPE amp) {
  amp_ = amp;
  return *this;
}

Signal& Signal::SetFreq(FP_TYPE freq) {
  freq_ = freq;
  period_ = 1.0 / freq_;
  return *this;
}

Signal& Signal::SetFmodDepth(FP_TYPE mod_depth_percent) {
  fmod_depth_percent_ = (uint8_t) mod_depth_percent;
  return *this;
}

FP_TYPE Signal::GetAmp() const {
  return amp_;
}

FP_TYPE Signal::GetFreq() const {
  return freq_;
}

FP_TYPE Signal::GetModDepth() const {
  return fmod_depth_percent_;
}

std::unique_ptr<Signal> Signal::Create(enSignalTypes sig_type) {
  if (sig_type == SIG_GEN_TYPE_NONE) {
    return nullptr;
  } else if (sig_type == SIG_GEN_TYPE_SINUS) {
    return std::make_unique<Sinus>(sample_rate_);
  } else if (sig_type == SIG_GEN_TYPE_SQUARE) {
    return std::make_unique<Square>(sample_rate_);
  } else if (sig_type == SIG_GEN_TYPE_TRIANGLE) {
    return std::make_unique<Triangle>(sample_rate_);
  } else if (sig_type == SIG_GEN_TYPE_SAW) {
    return std::make_unique<Saw>(sample_rate_);
  }
  return nullptr;
}

// class Sinus ---------------------------------------------------------------

Sinus::Sinus(uint32_t sample_timer_period)
  : Signal(sample_timer_period)
{}

FP_TYPE Sinus::GetValue(uint32_t point) const {
  FP_TYPE t = (FP_TYPE)point / sample_rate_;
  return amp_ * std::sinf(2.0 * pi * freq_ * t);
}

FP_TYPE Sinus::FreqMod(uint32_t point, Signal& fmod) const {
  FP_TYPE t = (FP_TYPE)point / sample_rate_;
  return amp_ * std::sinf(2.0 * pi * freq_ * t
                         + (freq_ - fmod.GetFreq()) / fmod.GetFreq()
                         * (5.0 * fmod_depth_percent_ / 100.0)
                         * fmod.GetIntegral(point));
}

FP_TYPE Sinus::GetIntegral(uint32_t point) const {
  FP_TYPE t = (FP_TYPE)point / sample_rate_;
  return (-1.0) * std::cosf(2.0 * pi * freq_ * t);
}

// class Square ---------------------------------------------------------------

Square::Square(uint32_t sample_timer_period)
  : Signal(sample_timer_period)
{}

FP_TYPE Square::GetValue(uint32_t point) const {
  int sample = (int)point % (int)(period_ * (FP_TYPE)sample_rate_);
  FP_TYPE t = (FP_TYPE)sample / (FP_TYPE)sample_rate_;
  return square(t);
}

FP_TYPE Square::FreqMod(uint32_t point, Signal& /*fmod*/) const {
  return GetValue(point);
}

FP_TYPE Square::GetIntegral(uint32_t point) const {
  int sample = (int)point % (int)(period_ * sample_rate_);
  FP_TYPE t = (FP_TYPE)sample / sample_rate_;
  if (t < period_ / 2.0) {
    return t;
  }
  return -t;
}

FP_TYPE Square::square(FP_TYPE t) const {
  if (t < period_ / 2.0) {
    return amp_;
  }
  return -amp_;
}

// class Triangle -------------------------------------------------------------

Triangle::Triangle(uint32_t sample_timer_period)
  : Signal(sample_timer_period)
{}

FP_TYPE Triangle::GetValue(uint32_t point) const {
  int sample = (int)point % (int)(period_ * sample_rate_);
  FP_TYPE t = (FP_TYPE)sample / sample_rate_;
  return amp_ * triangle(t);
}

FP_TYPE Triangle::FreqMod(uint32_t point, Signal& /*fmod*/) const {
  return GetValue(point);
}

FP_TYPE Triangle::GetIntegral(uint32_t point) const {
  int sample = (int)point % (int)(period_ * sample_rate_);
  FP_TYPE t = (FP_TYPE)sample / sample_rate_;

  if (t < period_ / 4.0) {
    return t * (2.0 * t / period_);
  } else if (t >= period_ / 4.0 && t < period_ * 3.0 / 4.0) {
    return t * (2.0 - 2.0 * t / period_);
  } else {
    return t * (2.0 * t / period_ - 4.0);
  }
}

FP_TYPE Triangle::triangle(FP_TYPE t) const {
  if (t < period_ / 4.0) {
    return 4.0 * t / period_;
  } else if (t >= period_ / 4.0 && t < period_ * 3.0 / 4.0) {
		return 2.0 - 4.0 * t / period_;
  } else {
    return 4.0 * t / period_ - 4.0;
  }
}

// class Saw ---------------------------------------------------------------

Saw::Saw(uint32_t sample_timer_period)
  : Signal(sample_timer_period)
{}

FP_TYPE Saw::GetValue(uint32_t point) const {
  int sample = (int)point % (int)(period_ * sample_rate_);
  FP_TYPE t = (FP_TYPE)sample / sample_rate_;
  return amp_ * sawtooth(t);
}

FP_TYPE Saw::FreqMod(uint32_t point, Signal& /*fmod*/) const {
  return GetValue(point);
}

FP_TYPE Saw::GetIntegral(uint32_t point) const {
  int sample = (int)point % (int)(period_ * sample_rate_);
  FP_TYPE t = (FP_TYPE)sample / sample_rate_;
  return t * (t - period_) / period_;
}

FP_TYPE Saw::sawtooth(FP_TYPE t) const {
  return 2.0 * t / period_ - 1.0;
}