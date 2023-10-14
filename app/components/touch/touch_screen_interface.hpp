#ifndef _TOUCH_SCREEN_H_
#define _TOUCH_SCREEN_H_

#include "main.h"

struct Coord {
  uint16_t x;
  uint16_t y;
};

class TouchScreen {
 public:
   virtual ~TouchScreen() {};
   virtual HAL_StatusTypeDef Init() = 0;
   virtual void CheckState() = 0;
   virtual void SetEvent() = 0;
   virtual Coord GetCoordinates() = 0;
};

#endif /*_TOUCH_SCREEN_H_*/
