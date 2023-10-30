#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

#include <tuple>

#ifndef DISCONNECTED_BACKEND
#else
//#include <Common/CustomTypes.hpp>
#include <gui/model/ModelListener.hpp>
#endif

Model::Model() :
  modelListener(0)
//#ifdef DISCONNECTED_BACKEND
//{}
//#else
  , backend_(Backend::GetInstance())
{
//  sdata_.resize(3);
//  backend_->AddSerializedClass(this);
  backend_->Start();
#ifndef NO_COIL
//  backend_->Coil().SetExpireTimerHandler(this, &Model::OnCoilTimerExpired);
#endif
#ifndef NO_PUMP
  backend_->Pump().SetExpireTimerHandler(this, &Model::OnPumpTimerExpired);
#endif
//  backend_->Miostim().SetExpireTimerHandler(this, &Model::OnMiostimTimerExpired);
//  Deserialize();
}
//#endif

void Model::tick()
{
    //StartBackend();
}

bool Model::GetVacuumCheck() {
//  return backend_->Pump().GetState();
  return is_vacuum_on_;
}

bool Model::GetVibrationCheck() {
//  return backend_->Coil().GetState();
  return is_vibration_on_;
}

bool Model::GetElectrostimCheck() {
//  return backend_->Miostim().GetState();
  return is_electrostim_on_;
}

void Model::SetVacuumCheck(bool state) {
//  backend_->Pump().SetPumpState(state);
  is_vacuum_on_ = state;
#ifndef NO_SERIALIZATION
  Serialize(0, (uint16_t) is_vacuum_on_);
#endif
}

void Model::SetVibrationCheck(bool state) {
//  backend_->Coil().SetState(state);
  is_vibration_on_ = state;
#ifndef NO_SERIALIZATION
  Serialize(1, (uint16_t) is_vibration_on_);
#endif
}

void Model::SetElectrostimCheck(bool state) {
//  backend_->Miostim().SetState(state);
  is_electrostim_on_ = state;
#ifndef NO_SERIALIZATION
  Serialize(2, (uint16_t) is_electrostim_on_);
#endif
}

void Model::OnCoilTimerExpired() {
#ifndef DISCONNECTED_BACKEND
  modelListener->OnCoilTimerExpired();
  is_vibration_expired_ = true;
  if (IsTotalStateExpired()) {
    modelListener->OnAllTimersExpired();
  }
#endif
}

void Model::OnPumpTimerExpired() {
#ifndef NO_PUMP
  modelListener->OnPumpTimerExpired();
  is_vacuum_expired_ = true;
  if (IsTotalStateExpired()) {
    modelListener->OnAllTimersExpired();
  }
#endif
}

void Model::OnMiostimTimerExpired()
{
#ifndef DISCONNECTED_BACKEND
  modelListener->OnMiostimTimerExpired();
  is_electrostim_expired_ = true;
  if (IsTotalStateExpired()) {
    modelListener->OnAllTimersExpired();
  }
#endif
}

void Model::OnButtonStart(bool state)
{
  if (state) {
    is_vacuum_expired_ = !GetVacuumCheck();
    #ifndef NO_PUMP
//        backend_->Pump().Run();
        backend_->Pump().SetPumpState(is_vacuum_on_);
    #endif
    is_vibration_expired_ = !GetVibrationCheck();
    #ifndef DISCONNECTED_BACKEND
//        backend_->Coil().Run();
        backend_->Coil().SetState(is_vibration_on_);
    #endif
    is_electrostim_expired_ = !GetElectrostimCheck();
    #ifndef DISCONNECTED_BACKEND
//        backend_->Miostim().Run();
        backend_->Miostim().SetState(is_electrostim_on_);
    #endif
  } else {
//    is_vacuum_on_ = false;
//    is_vibration_on_ = false;
//    is_electrostim_on_ = false;
    is_vacuum_expired_ = true;
    is_vibration_expired_ = true;
    is_electrostim_expired_ = true;
    #ifndef NO_PUMP
    backend_->Pump().Stop();
    #endif
    #ifndef DISCONNECTED_BACKEND
        backend_->Coil().Stop();
        backend_->Miostim().Stop();
//        backend_->Pump().SetPumpState(false);
//        backend_->Coil().SetState(false);
//        backend_->Miostim().SetState(false);
    #endif
    modelListener->OnAllTimersExpired();
  }
}

TimersStats Model::GetExpiryTimeMS()
{
//  #ifdef DISCONNECTED_BACKEND
//    return {};
//  #else
#ifndef NO_PUMP
      uint32_t vaco = backend_->Pump().GetTimerExpiryMS();
      return std::make_tuple(std::make_pair(is_vacuum_expired_, vaco),
                             std::make_pair(is_vibration_expired_, 0),
                             std::make_pair(is_electrostim_expired_, 0));
#else
      uint32_t vibro = backend_->Coil().GetTimerExpiryMS();
      uint32_t electro = backend_->Miostim().GetTimerExpiryMS();
#endif
}

bool Model::IsTotalStateExpired()
{
#ifndef DISCONNECTED_BACKEND
  if (is_vacuum_expired_ && is_vibration_expired_ && is_electrostim_expired_)
  {
    return true;
  }
  return false;
#endif
}

#ifndef NO_SERIALIZATION
std::vector<Model::StoredData>& Model::GetSerializedData()
{
  return sdata_;
}

void Model::Serialize(uint16_t idx, uint16_t data)
{
  sdata_[idx].value = data;
  if (flash_) {
    flash_->StoreToFlash(sdata_[idx]);
  }
}

void Model::Deserialize()
{
  if (flash_) {
    flash_->LoadFromFlash(sdata_);
  }
  is_electrostim_on_ = (bool)sdata_[2].value;
  is_vibration_on_ = (bool)sdata_[1].value;
  is_vacuum_on_ = (bool)sdata_[0].value;
}
#endif
