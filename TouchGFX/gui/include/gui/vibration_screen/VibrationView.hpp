#ifndef VIBRATIONVIEW_HPP
#define VIBRATIONVIEW_HPP

#include <gui_generated/vibration_screen/VibrationViewBase.hpp>
#include <gui/vibration_screen/VibrationPresenter.hpp>
#include <gui/common/GuiUtilities.hpp>

class VibrationView : public VibrationViewBase
{
public:
    VibrationView();
    virtual ~VibrationView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
#ifndef NO_COIL
    void OnIntensityChange(int value) override;
    void OnFrequencyChange(int value) override;
    void OnVibrationTest() override;
    void OnButtonBack() override;
    void OnTimeChanged(int value) override;
    void OnCoilTimerExpired();
#endif
protected:
};

#endif // VIBRATIONVIEW_HPP
