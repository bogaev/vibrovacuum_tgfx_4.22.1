#ifndef _PUMP_CONTROLLER_H_
#define _PUMP_CONTROLLER_H_

#include "periphery_drivers/pwm/pwm_controller.hpp"
#include "backend/devices/pressure_sensor/pressure_sensor_controller.hpp"
#include "utility/timer_facility.hpp"
#include "utility/callback.hpp"

#ifndef NO_SERIALIZATION
#include "periphery_drivers/flash/flash.hpp"
#endif

namespace pump {

struct InitSettings {
  pwm::InitSettings pwm;
  sensor::PressureInitSettings pressure;
};

class State;

class Controller final
#ifndef NO_SERIALIZATION
  : public flash::SerializedClass
#endif
{
  const uint8_t kHighThreshold = 80;
  const uint8_t kLowThreshold = 20;

public:
  using PressureState = sensor::Pressure::State;
  enum enState {
    DEINIT = 0,
    STOPPED,
    RUN,
    STATES_NUM
  };

  Controller(InitSettings settings);
  ~Controller();
  void Start();
  void Stop();
  void Pause();
  void Run();

  void SetPumpState(bool state);
  void SetUserVacuum(uint8_t new_value);

  uint8_t GetUserVacuum();
  void SetPwm(uint32_t value);

  void SetState(enState new_state);
  bool GetState();

  void PressureEventsHandler(PressureState state);
  void SetTimer(uint8_t minutes);
  uint32_t GetTimerExpiryMS();
  int GetTimerMinutes();

#ifndef NO_SERIALIZATION
  std::vector<StoredData>& GetSerializedData() override;
  void Serialize(uint16_t idx, uint16_t data) override;
  void Deserialize() override;
#endif

  template<typename T>
  void SetExpireTimerHandler(T* owner, void (T::*func_ptr)());

private:
  uint32_t ConvertToPwmValue(uint32_t value);
  void OnTimerEnd();

  pwm::InitSettings pump_settings_;
  pwm::IT_Controller pwm_;

  sensor::Pressure pressure_sensor_;

  uint8_t vacuum_value_ = 0;
  bool is_state_on_ = false;
  uint8_t minutes_;

  State* states_[STATES_NUM];
  State* state_;

  tim::SW_FreeRTOS_Timer timer_;
  std::unique_ptr<_util::GenericCallback<>> expire_callback_;
};

template<typename T>
void Controller::SetExpireTimerHandler(T* owner, void (T::*func_ptr)()) {
  expire_callback_ = std::make_unique<_util::Callback<T>>(owner, func_ptr);
}

}

#endif // #ifndef _PUMP_CONTROLLER_H_