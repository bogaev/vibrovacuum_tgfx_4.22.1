#include <gui/electrostim1_screen/Electrostim1View.hpp>
#include <gui/electrostim1_screen/Electrostim1Presenter.hpp>

Electrostim1Presenter::Electrostim1Presenter(Electrostim1View& v)
    : view(v)
{

}

void Electrostim1Presenter::activate()
{

}

void Electrostim1Presenter::deactivate()
{

}

void Electrostim1Presenter::SetPattern(int value)
{
#ifndef DISCONNECTED_BACKEND
  model->GetBackend().Miostim().SetPattern(static_cast<miostim::TreatPatternsTypeDef>(value));
#endif
}

int Electrostim1Presenter::GetPattern()
{
#ifndef DISCONNECTED_BACKEND
  return model->GetBackend().Miostim().GetPattern();
#endif
}

void Electrostim1Presenter::SetModFreq(int value)
{
#ifndef DISCONNECTED_BACKEND
  model->GetBackend().Miostim().SetModFreq(static_cast<miostim::mod_values::enFreqMods>(value));
#endif
}

int Electrostim1Presenter::GetModFreq()
{
#ifndef DISCONNECTED_BACKEND
  return model->GetBackend().Miostim().GetModFreq();
#endif
}

void Electrostim1Presenter::SetModDepth(int value)
{
#ifndef DISCONNECTED_BACKEND
  model->GetBackend().Miostim().SetModDepth(static_cast<miostim::mod_values::enDepthModCoeffs>(value));
#endif
}

int Electrostim1Presenter::GetModDepth()
{
#ifndef DISCONNECTED_BACKEND
  return model->GetBackend().Miostim().GetModDepth();
#endif
}

void Electrostim1Presenter::SetModRatio(int value)
{
#ifndef DISCONNECTED_BACKEND
  model->GetBackend().Miostim().SetModRatio(static_cast<miostim::mod_values::enStagesRatio>(value));
#endif
}

int Electrostim1Presenter::GetModRatio()
{
#ifndef DISCONNECTED_BACKEND
  return model->GetBackend().Miostim().GetModRatio();
#endif
}

void Electrostim1Presenter::OnElectrostimTest(bool state) {
#ifndef DISCONNECTED_BACKEND
  model->GetBackend().Miostim().SetState(state);
#endif
}

void Electrostim1Presenter::OnButtonBack()
{
#ifndef DISCONNECTED_BACKEND
//  model->GetBackend().Miostim().SetState(false);
  model->GetBackend().Miostim().Stop();
#endif
}

void Electrostim1Presenter::OnTimeChanged(int value)
{
#ifndef DISCONNECTED_BACKEND
    model->GetBackend().Miostim().SetTimer(value);
#endif
}
