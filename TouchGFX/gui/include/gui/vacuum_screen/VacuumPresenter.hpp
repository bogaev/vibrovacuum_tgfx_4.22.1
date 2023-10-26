#ifndef VACUUMPRESENTER_HPP
#define VACUUMPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class VacuumView;

class VacuumPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    VacuumPresenter(VacuumView& v);

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

    virtual ~VacuumPresenter() {};

    void SetVacuum(int value);
    int GetVacuum();
    int GetTimerMinutes();
    
    void OnVacuumTest(bool state);
    void OnButtonBack();
    void OnTimeChanged(int value);
    void OnPumpTimerExpired() override;

private:
    VacuumPresenter();

    VacuumView& view;
};

#endif // VACUUMPRESENTER_HPP
