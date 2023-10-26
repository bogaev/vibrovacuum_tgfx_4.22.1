#include <gui/electrostim2_screen/Electrostim2View.hpp>
#include <gui/electrostim2_screen/Electrostim2Presenter.hpp>

Electrostim2Presenter::Electrostim2Presenter(Electrostim2View& v)
    : view(v)
{

}

void Electrostim2Presenter::activate()
{

}

void Electrostim2Presenter::deactivate()
{

}

int Electrostim2Presenter::GetMinutes() {
#ifndef DISCONNECTED_BACKEND
  return model->GetBackend().Miostim().GetMinutes();
#endif
}

void Electrostim2Presenter::OnElectrostimTest(bool state) {
#ifndef DISCONNECTED_BACKEND
  model->GetBackend().Miostim().SetState(state);
//  if (state) {
//    model->GetBackend().Miostim().Run();
//  } else {
//    model->GetBackend().Miostim().Stop();
//  }
#endif
}

void Electrostim2Presenter::OnButtonBack()
{
#ifndef DISCONNECTED_BACKEND
//  model->GetBackend().Miostim().SetState(false);
  model->GetBackend().Miostim().Stop();
#endif
}

void Electrostim2Presenter::OnTimeChanged(int value)
{
#ifndef DISCONNECTED_BACKEND
    model->GetBackend().Miostim().SetTimer(value);
#endif
}

void Electrostim2Presenter::OnMiostimTimerExpired()
{
  view.OnMiostimTimerExpired();
}
