#ifndef _BACKEND_API_H_
#define _BACKEND_API_H_

#include "common\common.h"

#ifndef NO_COIL
#include "backend\devices\coil\coil_controller.hpp"
#endif

#ifndef NO_PUMP
#include "backend\devices\pump\pump_controller.hpp"
#include "backend\devices\pressure_sensor\pressure_sensor_controller.hpp"
#endif
#ifndef NO_ELECTRO
#include "backend\devices\miostim\pattern_runner\pattern_runner.hpp"
#endif
#ifndef NO_SERIALIZATION
#include "mcu_periphery\flash\flash.hpp"
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "main.h"
#include "cmsis_os.h"

#include <memory>
#include <vector>
#include <array>
#include <initializer_list>

class Backend {
public:
  static Backend* GetInstance();
  Backend(Backend const& s) = delete;
  Backend& operator=(Backend const& s) = delete;
  void Start();
  void Stop();

#ifndef NO_COIL
  coil::Controller& Coil() {
    return coil_;
  }
#endif
#ifndef NO_PUMP
  pump::Controller& Pump() {
    return pump_;
  }
#endif
#ifndef NO_ELECTRO
  miostim::PatternRunner& Miostim() {
    return miostim_;
  }
#endif
#ifndef NO_SERIALIZATION
  void AddSerializedClass(flash::SerializedClass* cls) {
    flash_.AddClass(std::move(cls));
  }
#endif

private:
#ifndef NO_COIL
  coil::Controller coil_;
#endif
#ifndef NO_PUMP
  pump::Controller pump_;
#endif
#ifndef NO_ELECTRO
  miostim::PatternRunner miostim_;
#endif
#ifndef NO_SERIALIZATION
  flash::Controller flash_;
#endif

  static inline Backend* backend_ = nullptr;

  Backend(
#ifndef NO_COIL
          coil::InitSettings coil_settings,
#endif
          pump::InitSettings pump_settings
#ifndef NO_ELECTRO
        , miostim::InitSettings mio_settings
#endif
            ) :
#ifndef NO_COIL
      coil_(coil_settings),
#endif
      pump_(pump_settings)
#ifndef NO_ELECTRO
    , miostim_(mio_settings)
#endif
#ifndef NO_SERIALIZATION
    , flash_(std::vector<flash::SerializedClass*>{ &coil_, &pump_, &miostim_ })
#endif
  {}
};

extern "C" void InitBackend();

#endif /* _BACKEND_API_H_ */