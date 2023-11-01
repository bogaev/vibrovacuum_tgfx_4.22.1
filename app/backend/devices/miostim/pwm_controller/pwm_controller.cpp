#include "pwm_controller.h"

#include <functional>
#include <assert.h>

#include "utility\shared_objects.h"

#include "tim.h"

BUF_DATA_TYPE duty_cycle_monitor = 0;

PwmController::PwmController(TIM_HandleTypeDef* timer,
                             tdPwmChannels channels,
                             pwm_gen::PwmGenerator generator)
  : timer_(timer)
  , generator_(std::move(generator))
  , channels_({channels.pos_halfwave_channel, channels.neg_halfwave_channel}) {

  SharedObjects::uart_received.RegisterObserver(this);
}

PwmController::~PwmController() {
  SharedObjects::uart_received.RemoveObserver(this);
};

void PwmController::SetSignal(uint8_t signal, uint8_t param, FP_TYPE value) {
  generator_.SetSignal(signal, param, value);
}

/**
  * @brief  Обработчик события обновления в классе "Издателя"
  */
void PwmController::Update(tdUartData msg) {
  generator_.SetSignal(msg.signal, msg.param, msg.value);
}

/**
  * @brief  Обработчик события "вывод половины значений буфера сигнала завершен"
  */
void PwmController::PWM_PulseHalfFinishedCallback(TIM_HandleTypeDef *htim) {
  GetTimerInstance(htim)->ISR_Handler();
}

/**
  * @brief  Обработчик события "вывод всех значений буфера сигнала завершен"
  */
void PwmController::PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  GetTimerInstance(htim)->ISR_Handler();
}

/**
  * @brief  Привязка экземпляра класса контроллера (this) к хендлеру таймера (timer_)
  */
PwmController* PwmController::htim_to_inst[9] = {0};
void PwmController::BindTimerToInstance(TIM_HandleTypeDef* htim, PwmController* ptr) {
  if (htim->Instance == TIM1) {
    htim_to_inst[0] = ptr;
  } else if (htim->Instance == TIM2) {
    htim_to_inst[1] = ptr;
  } else if (htim->Instance == TIM3) {
    htim_to_inst[2] = ptr;
  } else if (htim->Instance == TIM4) {
    htim_to_inst[3] = ptr;
  } else if (htim->Instance == TIM5) {
    htim_to_inst[4] = ptr;
  } else if (htim->Instance == TIM8) {
    if (ptr->channels_.pos_halfwave_channel == TIM_CHANNEL_1) {
      htim_to_inst[5] = ptr;
    } else {
      htim_to_inst[6] = ptr;
    }
  } else if (htim->Instance == TIM13) {
    htim_to_inst[7] = ptr;
  } else if (htim->Instance == TIM14) {
    htim_to_inst[8] = ptr;
  }
}

/**
  * @brief  Возвращает экземпляр класса (для обработчика прерывания)
  */
PwmController* PwmController::GetTimerInstance(TIM_HandleTypeDef* htim) {
  if (htim->Instance == TIM1) {
    return htim_to_inst[0];
  } else if (htim->Instance == TIM2) {
    return htim_to_inst[1];
  } else if (htim->Instance == TIM3) {
    return htim_to_inst[2];
  } else if (htim->Instance == TIM4) {
    return htim_to_inst[3];
  } else if (htim->Instance == TIM5) {
    return htim_to_inst[4];
  } else if (htim->Instance == TIM8) {
    auto ch = HAL_TIM_GetActiveChannel(htim);
    if (ch == HAL_TIM_ACTIVE_CHANNEL_1 || ch == HAL_TIM_ACTIVE_CHANNEL_2) {
      return htim_to_inst[5];
    } else {
      return htim_to_inst[6];
    }
  } else if (htim->Instance == TIM13) {
    return htim_to_inst[7];
  } else if (htim->Instance == TIM14) {
    return htim_to_inst[8];
  }
  return htim_to_inst[0];
}

/**
  * @brief  Возвращает текущий активный канал таймера
  */
