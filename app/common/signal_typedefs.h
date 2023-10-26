#ifndef _SIG_GEN_TYPEDEFS_H_
#define _SIG_GEN_TYPEDEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define FP_TYPE float

typedef enum {
  SIG_GEN_IT_MODE = 0,
  SIG_GEN_DMA_MODE,
  SIG_GEN_MODE_NUM                       
} SIG_GEN_ModeTypeDef;

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
  SIG_GEN_CARRIER = 0,
  SIG_GEN_AMP_MOD,
  SIG_GEN_FREQ_MOD,
  SIG_GEN_NUM
} enSignals;

typedef enum {
  SIG_GEN_PARAM_SIGNAL_TYPE = 0,
  SIG_GEN_PARAM_AMP,
  SIG_GEN_PARAM_FREQ,
  SIG_GEN_PARAM_AMP_DEPTH,
  SIG_GEN_PARAM_FREQ_DEPTH,
  SIG_GEN_PARAM_NUM
} enSignalParams;

typedef enum {
  SIG_GEN_TYPE_NONE = 0,
  SIG_GEN_TYPE_SINUS,
  SIG_GEN_TYPE_SQUARE,
  SIG_GEN_TYPE_TRIANGLE,
  SIG_GEN_TYPE_SAW,
  SIG_GEN_TYPE_NUM
} enSignalTypes;

typedef struct {
  FP_TYPE from;
  FP_TYPE to;
  FP_TYPE coeff;
} SIG_GEN_RangeCoeff;

typedef struct {
  SIG_GEN_RangeCoeff* amp_array;
  uint32_t amp_array_size;
  SIG_GEN_RangeCoeff* freq_array;
  uint32_t freq_array_size;
} SIG_GEN_CoeffsInitStruct;

#ifdef __cplusplus
}
#endif

#endif // #ifndef _SIG_GEN_TYPEDEFS_H_
