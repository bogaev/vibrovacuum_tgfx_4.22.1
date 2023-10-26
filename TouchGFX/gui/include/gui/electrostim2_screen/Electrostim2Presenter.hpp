#ifndef ELECTROSTIM2PRESENTER_HPP
#define ELECTROSTIM2PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>
//#include "common_/common.h"

using namespace touchgfx;

class Electrostim2View;

class Electrostim2Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    Electrostim2Presenter(Electrostim2View& v);

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

    virtual ~Electrostim2Presenter() {};

    int GetMinutes();

    void OnElectrostimTest(bool state);
    void OnButtonBack();
    void OnTimeChanged(int value);

    void OnMiostimTimerExpired() override;

private:
    Electrostim2Presenter();

    Electrostim2View& view;
};
#endif // ELECTROSTIM2PRESENTER_HPP
