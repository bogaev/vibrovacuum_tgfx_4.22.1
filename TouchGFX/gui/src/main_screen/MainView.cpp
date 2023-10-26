#include <gui/main_screen/MainView.hpp>
#include <gui/common/GuiUtilities.hpp>
#include <tuple>

MainView::MainView()
{
}

void MainView::setupScreen()
{
  MainViewBase::setupScreen();
  
  buttonCheckVacuum.setPressed( presenter->GetVacuumCheck() );
  buttonCheckVibration.setPressed( presenter->GetVibrationCheck() );
  buttonCheckStim.setPressed( presenter->GetElectrostimCheck() );
  
  textExpTimeVacuum.setVisible(static_cast<bool>(vaco_time_));
  imageTimerVaco.setVisible(static_cast<bool>(vaco_time_));
  
  textExpTimeVibro.setVisible(static_cast<bool>(vibro_time_));
  imageTimerVibro.setVisible(static_cast<bool>(vibro_time_));
  
  textExpTimeElectrostim.setVisible(static_cast<bool>(electro_time_));
  imageTimerElectro.setVisible(static_cast<bool>(electro_time_));
  
//  bool state = presenter->GetVibrationState();
//  buttonVibration.setPressed(state);
//  buttonVibration.invalidate();
}

void MainView::tearDownScreen()
{
    MainViewBase::tearDownScreen();
}

void MainView::UpdateTimerWidget(std::pair<bool, int> stat,
                                 touchgfx::TextAreaWithOneWildcard& value,
                                 touchgfx::Unicode::UnicodeChar* value_buf,
                                 uint16_t buf_size,
                                 touchgfx::Image& image) {    
                                   
    updateValueArea(stat.second, value_buf, value, buf_size);
    value.setVisible(static_cast<bool>(stat.second));
    value.invalidate();
    
    image.setVisible(static_cast<bool>(stat.second));
    image.setAlpha(alpha_);
    
    alpha_ = (alpha_ > 0 ? alpha_ : MAX_ALPHA);
    if (!stat.first) {
      alpha_ -= MAX_ALPHA / 5U;
    } else {
      image.setAlpha(MAX_ALPHA);
    }
    image.invalidate();
}

void MainView::handleTickEvent()
{
  auto [vaco, vibro, electro] = presenter->GetExpiryTimeMS();
  
  vaco.second = vaco.second / 1000U / 1U;
  if (vaco_time_ != vaco.second) {
    vaco_time_ = vaco.second;
    UpdateTimerWidget(vaco, 
                      textExpTimeVacuum, textExpTimeVacuumBuffer, TEXTEXPTIMEVACUUM_SIZE,
                      imageTimerVaco);
  }
  
  vibro.second = vibro.second / 1000U / 1U;
  if (vibro_time_ != vibro.second) {
    vibro_time_ = vibro.second;
    UpdateTimerWidget(vibro, 
                      textExpTimeVibro, textExpTimeVibroBuffer, TEXTEXPTIMEVIBRO_SIZE,
                      imageTimerVibro);
  }
  
  electro.second = electro.second / 1000U / 1U;
  if (electro_time_ != electro.second) {
    electro_time_ = electro.second;
    UpdateTimerWidget(electro, 
                      textExpTimeElectrostim, textExpTimeElectrostimBuffer, TEXTEXPTIMEELECTROSTIM_SIZE,
                      imageTimerElectro);
  }
}

void MainView::OnVacuumButtonCheck()
{
  presenter->OnVacuumButtonCheck(buttonCheckVacuum.getPressed());
}

void MainView::OnVibrationButtonCheck()
{
  presenter->OnVibrationButtonCheck(buttonCheckVibration.getPressed());
}

void MainView::OnStimulButtonCheck()
{
  presenter->OnStimulButtonCheck(buttonCheckStim.getPressed());
}

void MainView::OnButtonStart()
{
  if (   buttonCheckVacuum.getPressed()
      || buttonCheckVibration.getPressed()
      || buttonCheckStim.getPressed()
      ) {
      bool state = buttonStart.getPressed();
      presenter->OnButtonStart(state);

      buttonCheckVacuum.setTouchable(!state);
      buttonCheckVibration.setTouchable(!state);
      buttonCheckStim.setTouchable(!state);
      
      buttonVacuum.setTouchable(!state);
      buttonVibration.setTouchable(!state);
      buttonElectrostim.setTouchable(!state);
  } else {
      OnAllTimersExpired();
  }
}

void MainView::OnAllTimersExpired()
{
  buttonStart.setPressed(false);
  buttonStart.invalidate();

  buttonCheckVacuum.setTouchable(true);
  buttonCheckVibration.setTouchable(true);
  buttonCheckStim.setTouchable(true);
  
  buttonVacuum.setTouchable(true);
  buttonVibration.setTouchable(true);
  buttonElectrostim.setTouchable(true);
  
  alpha_ = MAX_ALPHA;
  imageTimerVaco.setAlpha(alpha_);
  imageTimerVaco.invalidate();
  
  imageTimerVibro.setAlpha(alpha_);
  imageTimerVibro.invalidate();
  
  imageTimerElectro.setAlpha(alpha_);
  imageTimerElectro.invalidate();
}
