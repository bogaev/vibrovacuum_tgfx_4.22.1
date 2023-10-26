#ifndef CUSTOMTYPES_H
#define CUSTOMTYPES_H

#include <touchgfx/Color.hpp>

#if defined(WIN32)
#else
#include "main.h"
#endif

#include "common\signal_typedefs.h"

const int EMITTERS_NUM = 8;

typedef enum {
  BUF_MODE_SINGLE = 0,
  BUF_MODE_DOUBLE,
  BUF_MODE_NUM
} BufferModeTypeDef;

typedef struct {
  uint16_t value;
  uint16_t reserved1;
  uint8_t emitter;
  uint8_t signal;
  uint8_t param;
  uint8_t reserved2;
} tdUartData;

typedef struct {
  uint8_t signal;
  uint8_t param;
  FP_TYPE value;
} tdSignalParams;

typedef struct {
  tdUartData data;
  uint32_t crc;
} tdUartMessage;

namespace miostim {

#if defined(WIN32)
#else
struct InitSettings {
  TIM_HandleTypeDef* pwm_timer;
  TIM_HandleTypeDef* pattern_timer;
  TIM_HandleTypeDef* sample_timer;
  uint32_t channels[2];
  uint8_t min_duty_cycle_percent;
  uint8_t max_duty_cycle_percent;
  uint8_t dead_time_th_percent;
  SIG_GEN_CoeffsInitStruct* coeffs;
};
#endif

typedef enum {
  SIG_GEN_OK = 0,
  SIG_GEN_ERROR_PWM_TIMER_NOT_SET,
  SIG_GEN_ERROR_SAMPLE_RATE_NOT_SET,
  SIG_GEN_ERROR_SAMPLE_RATE_LESS_THAN_FREQ,
  SIG_GEN_ERROR_AMPLITUDE_VALUE_ABOVE_MAX,
  SIG_GEN_ERROR_PWM_NOT_INITED,
  SIG_GEN_ERROR_INCORRECT_BOUNDS,
  SIG_GEN_ERROR_NUM
} SIG_GEN_StatusTypeDef;

typedef enum {
  TREAT_PATTERN_MODULATED_CONTINUOUS = 0,
  TREAT_PATTERN_MODULATED_PAUSED,
  TREAT_PATTERN_MODULATED_NON_MODULATED,
  TREAT_PATTERN_MODULATED_FIXED_150,
  TREAT_PATTERN_MODULATED_FIXED_150_PAUSED,
  TREAT_PATTERN_NUM
} TreatPatternsTypeDef;

namespace mod_values {
  constexpr static float MIN_ACT_TIME_MS = 200.;
  constexpr static float CARRIER_FREQ = 5000.;
  constexpr static float FIXED_MOD_FREQ = 150.;

  typedef enum { FM_10 = 0, FM_20, FM_30, FM_50, FM_80, FM_100, FM_150 } enFreqMods;
  constexpr static int FREQ_MOD_NUM = 7;
  constexpr static float FREQ_MOD[FREQ_MOD_NUM] = { 10., 20., 30., 50., 80., 100., 150. };

  typedef enum { DM_0 = 0, DM_25, DM_50, DM_75, DM_100 } enDepthModCoeffs;
  constexpr static int DEPTH_MOD_COEFF_NUM = 5;
  constexpr static float DEPTH_MOD_PERCENT[DEPTH_MOD_COEFF_NUM] = { 0., 25., 50., 75., 100. };

  typedef enum { APR_10_15 = 0, APR_2_3, APR_4_6 } enStagesRatio;
  constexpr static int STAGES_RATIO_NUM = 3;
  constexpr static float STAGES_RATIO[STAGES_RATIO_NUM][3] = {
    {1.0, 1.5, 10.0},
    {2.0, 3.0, 20.0},
    {4.0, 6.0, 40.0}
  };

  typedef enum { FLEX = 0, FIXED } enStagesFreqs;
  constexpr static int STAGES_FREQ_NUM = 2;
  constexpr static float STAGES_FREQ[STAGES_FREQ_NUM] = {CARRIER_FREQ, FIXED_MOD_FREQ};
}

}

#endif // CUSTOMTYPES_H
