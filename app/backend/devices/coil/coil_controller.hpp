#ifndef _COIL_CONTROLLER_H_
#define _COIL_CONTROLLER_H_

#include "periphery_drivers\pwm\pwm_controller.hpp"
#include "tim.h"
#include "utility\software_pwm.hpp"
#include "utility\callback.hpp"
#include "utility\timer_facility.hpp"

#ifndef NO_SERIALIZATION
#include "periphery_drivers\flash\flash.hpp"
#endif

#include <memory>

namespace coil {

struct InitSettings {
  pwm::InitSettings pwm;
  util::FreqInitSettings sw_pwm;
};

class Controller
#ifndef NO_SERIALIZATION
  : public flash::SerializedClass
#endif
{
public:
  using SW_Pwm = util::SoftwarePWM;
#ifndef NO_SERIALIZATION
  using flash::SerializedClass::StoredData;
#endif
  Controller(InitSettings settings);
  void Start();
  void Stop();
  void Run();

  void SetState(bool state);
  bool GetState();

  void SetIntesity(int value);
  int GetIntesity();

  void SetFrequency(int value);
  int GetFrequency();

  void SetTimer(uint8_t minutes);
  int GetTimerMinutes();
  uint32_t GetTimerExpiryMS();

#ifndef NO_SERIALIZATION
  std::vector<StoredData>& GetSerializedData() override;
  void Serialize(uint16_t idx, uint16_t data) override;
  void Deserialize() override;
#endif

  template<typename T>
  void SetExpireTimerHandler(T* owner, void (T::*func_ptr)());

private:
  void PwmPeriodChangeHandler(SW_Pwm::enPeriod state);
  void OnTimerEnd();

  InitSettings coil_settings_;
  pwm::IT_Controller pwm_;
  SW_Pwm sw_pwm_;

  bool is_state_on_ = false;
  uint8_t intensity_percents_;
  uint8_t freq_;
  uint8_t minutes_;

  tim::SW_FreeRTOS_Timer timer_;
  std::unique_ptr<_util::GenericCallback<>> expire_callback_;
};

template<typename T>
void Controller::SetExpireTimerHandler(T* owner, void (T::*func_ptr)()) {
  expire_callback_ = std::make_unique<_util::Callback<T>>(owner, func_ptr);
}

}

#endif // #ifndef _COIL_CONTROLLER_H_