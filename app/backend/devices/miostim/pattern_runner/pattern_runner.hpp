#ifndef _PATTERN_RUNNER_H_
#define _PATTERN_RUNNER_H_

#include "common\common.h"
#include "periphery_drivers\interrupt_manager\interrupt_manager.hpp"
#include "periphery_drivers\pwm\pwm_controller.hpp"
#include "backend\devices\miostim\pwm_generator\pwm_generator.h"
#include "backend\devices\miostim\pwm_controller\pwm_controller.h"
#include "backend\devices\miostim\treatment_patterns\treatment_patterns.hpp"

#ifndef NO_SERIALIZATION
#include "periphery_drivers\flash\flash.hpp"
#endif

namespace miostim {

class PatternRunner
#ifndef NO_SERIALIZATION
  : public flash::SerializedClass
#endif
{
public:
  PatternRunner(InitSettings stngs);

  void Start();
  void Stop();
  void Run();

  void SetState(bool state);
  bool GetState();

  void SetPattern(TreatPatternsTypeDef pattern);
  TreatPatternsTypeDef GetPattern();

  void SetModFreq(enFreqMods freq);
  enFreqMods GetModFreq();

  void SetModDepth(enDepthModCoeffs depth);
  enDepthModCoeffs GetModDepth();

  void SetModRatio(enStagesRatio ratio);
  enStagesRatio GetModRatio();

  void SetTimer(uint8_t num_of_minutes);
  uint8_t GetMinutes();
  uint32_t GetTimerExpiryMS();

  template<typename T>
  void SetExpireTimerHandler(T* owner, void (T::*func_ptr)());

#ifndef NO_SERIALIZATION
  std::vector<StoredData>& GetSerializedData() override;
  void Serialize(uint16_t idx, uint16_t data) override;
  void Deserialize() override;
#endif

private:
  const uint32_t sample_rate_ = 0;
  TreatPatternsTypeDef pattern_ = TREAT_PATTERN_MODULATED_CONTINUOUS;
//  TreatPatternsTypeDef pattern_ = TREAT_PATTERN_MODULATED_PAUSED;

  std::unique_ptr<PwmController> pwm_controllers_[GEN_TOTAL_NUM];
  std::unique_ptr<TreatPattern> treatment_;

  bool is_state_on_ = false;
  uint8_t num_of_minutes_ = 0;
  enStagesRatio ratio_ = APR_10_15;
  enDepthModCoeffs depth_ = DM_0;
  enFreqMods freq_ = FM_10;
};

template<typename T>
void PatternRunner::SetExpireTimerHandler(T* owner, void (T::*func_ptr)()) {
  treatment_->SetExpireTimerHandler(owner, func_ptr);
}

} // namespace miostim

#endif // #ifndef _PATTERN_RUNNER_H_