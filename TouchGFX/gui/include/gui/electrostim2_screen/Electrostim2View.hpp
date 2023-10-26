#ifndef ELECTROSTIM2VIEW_HPP
#define ELECTROSTIM2VIEW_HPP

#include <gui_generated/electrostim2_screen/Electrostim2ViewBase.hpp>
#include <gui/electrostim2_screen/Electrostim2Presenter.hpp>
#include <gui/common/GuiUtilities.hpp>
//#include "common_\common.h"
#include <string>

class Electrostim2View : public Electrostim2ViewBase
{
public:
    Electrostim2View();
    virtual ~Electrostim2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void OnElectrostimTest() override;
    void OnTimeChanged(int value) override;

    void OnMiostimTimerExpired();

protected:

private:
    void InitSliders();
};

#endif // ELECTROSTIM2VIEW_HPP
