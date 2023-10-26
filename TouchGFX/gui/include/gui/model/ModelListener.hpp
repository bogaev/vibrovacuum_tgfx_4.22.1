#ifndef MODELLISTENER_HPP
#define MODELLISTENER_HPP

#include <gui/model/Model.hpp>

class ModelListener
{
public:
    ModelListener() : model(0) {}
    
    virtual ~ModelListener() {}

    void bind(Model* m)
    {
        model = m;
    }
    
    virtual void OnCoilTimerExpired() {};
    virtual void OnPumpTimerExpired() {};
    virtual void OnMiostimTimerExpired() {};
    virtual void OnAllTimersExpired() {};

protected:
    Model* model;
};

#endif // MODELLISTENER_HPP
