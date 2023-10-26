#include <gui/vibration_screen/VibrationView.hpp>
#include <gui/vibration_screen/VibrationPresenter.hpp>

VibrationPresenter::VibrationPresenter(VibrationView& v)
    : view(v)
{

}

void VibrationPresenter::activate()
{

}

void VibrationPresenter::deactivate()
{

}

void VibrationPresenter::SetIntensity(int value)
{
#ifndef DISCONNECTED_BACKEND
  model->GetBackend().Coil().SetIntesity(value);
#endif
}

void VibrationPresenter::SetFrequency(int value)
{
#ifndef DISCONNECTED_BACKEND
    model->GetBackend().Coil().SetFrequency(value);
#endif
}

int VibrationPresenter::GetIntensity()
{
#ifdef DISCONNECTED_BACKEND
    return 0;
#else
    return (double)model->GetBackend().Coil().GetIntesity();
#endif
}

int VibrationPresenter::GetFrequency()
{
#ifdef DISCONNECTED_BACKEND
    return 0;
#else
    return model->GetBackend().Coil().GetFrequency();
#endif
}

int VibrationPresenter::GetTimerMinutes()
{
#ifdef DISCONNECTED_BACKEND
    return 0;
#else
    return model->GetBackend().Coil().GetTimerMinutes();
#endif
}

void VibrationPresenter::OnVibrationTest(bool state)
{
#ifndef DISCONNECTED_BACKEND
  model->GetBackend().Coil().SetState(state);
//  if (state) {
//    model->GetBackend().Coil().Run();
//  } else {
//    model->GetBackend().Coil().Stop();
//  }
#endif
}

void VibrationPresenter::OnButtonBack()
{
#ifndef DISCONNECTED_BACKEND
//  model->GetBackend().Coil().SetState(false);
  model->GetBackend().Coil().Stop();
#endif
}

void VibrationPresenter::OnTimeChanged(int value)
{
#ifndef DISCONNECTED_BACKEND
    model->GetBackend().Coil().SetTimer(value);
#endif
}

void VibrationPresenter::OnCoilTimerExpired()
{
  view.OnCoilTimerExpired();
}
