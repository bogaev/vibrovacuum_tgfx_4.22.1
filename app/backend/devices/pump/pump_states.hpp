#ifndef _PUMP_STATES_H_
#define _PUMP_STATES_H_

#include "pump_controller.hpp"

namespace pump {

class State {
public:
  State(Controller& pump) :
    pump_(pump) {}
    virtual ~State() {}

    virtual void SetUserEvent() {}
    virtual void PressureEvent(Controller::PressureState state) {}

protected:
  Controller& pump_;
};

class StopState : public State {
public:
  StopState(Controller& pump) :
    State(pump) {}

    void SetUserEvent() override {
      if (pump_.GetUserVacuum() > 0) {
        Run();
      }
    }

    void PressureEvent(Controller::PressureState state) override {
      if (state == Controller::PressureState::BELOW_TH) {
        if (pump_.GetUserVacuum() > 0) {
          Run();
        }
      }
    }

private:
  void Run() {
    pump_.SetPwm(pump_.GetUserVacuum());
    pump_.SetState(Controller::RUN);
  }
};

class RunState : public State {
public:
  RunState(Controller& pump) :
    State(pump) {}

  void SetUserEvent() override {
    if (pump_.GetUserVacuum() == 0) {
      Stop();
    } else {
      pump_.SetPwm(pump_.GetUserVacuum());
    }
  }

  void PressureEvent(Controller::PressureState state) override {
    if(state == Controller::PressureState::ABOVE_TH) {
      Stop();
    }
  }

private:
  void Stop() {
    pump_.SetPwm(0);
    pump_.SetState(Controller::STOPPED);
  }
};

}

#endif // #ifndef _PUMP_STATES_H_