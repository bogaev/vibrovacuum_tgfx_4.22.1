#include <gui/vacuum_screen/VacuumView.hpp>

VacuumView::VacuumView()
{

}

void VacuumView::setupScreen()
{
  VacuumViewBase::setupScreen();
  
  int value = presenter->GetVacuum();
  updateValueArea(value, textVacuumValueBuffer, textVacuumValue, TEXTVACUUMVALUE_SIZE);
  sliderAmplitude.setValue(value);
  
  value = presenter->GetTimerMinutes();
  updateValueArea(value, textVacuumTimeValueBuffer, textVacuumTimeValue, TEXTVACUUMTIMEVALUE_SIZE);
  sliderTime.setValue(value);
}

void VacuumView::tearDownScreen()
{
  VacuumViewBase::tearDownScreen();
}

void VacuumView::OnAmplitudeChange(int value)
{
  updateValueArea(value, textVacuumValueBuffer, textVacuumValue, TEXTVACUUMVALUE_SIZE);
  presenter->SetVacuum(value);
}

void VacuumView::OnVacuumTest()
{
  bool state = buttonTest.getPressed();
  presenter->OnVacuumTest(state);
  sliderTime.setTouchable(!state);
}

void VacuumView::OnButtonBack()
{
  presenter->OnButtonBack();
}

void VacuumView::OnTimeChanged(int value)
{
  updateValueArea(value, textVacuumTimeValueBuffer, textVacuumTimeValue, TEXTVACUUMTIMEVALUE_SIZE);
  presenter->OnTimeChanged(value);
}

void VacuumView::OnPumpTimerExpired()
{
  buttonTest.setPressed(false);
  buttonTest.invalidate();
}