HAL_TIM_ActiveChannel PwmController::GetActiveChannel(uint32_t tim_ch) {
  if (tim_ch == TIM_CHANNEL_1) {
    return HAL_TIM_ACTIVE_CHANNEL_1;
  } else if (tim_ch == TIM_CHANNEL_2) {
    return HAL_TIM_ACTIVE_CHANNEL_2;
  } else if (tim_ch == TIM_CHANNEL_3) {
    return HAL_TIM_ACTIVE_CHANNEL_3;
  } else if (tim_ch == TIM_CHANNEL_4) {
    return HAL_TIM_ACTIVE_CHANNEL_4;
  }
  return HAL_TIM_ACTIVE_CHANNEL_1;
}

// class IT_PwmController ============================================================

IT_PwmController::IT_PwmController(TIM_HandleTypeDef* timer,
                                   tdPwmChannels channels,
                                   pwm_gen::PwmGenerator generator,
                                   TIM_HandleTypeDef* sample_timer,
                                   IT_BUF_DATA_TYPE* buf_ptr,
                                   uint32_t buf_size)
  : PwmController(timer, channels, std::move(generator))
  , sample_timer_(sample_timer)
  , buf_ptr_(buf_ptr)
  , buf_size_(buf_size)
  , pwm_start_(osSemaphoreNew(1U, 1U, NULL))
  , pwm_transfer_complete_(osSemaphoreNew(1U, 0U, NULL))
{
  task_start_ =
    std::make_unique<RTOSTaskWrapper>(
        "start", RTOS_TASK_STACK_SIZE, osPriorityHigh,
         std::function<void(void*)>(std::bind(&IT_PwmController::TaskPwmStart, this, std::placeholders::_1)));

  HAL_TIM_RegisterCallback(sample_timer_, HAL_TIM_PERIOD_ELAPSED_CB_ID, TIM_PeriodElapsedCallback);
  BindTimerToInstance(sample_timer_, this);
}

IT_PwmController::~IT_PwmController() {
  Stop();
}

void IT_PwmController::SetSignal(uint8_t signal, uint8_t param, FP_TYPE value) {
  PwmController::SetSignal(signal, param, value);
}

void IT_PwmController::Start() {
  UpdateBuffer();
  task_start_->Create();
}

void IT_PwmController::Stop() {
  task_start_->Delete();
  HAL_TIM_PWM_Stop_IT(timer_, channels_.neg_halfwave_channel);
  HAL_TIM_PWM_Stop_IT(timer_, channels_.pos_halfwave_channel);
  HAL_TIM_Base_Stop_IT(sample_timer_);
}

void IT_PwmController::Run() {
  auto dc = buf_ptr_[index_++];
  duty_cycle_monitor = dc;
  if(dc < 0) { // TODO убрать зависимость от знака числа?
    __HAL_TIM_SET_COMPARE(timer_, channels_.pos_halfwave_channel, 0);
    __HAL_TIM_SET_COMPARE(timer_, channels_.neg_halfwave_channel, -dc);
  }
  else {
    __HAL_TIM_SET_COMPARE(timer_, channels_.neg_halfwave_channel, 0);
    __HAL_TIM_SET_COMPARE(timer_, channels_.pos_halfwave_channel, dc);
  }
  index_ = (index_ >= buf_size_ ? 0 : index_);
}

void IT_PwmController::TaskPwmStart(void *argument) {
  for(;;) {
    if (osSemaphoreAcquire( pwm_start_, osWaitForever ) == osOK) {
      HAL_TIM_Base_Start_IT(sample_timer_);
      HAL_TIM_PWM_Start_IT(timer_, channels_.pos_halfwave_channel);
      HAL_TIM_PWM_Start_IT(timer_, channels_.neg_halfwave_channel);
      osSemaphoreRelease(pwm_transfer_complete_);
    }
  }
}

void IT_PwmController::ISR_Handler() {
  Run();
}

void IT_PwmController::TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  GetTimerInstance(htim)->ISR_Handler();
}

