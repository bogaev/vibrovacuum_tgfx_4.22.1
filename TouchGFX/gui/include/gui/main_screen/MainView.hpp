#ifndef MAINVIEW_HPP
#define MAINVIEW_HPP

#include <gui_generated/main_screen/MainViewBase.hpp>
#include <gui/main_screen/MainPresenter.hpp>

class MainView : public MainViewBase
{
public:
    MainView();
    virtual ~MainView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void handleTickEvent() override;
    
    void OnVacuumButtonCheck() override;
    void OnVibrationButtonCheck() override;
    void OnStimulButtonCheck() override;
    void OnButtonStart() override;

    void OnAllTimersExpired();
  
private:
    void UpdateTimerWidget(std::pair<bool, int> time,
                           touchgfx::TextAreaWithOneWildcard& value,
                           touchgfx::Unicode::UnicodeChar* value_buf,
                           uint16_t buf_size,
                           touchgfx::Image& image);
    
    int vaco_time_ = 0;
    int vibro_time_ = 0;
    int electro_time_ = 0;
    uint8_t alpha_ = MAX_ALPHA;
};

#endif // MAINVIEW_HPP
