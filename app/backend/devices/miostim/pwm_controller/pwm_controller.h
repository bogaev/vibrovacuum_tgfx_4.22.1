/**
  ******************************************************************************
  * @file    pwm_generator.h
  * @author  bogaev.s@gmail.com
  * @brief   Файл определяет классы, управляющие генерацией ШИМ
  *          в различных режимах работы периферии STM32 (IT, DMA)
  ******************************************************************************
  */

#ifndef _PWM_CONTROLLER_H_
#define _PWM_CONTROLLER_H_

#include <memory>
#include <functional>
#include <unordered_map>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "main.h"
#include "cmsis_os.h"

#include "common\common.h"
#include "common\sig_gen_config.h"
#include "backend\devices\miostim\signals\signals.h"
#include "backend\devices\miostim\pwm_generator\pwm_generator.h"
#include "utility\os_tasks_wrapper.h"
#include "utility\callback.hpp"
#include "utility\observer.h"

class RTOSTaskWrapper;

struct tdPwmChannels {
  uint32_t pos_halfwave_channel;
  uint32_t neg_halfwave_channel;
};

/**
  * @brief  Базовый класс для контроллера генерации ШИМ
  */
class PwmController : public _pattern::Observer<tdUartData> {
 public:
  PwmController(TIM_HandleTypeDef* timer,
                tdPwmChannels channels,
                pwm_gen::PwmGenerator generator);
  virtual ~PwmController();

  virtual void Start() = 0;
  virtual void Stop() = 0;
  virtual void Run() = 0;
  virtual void ISR_Handler() = 0;
  virtual void SetSignal(uint8_t signal, uint8_t param, FP_TYPE value);

 protected:
  static void PWM_PulseHalfFinishedCallback(TIM_HandleTypeDef *htim);
  static void PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);
  static void BindTimerToInstance(TIM_HandleTypeDef* htim, PwmController* ptr);
  static PwmController* GetTimerInstance(TIM_HandleTypeDef* htim);
  static HAL_TIM_ActiveChannel GetActiveChannel(uint32_t tim_ch);
  void Update(tdUartData msg) override;

  static PwmController* htim_to_inst[9]; /// таблица хендлер таймера -> экземпляр класса
  TIM_HandleTypeDef* timer_ = nullptr; /// таймер ШИМ STM32
  tdPwmChannels channels_; /// каналы таймера ШИМ STM32
  pwm_gen::PwmGenerator generator_; /// генератор значений ШИМ
  bool is_active_ = false; /// текущий статус генерации сигнала
};

/**
  * @brief  Класс контроллера генерации ШИМ в режиме прерывания (IT)
  */
class IT_PwmController : public PwmController {
 public:
  IT_PwmController(TIM_HandleTypeDef* timer,
                   tdPwmChannels channels,
                   pwm_gen::PwmGenerator generator,
                   TIM_HandleTypeDef* sample_timer,
                   IT_BUF_DATA_TYPE* buf_ptr,
                   uint32_t buf_size);
  virtual ~IT_PwmController();

  void Start() override;
  void Stop() override;
  void Run() override;
  void SetSignal(uint8_t signal, uint8_t param, FP_TYPE value) override;

 private:
  static void TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
  void ISR_Handler() override;
  void TaskPwmStart(void *argument);
  void UpdateBuffer();

  TIM_HandleTypeDef* const sample_timer_; /// таймер семплирования сигнала
  IT_BUF_DATA_TYPE* const buf_ptr_; /// указатель на начало буфера значений сигнала
  const uint32_t buf_size_; /// размер буфера значений сигнала
  uint32_t index_ = 0; /// текущий индекс массива значений сигнала
  osSemaphoreId_t pwm_start_;
  osSemaphoreId_t pwm_transfer_complete_;
  std::unique_ptr<RTOSTaskWrapper> task_start_; /// обёртка задачи FreeRTOS
};

/**
  * @brief  Класс контроллера генерации ШИМ в режиме прямого доступа к памяти (DMA)
  */
class DMA_PwmController : public PwmController {
 public:
  DMA_PwmController(TIM_HandleTypeDef* timer,
                    tdPwmChannels channels,
                    pwm_gen::PwmGenerator generator,
                    BufferModeTypeDef buf_mode,
                    BUF_DATA_TYPE* buf_ptr,
                    uint32_t buf_size);
  virtual ~DMA_PwmController();

  void Start() override;
  void StartBufferUpdate();
  void Stop() override;
  void Run() override;
  void SetSignal(uint8_t signal, uint8_t param, FP_TYPE value) override;
  void UpdateBufferSize(size_t new_size);
  template<typename T>
  void SetBufferReadyHandler(T* owner, void (T::*func_ptr)(PwmController* obj)) {
    buf_ready_callback_ = std::make_unique<_util::Callback<T, PwmController*>>(owner, func_ptr);
  }

 private:
//  static void DMAHalfFinishedCallback(DMA_HandleTypeDef *hdma);
//  static void DMAFinishedCallback(DMA_HandleTypeDef *hdma);
  void UpdateFrame(uint32_t size);
  void SwitchFrame();
  void SetAllDMAReady();
  void ISR_Handler() override;
  void TaskDmaStart(void *argument); /// код задачи FreeRTOS запуска таймера
  void TaskBufUpd(void *argument); /// код задачи FreeRTOS обновления буфера

  BufferModeTypeDef buf_mode_ = BUF_MODE_DOUBLE; /// режим буфера
  BUF_DATA_TYPE* const buf_ptr_; /// указатель на начало буфера значений ШИМ
  uint32_t buf_size_; /// размер буфера значений ШИМ

  uint32_t ch_buf_size_; /// размер буфера для одного канала
  BUF_DATA_TYPE* pos_ch_; /// буфер сигнала для положительной полуволны
  BUF_DATA_TYPE* neg_ch_; /// буфер сигнала для отрицательной полуволны

  osSemaphoreId_t calc_frame_sem_;
  osSemaphoreId_t calc_upd_buf_sem_;
  osSemaphoreId_t calc_buf_ready_sem_;
  osSemaphoreId_t pwm_start_;

//  bool is_buf_ready_ = false;
//  BufferModeTypeDef buf_mode_ = BUF_MODE_DOUBLE; /// режим буфера
  std::unique_ptr<RTOSTaskWrapper> task_start_; /// обёртка задачи FreeRTOS
  std::unique_ptr<RTOSTaskWrapper> task_buf_upd_; /// обёртка задачи FreeRTOS
  std::unique_ptr<_util::GenericCallback<PwmController*>> buf_ready_callback_; /// callback сигнала об успешном обновлении буфера
};

#endif // #ifndef _SIN_H_