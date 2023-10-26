#ifndef _BACKEND_API_H_
#define _BACKEND_API_H_

#include "common\common.h"
#include "devices\pump\pump_controller.hpp"
//#include "devices\coil\coil_controller.hpp"
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

//  coil::Controller& Coil() {
//    return coil_;
//  }

  pump::Controller& Pump() {
    return pump_;
  }

//  miostim::PatternRunner& Miostim() {
//    return miostim_;
//  }

  void Start();
  void Stop();
//  void AddSerializedClass(flash::SerializedClass* cls) {
//    flash_.AddClass(std::move(cls));
//  }

private:
//  coil::Controller coil_;
  pump::Controller pump_;
//  miostim::PatternRunner miostim_;
//  flash::Controller flash_;

  static inline Backend* backend_ = nullptr;

  Backend(//coil::InitSettings coil_settings,
          pump::InitSettings pump_settings
//          miostim::InitSettings mio_settings
         )
    : //coil_(coil_settings)
      pump_(pump_settings)
//    , miostim_(mio_settings)
//    , flash_(std::vector<flash::SerializedClass*>{ &coil_, &pump_, &miostim_ })
  {}
};

extern "C" void InitBackend();

#endif /* _BACKEND_API_H_ */