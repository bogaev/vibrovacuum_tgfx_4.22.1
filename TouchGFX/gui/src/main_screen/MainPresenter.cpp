#include <gui/main_screen/MainView.hpp>
#include <gui/main_screen/MainPresenter.hpp>

MainPresenter::MainPresenter(MainView& v)
    : view(v)
{

}

void MainPresenter::activate()
{
}

void MainPresenter::deactivate()
{

}

void MainPresenter::updateTimer(uint16_t seconds)
{
  //view.setTimer(seconds);
}

bool MainPresenter::GetVacuumCheck()
{
  return model->GetVacuumCheck();
}

bool MainPresenter::GetVibrationCheck()
{
  return model->GetVibrationCheck();
}

bool MainPresenter::GetElectrostimCheck()
{
  return model->GetElectrostimCheck();
}

bool MainPresenter::GetVibrationState()
{
#ifndef DISCONNECTED_BACKEND
  return model->GetBackend().Coil().GetState();
#endif
}

void MainPresenter::OnVacuumButtonCheck(bool state)
{
  model->SetVacuumCheck(state);
}

void MainPresenter::OnVibrationButtonCheck(bool state)
{
  model->SetVibrationCheck(state);
}

void MainPresenter::OnStimulButtonCheck(bool state)
{
  model->SetElectrostimCheck(state);
}

void MainPresenter::OnButtonStart(bool state)
{
  model->OnButtonStart(state);
}

void MainPresenter::OnAllTimersExpired()
{
  view.OnAllTimersExpired();
}

TimersStats MainPresenter::GetExpiryTimeMS()
{
  return model->GetExpiryTimeMS();
}
