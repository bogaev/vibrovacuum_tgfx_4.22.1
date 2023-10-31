#ifndef ELECTROSTIM1VIEW_HPP
#define ELECTROSTIM1VIEW_HPP

#include <gui_generated/electrostim1_screen/Electrostim1ViewBase.hpp>
#include <gui/electrostim1_screen/Electrostim1Presenter.hpp>

#include <gui/common/GuiUtilities.hpp>
//#include "common_\common.h"
#include <string>

class Electrostim1View : public Electrostim1ViewBase
{
public:
    Electrostim1View();
    virtual ~Electrostim1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

#ifndef NO_ELECTRO
    virtual void OnPatternChange(int value) override;
    virtual void OnModFreqChange(int value) override;
    virtual void OnModDepthChange(int value) override;
    virtual void OnModRatioChange(int value) override;
    void OnButtonBack() override;

protected:

private:
    void InitSliders();
#endif
};

#endif // ELECTROSTIM1VIEW_HPP
