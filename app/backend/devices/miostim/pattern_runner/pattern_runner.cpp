#include "pattern_runner.hpp"
#include "backend\devices\miostim\signal_modulator\signal_modulator.h"
#include "common\sig_gen_config.h"

namespace miostim {

static SIG_GEN_StatusTypeDef CheckCorrStruct(SIG_GEN_RangeCoeff* array,
                                             uint32_t size) {
  if(array[0].from != 0) {
    return SIG_GEN_ERROR_INCORRECT_BOUNDS;
  }

  for(uint32_t i = 0; i < size-1; ++i) {
    FP_TYPE cur_end = array[i].to;
    FP_TYPE next_start = array[i+1].from;
    if(cur_end != next_start) {
      return SIG_GEN_ERROR_INCORRECT_BOUNDS;
    }
  }
  return SIG_GEN_OK;
}

PatternRunner::PatternRunner(InitSettings sttngs)
//  , sample_rate_(sttngs.coeffs ? sttngs.coeffs->freq_array[sttngs.coeffs->freq_array_size-1].to * 2. : 1000)
  : sample_rate_(CARRIER_FREQ_MAX_HZ * POINTS_PER_PERIOD_NUM)
{
  if (sttngs.pwm_timer == 0) {
    return;
  }

  pwm_gen::PwmGenerator::Settings duty_cycle_settings;
  duty_cycle_settings.min_percent = (FP_TYPE)sttngs.min_duty_cycle_percent;
  duty_cycle_settings.max_percent = (FP_TYPE)sttngs.max_duty_cycle_percent;
  duty_cycle_settings.timer_period = (FP_TYPE)sttngs.pwm_timer->Init.Period;

  pwm_gen::SignalStabilizer::Settings stabilizer_settings = {0};
  if(sttngs.coeffs != 0) {
    {
      auto err = CheckCorrStruct(sttngs.coeffs->amp_array,
                                 sttngs.coeffs->amp_array_size);
      if (err) {
        return;
      }
    }
    {
      auto err = CheckCorrStruct(sttngs.coeffs->freq_array,
                                 sttngs.coeffs->freq_array_size);
      if (err) {
        return;
      }
    }
    stabilizer_settings.coeffs = sttngs.coeffs;
  }

  for(uint32_t i = 0; i < GEN_TOTAL_NUM; ++i) {
    auto pwm_gen = pwm_gen::PwmGenerator(
                    SignalModulator((uint32_t)sample_rate_, SIG_GEN_TYPE_SINUS),
                    duty_cycle_settings,
                    stabilizer_settings,
                    sttngs.dead_time_th_percent);

//    pwm_controllers_ = std::make_unique<IT_PwmController>(
//                  sttngs.pwm_timer,
//                  tdPwmChannels{sttngs.channels[0], sttngs.channels[1]},
//                  std::move(pwm_gen),
//                  &htim13,
//                  (BUF_DATA_TYPE*)(BUF_ADDRESS_START),
//                   BUF_SIZE);

    pwm_controllers_[i] = std::make_unique<DMA_PwmController>(
                  sttngs.pwm_timer,
                  tdPwmChannels{sttngs.channels[0], sttngs.channels[1]},
                  std::move(pwm_gen),
                  BUF_MODE_SINGLE,
                  (BUF_DATA_TYPE*)(BUF_ADDRESS_START + i * SDRAM_BANK_SIZE),
                   BUF_SIZE);
  }

  treatment_ = std::make_unique<TreatPattern>(pwm_controllers_, sample_rate_);
#ifndef NO_SERIALIZATION
  sdata_.resize(6);
#endif
}

void PatternRunner::Start() {
#ifndef NO_SERIALIZATION
  Deserialize();
#endif
  treatment_->Start();
//  pwm_controllers_->Start();
}

void PatternRunner::Stop() {
  treatment_->Stop();
}

void PatternRunner::Run() {
  treatment_->Run();
}

void PatternRunner::SetPattern(TreatPatternsTypeDef pattern) {
  if (pattern_ != pattern) {
    pattern_ = pattern;
    treatment_.reset();
    treatment_ = TreatPattern::Create(pattern_);
#ifndef NO_SERIALIZATION
    Serialize(0, (uint16_t)pattern_);
#endif
  }
}

TreatPatternsTypeDef PatternRunner::GetPattern() {
  return pattern_;
}

void PatternRunner::SetModFreq(enFreqMods freq) {
  if (freq_ != freq) {
    freq_ = freq;
    treatment_->SetModFreq(freq_);
#ifndef NO_SERIALIZATION
    Serialize(1, (uint16_t)freq_);
#endif
  }
}

enFreqMods PatternRunner::GetModFreq() {
  return freq_;
}

void PatternRunner::SetModDepth(enDepthModCoeffs depth) {
  if (depth_ != depth) {
    depth_ = depth;
    treatment_->SetModDepth(depth_);
#ifndef NO_SERIALIZATION
    Serialize(2, (uint16_t)depth_);
#endif
  }
}

enDepthModCoeffs PatternRunner::GetModDepth() {
  return depth_;
}

void PatternRunner::SetModRatio(enStagesRatio ratio) {
  if (ratio_ != ratio) {
    ratio_ = ratio;
    treatment_->SetModRatio(ratio_);
#ifndef NO_SERIALIZATION
    Serialize(3, (uint16_t)ratio);
#endif
  }
}

enStagesRatio PatternRunner::GetModRatio() {
  return ratio_;
}

void PatternRunner::SetTimer(uint8_t num_of_minutes) {
  if (num_of_minutes_ != num_of_minutes) {
    num_of_minutes_ = num_of_minutes;
    treatment_->SetTimer(num_of_minutes_);
#ifndef NO_SERIALIZATION
    Serialize(4, (uint16_t)num_of_minutes_);
#endif
  }
}

uint8_t PatternRunner::GetMinutes() {
  return num_of_minutes_;
}

void PatternRunner::SetState(bool state) {
  if (is_state_on_ != state) {
    is_state_on_ = state;
    treatment_->SetState(is_state_on_);
#ifndef NO_SERIALIZATION
    Serialize(5, (uint16_t)is_state_on_);
#endif
  }
}

bool PatternRunner::GetState() {
  return is_state_on_;
}

uint32_t PatternRunner::GetTimerExpiryMS() {
  return treatment_->GetTimerExpiryMS();
}

#ifndef NO_SERIALIZATION
std::vector<PatternRunner::StoredData>& PatternRunner::GetSerializedData() {
  return sdata_;
}

void PatternRunner::Serialize(uint16_t idx, uint16_t data) {
  sdata_[idx].value = data;
  if (flash_) {
    flash_->StoreToFlash(sdata_[idx]);
  }
}

void PatternRunner::Deserialize() {
  if (flash_) {
    flash_->LoadFromFlash(sdata_);
  }
  SetModFreq((enFreqMods) sdata_[1].value);
  SetModDepth((enDepthModCoeffs) sdata_[2].value);
  SetModRatio((enStagesRatio) sdata_[3].value);
  if (sdata_[4].value > 1U) {
    SetTimer(sdata_[4].value);
  }
  SetState((bool)sdata_[5].value);
}
#endif

} // namespace miostim
