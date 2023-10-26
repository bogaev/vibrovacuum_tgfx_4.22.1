#ifndef VACUUMVIEW_HPP
#define VACUUMVIEW_HPP

#include <gui_generated/vacuum_screen/VacuumViewBase.hpp>
#include <gui/vacuum_screen/VacuumPresenter.hpp>
#include <gui/common/GuiUtilities.hpp>

class VacuumView : public VacuumViewBase
{
public:
    VacuumView();
    virtual ~VacuumView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void OnAmplitudeChange(int value) override;
    void OnVacuumTest() override;
    void OnButtonBack() override;
    void OnTimeChanged(int value) override;

    void OnPumpTimerExpired();
    
protected:
};

#endif // VACUUMVIEW_HPP
