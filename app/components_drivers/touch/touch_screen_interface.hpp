#ifndef _TOUCH_SCREEN_H_
#define _TOUCH_SCREEN_H_

#include "main.h"

class TouchScreen {
 public:
   virtual ~TouchScreen() {};
   virtual HAL_StatusTypeDef Init() = 0;
   virtual bool CheckState(int32_t& x, int32_t& y) = 0;
   virtual void SetEvent() = 0;
};

#endif /*_TOUCH_SCREEN_H_*/
