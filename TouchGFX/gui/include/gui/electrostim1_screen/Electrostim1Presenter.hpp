#ifndef ELECTROSTIM1PRESENTER_HPP
#define ELECTROSTIM1PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>
//#include "common_/common.h"

#include <utility>

using namespace touchgfx;

class Electrostim1View;

class Electrostim1Presenter : public touchgfx::Presenter, public ModelListener
{
public:
    Electrostim1Presenter(Electrostim1View& v);

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

    virtual ~Electrostim1Presenter() {};

    void SetPattern(int value);
    int GetPattern();

    void SetModFreq(int value);
    int GetModFreq();

    void SetModDepth(int value);
    int GetModDepth();

    void SetModRatio(int value);
    int GetModRatio();

    void OnElectrostimTest(bool state);
    void OnButtonBack();
    void OnTimeChanged(int value);

private:
    Electrostim1Presenter();

    Electrostim1View& view;
};

#endif // ELECTROSTIM1PRESENTER_HPP
