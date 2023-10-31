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
#ifndef NO_COIL
  model->GetBackend().Coil().SetIntesity(value);
#endif
}

void VibrationPresenter::SetFrequency(int value)
{
#ifndef NO_COIL
    model->GetBackend().Coil().SetFrequency(value);
#endif
}

int VibrationPresenter::GetIntensity()
{
#ifdef NO_COIL
    return 0;
#else
    return (double)model->GetBackend().Coil().GetIntesity();
#endif
}

int VibrationPresenter::GetFrequency()
{
#ifdef NO_COIL
    return 0;
#else
    return model->GetBackend().Coil().GetFrequency();
#endif
}

int VibrationPresenter::GetTimerMinutes()
{
#ifdef NO_COIL
    return 0;
#else
    return model->GetBackend().Coil().GetTimerMinutes();
#endif
}

void VibrationPresenter::OnVibrationTest(bool state)
{
#ifndef NO_COIL
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
#ifndef NO_COIL
//  model->GetBackend().Coil().SetState(false);
  model->GetBackend().Coil().Stop();
#endif
}

void VibrationPresenter::OnTimeChanged(int value)
{
#ifndef NO_COIL
    model->GetBackend().Coil().SetTimer(value);
#endif
}

void VibrationPresenter::OnCoilTimerExpired()
{
#ifndef NO_COIL
  view.OnCoilTimerExpired();
#endif
}