void IT_PwmController::UpdateBuffer() {
  for (size_t i = 0; i < buf_size_; ++i) {
    if (generator_.IsNegHalfwave()) {
      buf_ptr_[i] = (-1) * (IT_BUF_DATA_TYPE) generator_.GetValue();
    } else {
      buf_ptr_[i] = (IT_BUF_DATA_TYPE) generator_.GetValue();
    }
  }
}

// class DMA_PwmController ============================================================

DMA_PwmController::DMA_PwmController(TIM_HandleTypeDef* timer,
                                     tdPwmChannels channels,
                                     pwm_gen::PwmGenerator generator,
                                     BufferModeTypeDef buf_mode,
                                     BUF_DATA_TYPE* buf_ptr,
                                     uint32_t buf_size)
  : PwmController(timer, channels, std::move(generator))
  , buf_mode_(buf_mode)
  , buf_ptr_(buf_ptr)
  , buf_size_(buf_size)
  , ch_buf_size_(buf_size_/2)
  , pos_ch_(buf_ptr_)
  , neg_ch_(pos_ch_ + ch_buf_size_)
  , calc_frame_sem_(osSemaphoreNew(1, 0U, NULL))
  , calc_upd_buf_sem_(osSemaphoreNew(1, 0U, NULL))
  , calc_buf_ready_sem_(osSemaphoreNew(1, 0U, NULL))
  , pwm_start_(osSemaphoreNew(1, 0U, NULL))
{
  assert(buf_ptr_);
  assert(buf_size_);
  assert(ch_buf_size_);
  assert(pos_ch_);
  assert(neg_ch_);
  assert(calc_upd_buf_sem_);
  assert(calc_buf_ready_sem_);
  assert(pwm_start_);

  task_start_ =
    std::make_unique<RTOSTaskWrapper>("start", RTOS_TASK_STACK_SIZE, osPriorityHigh,
                                      std::function<void(void*)>(std::bind(&DMA_PwmController::TaskDmaStart, this, std::placeholders::_1)));

  task_buf_upd_ =
    std::make_unique<RTOSTaskWrapper>("buf_upd", RTOS_TASK_STACK_SIZE, osPriorityNormal,
                                      std::function<void(void*)>(std::bind(&DMA_PwmController::TaskBufUpd, this, std::placeholders::_1)));

#ifndef USE_SDRAM
  HAL_TIM_RegisterCallback(timer_, HAL_TIM_PWM_PULSE_FINISHED_HALF_CB_ID, PWM_PulseHalfFinishedCallback);
  HAL_TIM_RegisterCallback(timer_, HAL_TIM_PWM_PULSE_FINISHED_CB_ID, PWM_PulseFinishedCallback);
#endif
  //	HAL_DMA_RegisterCallback(timer_->hdma[TIM_DMA_ID_CC1], HAL_DMA_XFER_HALFCPLT_CB_ID, DMAHalfFinishedCallback);
  //	HAL_DMA_RegisterCallback(timer_->hdma[TIM_DMA_ID_CC1], HAL_DMA_XFER_CPLT_CB_ID, DMAFinishedCallback);

  BindTimerToInstance(timer_, this);
  task_start_->Create();
  task_buf_upd_->Create();
}

DMA_PwmController::~DMA_PwmController() {
  Stop();
  task_buf_upd_->Delete();
  task_start_->Delete();
}

void DMA_PwmController::SetSignal(uint8_t signal, uint8_t param, FP_TYPE value) {
  PwmController::SetSignal(signal, param, value);
}

void DMA_PwmController::UpdateBufferSize(size_t new_size) {
  buf_size_ = new_size;
  ch_buf_size_ = buf_size_ / 2;
}

void DMA_PwmController::StartBufferUpdate() {
  osSemaphoreRelease(calc_upd_buf_sem_);
}

void DMA_PwmController::Start() {
  StartBufferUpdate();
}

void DMA_PwmController::Stop() {
  HAL_TIM_PWM_Stop_DMA(timer_, channels_.pos_halfwave_channel);
  HAL_TIM_PWM_Stop_DMA(timer_, channels_.neg_halfwave_channel);
  SetAllDMAReady();
}

