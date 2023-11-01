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
#ifndef NO_ELECTRO
  model->GetBackend().Miostim().SetPattern(static_cast<miostim::TreatPatternsTypeDef>(value));
#endif
}

int Electrostim1Presenter::GetPattern()
{
#ifndef NO_ELECTRO
  return model->GetBackend().Miostim().GetPattern();
#else
  return 0;
#endif
}

void Electrostim1Presenter::SetModFreq(int value)
{
#ifndef NO_ELECTRO
  model->GetBackend().Miostim().SetModFreq(static_cast<miostim::mod_values::enFreqMods>(value));
#endif
}

int Electrostim1Presenter::GetModFreq()
{
#ifndef NO_ELECTRO
  return model->GetBackend().Miostim().GetModFreq();
#else
  return 0;
#endif
}

void Electrostim1Presenter::SetModDepth(int value)
{
#ifndef NO_ELECTRO
  model->GetBackend().Miostim().SetModDepth(static_cast<miostim::mod_values::enDepthModCoeffs>(value));
#endif
}

int Electrostim1Presenter::GetModDepth()
{
#ifndef NO_ELECTRO
  return model->GetBackend().Miostim().GetModDepth();
#else
  return 0;
#endif
}

void Electrostim1Presenter::SetModRatio(int value)
{
#ifndef NO_ELECTRO
  model->GetBackend().Miostim().SetModRatio(static_cast<miostim::mod_values::enStagesRatio>(value));
#endif
}

int Electrostim1Presenter::GetModRatio()
{
#ifndef NO_ELECTRO
  return model->GetBackend().Miostim().GetModRatio();
#else
  return 0;
#endif
}

void Electrostim1Presenter::OnElectrostimTest(bool state) {
#ifndef NO_ELECTRO
  model->GetBackend().Miostim().SetState(state);
#endif
}

void Electrostim1Presenter::OnButtonBack()
{
#ifndef NO_ELECTRO
//  model->GetBackend().Miostim().SetState(false);
  model->GetBackend().Miostim().Stop();
#endif
}

void Electrostim1Presenter::OnTimeChanged(int value)
{
#ifndef NO_ELECTRO
    model->GetBackend().Miostim().SetTimer(value);
#endif
}
