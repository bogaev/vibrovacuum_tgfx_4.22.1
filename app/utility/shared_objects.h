#ifndef _SHARED_OBJECTS_H_
#define _SHARED_OBJECTS_H_

#include "utility\observer.h"
#include "common\common.h"
//#include "signal_generator\params_updater\params_updater.h"

struct SharedObjects {
//  static _pattern::Publisher mod_period_end;
  inline static _pattern::Publisher<tdUartData> uart_received{};
//  static miostim::SignalUpdater signal_updater;
  inline static _pattern::Publisher<tdSignalParams> signal_updater{};
};

#endif // _SHARED_OBJECTS_H_