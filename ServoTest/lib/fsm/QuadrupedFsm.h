#ifndef _QUADRUPED_FSM_H
#define _QUADRUPED_FSM_H

#include "..\hal\IServoController.h"
#include <tinyfsm.hpp>

struct ModeEvent : tinyfsm::Event {
  IServoController *sController;
};
struct ToDisable : ModeEvent {
  IServoController *sController;
  ToDisable(IServoController *sc) : sController(sc) {
    // sController = sc;
  }
};
struct ToEnable : tinyfsm::Event {
  IServoController *sController;
  ToEnable(IServoController *sc) : sController(sc) {
    // sController = sc;
  }
};
struct ToIdle : ModeEvent {};
struct ToSit : ModeEvent {};
struct ToStand : ModeEvent {};
struct ToWalk : ModeEvent {};

class QuadrupedFsm : public tinyfsm::Fsm<QuadrupedFsm> {

public:
    
  void react(tinyfsm::Event const &) { }; /* default reaction for unhandled events */
  virtual void react(ToDisable const &event) {};
  virtual void react(ToEnable const &event) {};
  virtual void react(ToIdle const &) {};
  virtual void react(ToSit const &) {};
  virtual void react(ToStand const &) {};
  virtual void react(ToWalk const &) {};

  virtual void entry(void) { };  /* entry actions in some states */
  void exit(void)  { };  /* no exit actions at all */
};


#endif