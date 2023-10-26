#ifndef _VALUE_CONVERTERS_H_
#define _VALUE_CONVERTERS_H_

#include "tim.h"
#include "tim.h"

#include <array>

namespace _util {

inline uint32_t PercentsToTimerValue(TIM_HandleTypeDef* timer, int8_t value) {
  return uint32_t(timer->Init.Period * float(value) / 100.);
}

} // namespace _util

#endif // _VALUE_CONVERTERS_H_