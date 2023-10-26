#include <gui/vacuum_screen/VacuumView.hpp>
#include <gui/vacuum_screen/VacuumPresenter.hpp>

VacuumPresenter::VacuumPresenter(VacuumView& v)
    : view(v)
{

}

void VacuumPresenter::activate()
{

}

void VacuumPresenter::deactivate()
{

}

void VacuumPresenter::SetVacuum(int value)
{
#ifndef DISCONNECTED_PUMP
  model->GetBackend().Pump().SetUserVacuum(value);
#endif
}

int VacuumPresenter::GetVacuum() {
#ifdef DISCONNECTED_PUMP
  return 0;
#else
  return (int)model->GetBackend().Pump().GetUserVacuum();
#endif
}

int VacuumPresenter::GetTimerMinutes()
{
#ifdef DISCONNECTED_PUMP
    return 0;
#else
    return model->GetBackend().Pump().GetTimerMinutes();
#endif
}

void VacuumPresenter::OnVacuumTest(bool state)
{
#ifndef DISCONNECTED_PUMP
  model->GetBackend().Pump().SetPumpState(state);
//  if (state) {
//    model->GetBackend().Pump().Run();
//  } else {
//    model->GetBackend().Pump().Stop();
//  }
#endif
}

void VacuumPresenter::OnButtonBack()
{
#ifndef DISCONNECTED_PUMP
//  model->GetBackend().Pump().SetPumpState(false);
  model->GetBackend().Pump().Stop();
#endif
}

void VacuumPresenter::OnTimeChanged(int value)
{
#ifndef DISCONNECTED_PUMP
    model->GetBackend().Pump().SetTimer(value);
#endif
}

void VacuumPresenter::OnPumpTimerExpired()
{
  view.OnPumpTimerExpired();
}
