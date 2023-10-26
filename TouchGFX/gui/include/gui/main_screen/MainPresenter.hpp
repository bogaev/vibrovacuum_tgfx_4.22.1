#ifndef MAINPRESENTER_HPP
#define MAINPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class MainView;

class MainPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    MainPresenter(MainView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~MainPresenter() {};

    void updateTimer(uint16_t seconds);

  bool GetVacuumCheck();
  bool GetVibrationCheck();
  bool GetElectrostimCheck();
  
  bool GetVibrationState();
  
  void OnVacuumButtonCheck(bool state);
  void OnVibrationButtonCheck(bool state);
  void OnStimulButtonCheck(bool state);
  void OnButtonStart(bool state);
  
  void OnAllTimersExpired() override;
  TimersStats GetExpiryTimeMS();

private:
    MainPresenter();

    MainView& view;
};

#endif // MAINPRESENTER_HPP
