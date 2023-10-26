#include "adc_controller.hpp"

namespace adc {

// ADC BaseController class =============================================
  
BaseController::BaseController(InitSettings settings) :
  hadc_(settings.hadc)
{
  adc_interrupt_manager.SetListener(this);
}

BaseController::~BaseController() {
  Stop();
}

void BaseController::Stop() {
  HAL_ADC_Stop(hadc_);
  HAL_ADC_Stop_IT(hadc_);
  HAL_ADC_Stop_DMA(hadc_);
}

bool BaseController::ISR_Handler(ADC_HandleTypeDef* hadc) {
  if(hadc_ == hadc) {
    if(measure_end_callback_->isValid())
      measure_end_callback_->execute();
    return true;
  }
  return false;
}

// ADC IT_Controller class =============================================

IT_Controller::IT_Controller(InitSettings settings) :
  BaseController(settings)
{}

void IT_Controller::Start() {
  HAL_ADC_Start_IT(hadc_);
}

uint16_t IT_Controller::GetValue() {
  return HAL_ADC_GetValue(hadc_);
}

// ADC DMAController =============================================

DMAController::DMAController(InitSettings settings) : 
  BaseController(settings),
  buffer_(settings.buffer_size, 0) {
}

void DMAController::Start() {
  HAL_ADC_Start_DMA(hadc_, (uint32_t*) buffer_.data(), buffer_.size());
}

uint16_t DMAController::GetValue() {
  return (uint16_t)((double)std::accumulate(buffer_.cbegin(), buffer_.cend(), 0) 
          / (double)buffer_.size());
}

} // namespace adc
