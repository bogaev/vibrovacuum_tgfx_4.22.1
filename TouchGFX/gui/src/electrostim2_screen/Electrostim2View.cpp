#include <gui/electrostim2_screen/Electrostim2View.hpp>

Electrostim2View::Electrostim2View()
{
}

void Electrostim2View::setupScreen()
{
  Electrostim2ViewBase::setupScreen();
    
  int value = presenter->GetMinutes();
  updateValueArea(value, textElectrostimTimeValueBuffer, textElectrostimTimeValue, TEXTELECTROSTIMTIMEVALUE_SIZE);
  textElectrostimTimeValue.invalidate();
  sliderTime.setValue(value);
  sliderTime.invalidate();
}

void Electrostim2View::tearDownScreen()
{
    Electrostim2ViewBase::tearDownScreen();
}

void Electrostim2View::OnElectrostimTest() {
  bool state = buttonTest.getPressed();
  presenter->OnElectrostimTest(state);
  sliderTime.setTouchable(!state);
}

void Electrostim2View::OnTimeChanged(int value)
{
    updateValueArea(value, textElectrostimTimeValueBuffer, textElectrostimTimeValue, TEXTELECTROSTIMTIMEVALUE_SIZE);
    presenter->OnTimeChanged(value);
}

void Electrostim2View::OnMiostimTimerExpired()
{
    buttonTest.setPressed(false);
    buttonTest.invalidate();
}
