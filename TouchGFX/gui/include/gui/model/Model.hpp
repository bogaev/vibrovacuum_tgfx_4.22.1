#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>

#ifdef DISCONNECTED_BACKEND
//#include <common_\common.h>
#else
#include "system/backend.hpp"
#include "mcu_periphery\flash\flash.hpp"
#endif

#include <tuple>
#include <utility>

class ModelListener;

using TimersStats = std::tuple<std::pair<bool, int>,
                               std::pair<bool, int>,
                               std::pair<bool, int>>;

constexpr uint8_t MAX_ALPHA = 255;

class Model
#ifndef DISCONNECTED_BACKEND
  : public flash::SerializedClass
#endif
{
public:
  Model();

  void bind(ModelListener* listener)
  {
    modelListener = listener;
  }

  void tick();

  bool GetVacuumCheck();
  bool GetVibrationCheck();
  bool GetElectrostimCheck();

  void SetVacuumCheck(bool state);
  void SetVibrationCheck(bool state);
  void SetElectrostimCheck(bool state);
  void OnButtonStart(bool state);

  TimersStats GetExpiryTimeMS();

#ifndef DISCONNECTED_BACKEND
  std::vector<StoredData>& GetSerializedData() override;
  void Serialize(uint16_t idx, uint16_t data) override;
  void Deserialize() override;

  Backend& GetBackend() {
    return *backend_;
  }
#endif

protected:
  ModelListener* modelListener;

private:
  bool is_vacuum_on_ = false;
  bool is_vibration_on_ = false;
  bool is_electrostim_on_ = false;

  bool is_vacuum_expired_ = true;
  bool is_vibration_expired_ = true;
  bool is_electrostim_expired_ = true;

#ifndef DISCONNECTED_BACKEND
  bool is_backend_started_ = false;
  Backend* backend_ = nullptr;

  void StartBackend(void) {
    if (!is_backend_started_) {
      backend_ = Backend::GetInstance();
      backend_->Start();
      is_backend_started_ = true;
    }
  }
#endif

  void OnCoilTimerExpired();
  void OnPumpTimerExpired();
  void OnMiostimTimerExpired();
  bool IsTotalStateExpired();
};

#endif // MODEL_HPP
