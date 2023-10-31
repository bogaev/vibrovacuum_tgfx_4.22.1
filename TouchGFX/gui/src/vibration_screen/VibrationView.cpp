#include <gui/vibration_screen/VibrationView.hpp>

VibrationView::VibrationView()
{

}

void VibrationView::setupScreen()
{
  VibrationViewBase::setupScreen();

#ifndef NO_COIL
  int value = presenter->GetIntensity();
  updateValueArea(value, textIntensityValueBuffer, textIntensityValue, TEXTINTENSITYVALUE_SIZE);
  sliderIntensity.setValue(value);

  value = presenter->GetFrequency();
  updateValueArea(value, textFrequencyValueBuffer, textFrequencyValue, TEXTFREQUENCYVALUE_SIZE);
  sliderFrequency.setValue(value);

  value = presenter->GetTimerMinutes();
  updateValueArea(value, textVibrationTimeValueBuffer, textVibrationTimeValue, TEXTVIBRATIONTIMEVALUE_SIZE);
  sliderTime.setValue(value);
#endif
}

void VibrationView::tearDownScreen()
{
  VibrationViewBase::tearDownScreen();
}

#ifndef NO_COIL
void VibrationView::OnIntensityChange(int value)
{
  updateValueArea(value, textIntensityValueBuffer, textIntensityValue, TEXTINTENSITYVALUE_SIZE);
  presenter->SetIntensity(value);
}

void VibrationView::OnFrequencyChange(int value)
{
  updateValueArea(value, textFrequencyValueBuffer, textFrequencyValue, TEXTFREQUENCYVALUE_SIZE);
  presenter->SetFrequency(value);
}

void VibrationView::OnVibrationTest()
{
  bool state = buttonTest.getPressed();
  presenter->OnVibrationTest(state);
  sliderTime.setTouchable(!state);
}

void VibrationView::OnButtonBack()
{
  presenter->OnButtonBack();
}

void VibrationView::OnTimeChanged(int value)
{
  updateValueArea(value, textVibrationTimeValueBuffer, textVibrationTimeValue, TEXTVIBRATIONTIMEVALUE_SIZE);
  presenter->OnTimeChanged(value);
}

void VibrationView::OnCoilTimerExpired()
{
  buttonTest.setPressed(false);
  buttonTest.invalidate();
}
#endif