void DMA_PwmController::Run() {
  HAL_TIM_PWM_Start_DMA(timer_, channels_.pos_halfwave_channel, (uint32_t*)pos_ch_, ch_buf_size_);
  HAL_TIM_PWM_Start_DMA(timer_, channels_.neg_halfwave_channel, (uint32_t*)neg_ch_, ch_buf_size_);
}

/**
  * @brief  Метод, содержащий код задачи FreeRTOS которая запускает ШИМ
  */
void DMA_PwmController::TaskDmaStart(void *argument) {
  for(;;) {
    if ( osSemaphoreAcquire( pwm_start_, osWaitForever ) == osOK ) {
      HAL_TIM_PWM_Start_DMA(timer_, channels_.pos_halfwave_channel, (uint32_t*)pos_ch_, ch_buf_size_);
      HAL_TIM_PWM_Start_DMA(timer_, channels_.neg_halfwave_channel, (uint32_t*)neg_ch_, ch_buf_size_);
    }
  }
}

/**
  * @brief  Метод, содержащий код задачи FreeRTOS которая обновляет буфер значений
  */
void DMA_PwmController::TaskBufUpd(void *argument) {
  for(;;) {
    if (osSemaphoreAcquire(calc_upd_buf_sem_, osWaitForever) == osOK) {
      if (buf_mode_ == BUF_MODE_DOUBLE) {
        SwitchFrame();
        UpdateFrame(ch_buf_size_ / 2);
      } else {
        UpdateFrame(ch_buf_size_);
      }
      if(buf_ready_callback_ && buf_ready_callback_->isValid()) {
        buf_ready_callback_->execute(this);
      } else {
        Run();
      }
    }
  }
}

/**
  * @brief  Обработчик прерывания таймера
  */
void DMA_PwmController::ISR_Handler() {
  if (HAL_TIM_GetActiveChannel(timer_) == GetActiveChannel(channels_.pos_halfwave_channel)) {
    SwitchFrame();
    UpdateFrame(ch_buf_size_/2);
    //		osSemaphoreRelease(calc_frame_sem_);
  }
}

/**
  * @brief  Обновление буфера значений ШИМ
  */
//__IO uint32_t cycles_num_debug = 0;
void DMA_PwmController::UpdateFrame(uint32_t size) {
  for (size_t i = 0; i < size; ++i) {
//    StartCyclesCounter();
//    static BUF_DATA_TYPE cnt = 0;
//    cnt = (cnt > timer_->Init.Period ? 0 : cnt);
//    BUF_DATA_TYPE dc = cnt;
//    cnt += 1;
//    duty_cycle_monitor = cnt;
//    pos_ch_[i] = dc;
//    neg_ch_[i] = 0;
    BUF_DATA_TYPE dc = (BUF_DATA_TYPE) generator_.GetValue();
//    cycles_num_debug = GetCyclesCounter();
//    StopCyclesCounter();

    if (generator_.IsNegHalfwave()) {
      pos_ch_[i] = 0;
      neg_ch_[i] = dc;
    } else {
      pos_ch_[i] = dc;
      neg_ch_[i] = 0;
    }
  }
}

/**
  * @brief  Переключение адреса начала буфера (для двойной буферизации)
  */
void DMA_PwmController::SwitchFrame() {
  if (pos_ch_ == buf_ptr_) {
    pos_ch_ = pos_ch_ + ch_buf_size_ / 2;
    neg_ch_ = neg_ch_ + ch_buf_size_ / 2;
  } else {
    pos_ch_ = buf_ptr_;
    neg_ch_ = pos_ch_ + ch_buf_size_;
  }
}

/**
  * @brief  Сброс состояния DMA после остановки DMA
  */
void DMA_PwmController::SetAllDMAReady() {
  for (int i = 0; i < 7; ++i) {
    timer_->hdma[i]->State = HAL_DMA_STATE_READY;
    __HAL_UNLOCK(timer_->hdma[i]);
  }
}
