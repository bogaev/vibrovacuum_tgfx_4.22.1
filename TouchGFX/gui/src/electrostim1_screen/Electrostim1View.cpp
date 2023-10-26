#include <gui/electrostim1_screen/Electrostim1View.hpp>

Electrostim1View::Electrostim1View()
{
}

void Electrostim1View::setupScreen()
{
  Electrostim1ViewBase::setupScreen();

  InitSliders();
}

void Electrostim1View::tearDownScreen()
{
  Electrostim1ViewBase::tearDownScreen();
}

void Electrostim1View::OnPatternChange(int value)
{
  updateValueArea(value, textPatternBuffer, textPattern, TEXTPATTERN_SIZE);

  presenter->SetPattern(value - 1);
  presenter->SetModFreq(sliderModulationFreq.getValue());
  presenter->SetModDepth(sliderModDepth.getValue());
  presenter->SetModRatio(sliderModRatio.getValue());
}

void Electrostim1View::OnModFreqChange(int value)
{
#ifndef DISCONNECTED_BACKEND
  int v = static_cast<int>(miostim::mod_values::FREQ_MOD[value]);
  updateValueArea(v, textValueFreqBuffer, textValueFreq, TEXTVALUEFREQ_SIZE);
  presenter->SetModFreq(value);
#endif
}

void Electrostim1View::OnModDepthChange(int value)
{
#ifndef DISCONNECTED_BACKEND
  int v = static_cast<int>(miostim::mod_values::DEPTH_MOD_PERCENT[value]);
  updateValueArea(v, textValueModDepthBuffer, textValueModDepth, TEXTVALUEMODDEPTH_SIZE);
  presenter->SetModDepth(value);
#endif
}

void Electrostim1View::OnModRatioChange(int value)
{
#ifndef DISCONNECTED_BACKEND
  int v = static_cast<int>(miostim::mod_values::STAGES_RATIO[value][0]);
  updateValueArea(v, textValueModRatioBuffer1, textValueModRatio, TEXTVALUEMODRATIOBUFFER1_SIZE);

  if (value == 0) {
    double v = miostim::mod_values::STAGES_RATIO[value][1];
    updateValueArea(v, textValueModRatioBuffer2, textValueModRatio, TEXTVALUEMODRATIOBUFFER2_SIZE);
  }
  else {
    int v = int(miostim::mod_values::STAGES_RATIO[value][1]);
    updateValueArea(v, textValueModRatioBuffer2, textValueModRatio, TEXTVALUEMODRATIOBUFFER2_SIZE);
  }
  presenter->SetModRatio(value);
#endif
}

void Electrostim1View::InitSliders()
{
#ifndef DISCONNECTED_BACKEND
  int value = presenter->GetPattern();
  sliderPattern.setValue(value);
  updateValueArea(1, textPatternBuffer, textPattern, TEXTPATTERN_SIZE);

  value = presenter->GetModFreq();
  updateValueArea(static_cast<int>(miostim::mod_values::FREQ_MOD[value]),
                  textValueFreqBuffer, textValueFreq, TEXTVALUEFREQ_SIZE);
  textValueFreq.invalidate();
  sliderModulationFreq.setValue(value);
  sliderModulationFreq.invalidate();

  value = presenter->GetModDepth();
  updateValueArea(static_cast<int>(miostim::mod_values::DEPTH_MOD_PERCENT[value]),
                  textValueModDepthBuffer, textValueModDepth, TEXTVALUEMODDEPTH_SIZE);
  textValueModDepth.invalidate();
  sliderModDepth.setValue(value);
  sliderModDepth.invalidate();

  int v = presenter->GetModRatio();
  int num = static_cast<int>(miostim::mod_values::STAGES_RATIO[v][0]);
  double den = static_cast<int>(miostim::mod_values::STAGES_RATIO[v][1]);
  updateValueArea(num, textValueModRatioBuffer1, textValueModRatio, TEXTVALUEMODRATIOBUFFER1_SIZE);
  updateValueArea(den, textValueModRatioBuffer2, textValueModRatio, TEXTVALUEMODRATIOBUFFER2_SIZE);
  textValueModRatio.invalidate();
  sliderModRatio.setValue(v);
  sliderModRatio.invalidate();
#endif
}

void Electrostim1View::OnButtonBack()
{
  presenter->OnButtonBack();
}
