#include "pump_controller.hpp"
#include "pump_states.hpp"
#include "utility\value_converters.hpp"

volatile static uint32_t current_pressure_debug;

using namespace std;

namespace pump {

Controller::Controller(InitSettings settings)
  : pump_settings_(settings.pwm)
  , pwm_({{settings.pwm.timer, settings.pwm.channel}})
  , pressure_sensor_(settings.pressure)
  , timer_(this, 1_ms, AUTORELOAD_OFF)
{
  states_[DEINIT] = new State(*this);
  states_[STOPPED] = new StopState(*this);
  states_[RUN] = new RunState(*this);
  state_ = states_[STOPPED];
  pressure_sensor_.SetChangePressureStateHandler(this, &Controller::PressureEventsHandler);
  timer_.SetExpireTimerHandler(this, &Controller::OnTimerEnd);
#ifndef NO_SERIALIZATION
  sdata_.resize(3);
#endif
}

Controller::~Controller() {
  for(int i = 0; i < STATES_NUM; ++i) {
    delete states_[i];
  }
}

void Controller::Start() {
  pressure_sensor_.Start();
  pwm_.Start();
#ifndef NO_SERIALIZATION
  Deserialize();
#endif
}

void Controller::Stop() {
  SetPwm(0);
  pressure_sensor_.Stop();
  pwm_.Stop();
  timer_.Stop();
  SetState(STOPPED);
}

void Controller::Pause() {
  Stop();
}

void Controller::Run() {
//  if (is_state_on_) {
    Start();
    state_->SetUserEvent();
//    if (timer_.GetTimeMS() > 1) {
//      timer_.Start();
//    }
//  }
}

void Controller::SetPumpState(bool state) {
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
  }
}

void Controller::SetUserVacuum(uint8_t new_value) {
  if (vacuum_value_ != new_value) {
    vacuum_value_ = new_value;
#ifndef NO_SERIALIZATION
    Serialize(1, (uint16_t)vacuum_value_);
#endif
  }

  if (is_state_on_) {
    state_->SetUserEvent();
  }
}

uint8_t Controller::GetUserVacuum() {
  return vacuum_value_;
}

void Controller::SetPwm(uint32_t percents) {
  uint32_t value = _util::PercentsToTimerValue(pump_settings_.timer, percents * 0.8);
  pwm_.SetPwmAllChannels(value);
}

void Controller::SetState(enState new_state) {
  state_ = states_[new_state];
}

bool Controller::GetState() {
  return is_state_on_;
}

void Controller::PressureEventsHandler(PressureState pressure_state) {
//  state_->PressureEvent(pressure_state);
}

void Controller::SetTimer(uint8_t minutes) {
  if (minutes_ != minutes) {
    minutes_ = minutes;
#ifndef NO_SERIALIZATION
    Serialize(2, (uint16_t)minutes_);
#endif
  }

  if (minutes == 0) {
    timer_.ChangePeriod( TimerPeriod{1U} );
  } else {
    timer_.ChangePeriod( TimerPeriod{ minutes * 1000U * 1U} );
  }
  timer_.Stop();
}

uint32_t Controller::GetTimerExpiryMS() {
  return timer_.GetExpiryTimeMS();
}

int Controller::GetTimerMinutes() {
  return (int) minutes_;
}

void Controller::OnTimerEnd() {
  Stop();
  if (expire_callback_->isValid()) {
    expire_callback_->execute();
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
  minutes_ = sdata_[2].value;
  if (minutes_ > 1U) {
    timer_.ChangePeriod( TimerPeriod{ minutes_ * 1000U * 1U } );
  }
  vacuum_value_ = sdata_[1].value;
  is_state_on_ = (bool)sdata_[0].value;
}
#endif

}