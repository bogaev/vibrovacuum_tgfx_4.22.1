#ifndef _BACKEND_API_H_
#define _BACKEND_API_H_

#include "common\common.h"
#include "backend\devices\pump\pump_controller.hpp"
//#include "backend\devices\coil\coil_controller.hpp"
//#include "devices\pressure_sensor\pressure_sensor_controller.hpp"
//#include "devices\miostimul\pattern_runner\pattern_runner.hpp"
//#include "mcu_periphery\flash\flash.hpp"

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

#ifndef DISCONNECTED_COIL
  coil::Controller& Coil() {
    return coil_;
  }
#endif

  pump::Controller& Pump() {
    return pump_;
  }

//  miostim::PatternRunner& Miostim() {
//    return miostim_;
//  }

  void Start();
  void Stop();
#ifndef NO_SERIALIZATION
  void AddSerializedClass(flash::SerializedClass* cls) {
    flash_.AddClass(std::move(cls));
  }
#endif

private:
#ifndef DISCONNECTED_COIL
  coil::Controller coil_;
#endif
  pump::Controller pump_;
//  miostim::PatternRunner miostim_;
#ifndef NO_SERIALIZATION
  flash::Controller flash_;
#endif

  static inline Backend* backend_ = nullptr;

  Backend(
#ifndef DISCONNECTED_COIL
          coil::InitSettings coil_settings,
#endif
          pump::InitSettings pump_settings
//          miostim::InitSettings mio_settings
            ) :
#ifndef DISCONNECTED_COIL
      coil_(coil_settings),
#endif
      pump_(pump_settings)
//    , miostim_(mio_settings)
#ifndef NO_SERIALIZATION
    , flash_(std::vector<flash::SerializedClass*>{ &coil_, &pump_, &miostim_ })
#endif
  {}
};

extern "C" void InitBackend();

#endif /* _BACKEND_API_H_ */