#ifndef _PWM_CONTROLLER_MOCK_H_
#define _PWM_CONTROLLER_MOCK_H_

#include "pwm_controller.h"
#include <algorithm>
#include <array>

struct InitChecker {
  bool start_buffer_update_method_done  = false;
  bool run_method_done                  = false;
  bool is_buffer_filled                 = false;
};

class Mock_DMA_PwmController : public DMA_PwmController {  
 public:
  Mock_DMA_PwmController(TIM_HandleTypeDef* timer,
                          tdPwmChannels channels,
                          pwm_gen::PwmGenerator generator,
                          BufferModeTypeDef buf_mode,
                          BUF_DATA_TYPE* buf_ptr,
                          uint32_t buf_size,
                          InitChecker* init_checker)
    : DMA_PwmController(timer, channels, std::move(generator),
                        buf_mode, buf_ptr, buf_size)
    , init_checker_(init_checker) 
  {}
    
  virtual ~Mock_DMA_PwmController() {}
  
  void StartBufferUpdate() override {
    if (buf_mode_ == BUF_MODE_DOUBLE) {
      SwitchFrame();
      UpdateFrame(ch_buf_size_ / 2);
    } else {
      UpdateFrame(ch_buf_size_);
    }
    if (buf_ready_callback_ && buf_ready_callback_->isValid()) {
      buf_ready_callback_->execute(this);
    } else {
      init_checker_->start_buffer_update_method_done = true;
      Run();
    }
  }
  
  void Run() override {
    CHECK_EQUAL(true, init_checker_->start_buffer_update_method_done);
    DMA_PwmController::Run();
        
    init_checker_->run_method_done = true;
  }
  
  void ISR_Handler() override {
    DMA_PwmController::ISR_Handler();
  }
  
  void IsBufferFilledProperly() {
    for (int i = 0; i < ch_buf_size_; ++i) {
      if (pos_ch_[i] != 0xDEAD) {
        init_checker_->is_buffer_filled = false;
        return;
      }
    }
    for (int i = 0; i < ch_buf_size_; ++i) {
      if (neg_ch_[i] != 0xDEAD) {
        init_checker_->is_buffer_filled = false;
        return;
      }
    }
    init_checker_->is_buffer_filled = true;
    return;
  }
  
 private:
  InitChecker* init_checker_;
};

#endif // #ifndef _PWM_CONTROLLER_MOCK_H_