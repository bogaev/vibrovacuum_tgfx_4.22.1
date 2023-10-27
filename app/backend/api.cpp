#include "backend\api.hpp"
#include "stm32f7xx_hal_adc.h"

//static SIG_GEN_RangeCoeff amp_array_[] = {
//  { 0., 0.3, 1.0 },
//  { 0.3, 0.5, 1.0 },
//  { 0.5, 1.0, 1.0 }
//};
//
//static SIG_GEN_RangeCoeff freq_array_[] = {
//  { 0., 20., 1.0 },
//  { 20., 50., 1.0 },
//  { 50., 5'000., 1.0 }
//};
//
//static SIG_GEN_CoeffsInitStruct corr_array_[] = {
//  amp_array_,
//  3,
//  freq_array_,
//  3
//};

void InitBackend() {
  Backend::GetInstance();
}

Backend* Backend::GetInstance() {
  if(backend_ == nullptr) {
      backend_ = new Backend(
//        coil::InitSettings {
//          pwm::InitSettings {
//            .timer = &htim5,
//            .channel = TIM_CHANNEL_2
//          }, // pwm_vibration
//          util::FreqInitSettings {
//            .freq = 10.,
//            .duty_cycle = 0.5
//          }
//        },
        pump::InitSettings {
           pwm::InitSettings{
             .timer = &htim4,
             .channel = { TIM_CHANNEL_2 }
           },  // pwm_vacuum
           sensor::PressureInitSettings {
             adc::InitSettings {
                .hadc = &hadc1,
                .buffer_size = sensor::kBufferSize
             },
             .threshhold = 500
           }
        }
//        miostim::InitSettings {
//          .pwm_timer = &htim3,
//          .pattern_timer = &htim14,
//          .sample_timer = &htim13,
//          { TIM_CHANNEL_2, TIM_CHANNEL_3 },
//          .min_duty_cycle_percent = 0,
//          .max_duty_cycle_percent = 100,
//          .dead_time_th_percent = 0,
//          0
//        }
    );
  }

  return backend_;
}

void Backend::Start() {
//  flash_.Init();
//  coil_.Start();
  pump_.Start();
//  miostim_.Start();

//  static pwm::IT_Controller pwm_controller_test(&htim3, TIM_CHANNEL_2);
//  pwm_controller_test.Start();
//  pwm_controller_test.SetPwm(100);

#if defined (__DEBUG)
//  InitTest();
#endif
}

void Backend::Stop() {
//  coil_.Stop();
  pump_.Stop();
//  miostim_.Stop();
}
