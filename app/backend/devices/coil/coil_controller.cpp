#include "tim.h"
#include "coil_controller.hpp"

#include "utility\value_converters.hpp"

using namespace std;

namespace coil {

Controller::Controller(InitSettings settings)
      : coil_settings_(std::move(settings))
      , pwm_({{coil_settings_.pwm.timer, coil_settings_.pwm.channel}})
      , sw_pwm_(SW_Pwm::TimType::OS, coil_settings_.sw_pwm)
      , timer_(this, 1_ms, AUTORELOAD_OFF) {
  sw_pwm_.SetPwmPeriodChangeHandler(this, &Controller::PwmPeriodChangeHandler);
  timer_.SetExpireTimerHandler(this, &Controller::OnTimerEnd);
#ifndef NO_SERIALIZATION
  sdata_.resize(4);
#endif
}

void Controller::Start() {
  pwm_.Start();
  pwm_.SetPwmAllChannels(0);
#ifndef NO_SERIALIZATION
  Deserialize();
#endif
//!!  sw_pwm_.Start();
}

void Controller::Stop() {
  pwm_.SetPwmAllChannels(0);
  pwm_.Stop();
  sw_pwm_.Stop();
  timer_.Stop();
}

void Controller::Run() {
//  if (is_state_on_) {
    Start();
    sw_pwm_.Start();
    uint32_t v = _util::PercentsToTimerValue(coil_settings_.pwm.timer, intensity_percents_ * 0.6);
    pwm_.SetPwmAllChannels(v);
//    if (timer_.GetTimeMS() > 1) {
//      timer_.Start();
//    }
//  }
}

void Controller::SetState(bool state) {
  if (is_state_on_ != state) {
    is_state_on_ = state;
#ifndef NO_SERIALIZATION
    Serialize(0, (uint16_t)is_state_on_);
#endif
  }

  if (is_state_on_) {
    Run();
    if (timer_.GetTimeMS() > 1) {
      timer_.Start();
    }
  } else {
    Stop();
    pwm_.SetPwmAllChannels(0);
  }
}

bool Controller::GetState() {
  return is_state_on_;
}

void Controller::SetIntesity(int value) {
  if (intensity_percents_ != value) {
    intensity_percents_ = (uint8_t) value;
#ifndef NO_SERIALIZATION
    Serialize(2, intensity_percents_);
#endif
  }

  if (is_state_on_) {
    uint32_t v = _util::PercentsToTimerValue(coil_settings_.pwm.timer, intensity_percents_ * 0.6);
    pwm_.SetPwmAllChannels(v);
  }
}

int Controller::GetIntesity() {
  return (int) intensity_percents_;
}

void Controller::SetFrequency(int value) {
  if (freq_ != value) {
    freq_ = (uint8_t) value;
#ifndef NO_SERIALIZATION
    Serialize(1, freq_);
#endif
    sw_pwm_.SetFrequency(freq_);
  }
}

int Controller::GetFrequency() {
  return (int) freq_;
}

void Controller::PwmPeriodChangeHandler(SW_Pwm::enPeriod state) {
  if (is_state_on_) {
    if (state == SW_Pwm::enPeriod::HIGH) {
      auto v = _util::PercentsToTimerValue(coil_settings_.pwm.timer, intensity_percents_ * 0.6);
      pwm_.SetPwmAllChannels(v);
    } else {
      pwm_.SetPwmAllChannels(0);
    }
  }
}

#ifndef NO_SERIALIZATION
std::vector<Controller::StoredData>& Controller::GetSerializedData() {
  return sdata_;
}

void Controller::Serialize(uint16_t idx, uint16_t data) {
  sdata_[idx].value = data;
  if (flash_) {
    flash_->StoreToFlash(sdata_[idx]);
  }
}

void Controller::Deserialize() {
  if (flash_) {
    flash_->LoadFromFlash(sdata_);
  }
  minutes_ = sdata_[3].value;
  if (minutes_ > 1U) {
    timer_.ChangePeriod( TimerPeriod{ minutes_ * 1000U * 1U } );
  }
  intensity_percents_ = sdata_[2].value;
  freq_ = sdata_[1].value;
  is_state_on_ = (bool)sdata_[0].value;
}
#endif

void Controller::SetTimer(uint8_t minutes) {
  if (minutes_ != minutes) {
    minutes_ = minutes;
#ifndef NO_SERIALIZATION
    Serialize(3, minutes_);
#endif
  }

  if (minutes_ == 0) {
    timer_.ChangePeriod( TimerPeriod{ 1U } );
  } else {
    timer_.ChangePeriod( TimerPeriod{ minutes_ * 1000U * 1U } );
  }
  timer_.Stop();
}

int Controller::GetTimerMinutes() {
  return (int) minutes_;
}

uint32_t Controller::GetTimerExpiryMS() {
  return timer_.GetExpiryTimeMS();
}

void Controller::OnTimerEnd() {
  Stop();
  if (expire_callback_->isValid()) {
    expire_callback_->execute();
  }
}

}