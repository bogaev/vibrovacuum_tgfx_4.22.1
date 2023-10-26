#ifndef VIBRATIONPRESENTER_HPP
#define VIBRATIONPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class VibrationView;

class VibrationPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    VibrationPresenter(VibrationView& v);

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

    virtual ~VibrationPresenter() {};

    void SetIntensity(int value);
    void SetFrequency(int value);

    int GetIntensity();
    int GetFrequency();
    int GetTimerMinutes();
    
    void OnVibrationTest(bool state);
    void OnButtonBack();
    void OnTimeChanged(int value);
    void OnCoilTimerExpired() override;

private:
    VibrationPresenter();

    VibrationView& view;
};

#endif // VIBRATIONPRESENTER_HPP
