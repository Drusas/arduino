#ifndef _QUADRUPED_FSM_H
#define _QUADRUPED_FSM_H

#include "..\hal\IServoController.h"
#include "Quadruped.h"
#include <tinyfsm.hpp>

struct ModeEvent : tinyfsm::Event {};

struct ToDisable : ModeEvent {
  IServoController *sController;
  Quadruped *q;
  ToDisable(IServoController *sc, Quadruped *quad) : sController(sc), q(quad) {}
};

struct ToEnable : ModeEvent {
  IServoController *sController;
  ToEnable(IServoController *sc) : sController(sc) {}
};

struct ToIdle : ModeEvent {};

struct ToLay : ModeEvent {
    Quadruped *q;
    ToLay(Quadruped *quad) : q(quad) {}
};

struct ToSit : ModeEvent {
    Quadruped *q;
    ToSit(Quadruped *quad) : q(quad) {}
};

struct ToStand : ModeEvent {
    Quadruped *q;
    ToStand(Quadruped *quad) : q(quad) {}
};

struct ToWalk : ModeEvent {
    Quadruped *q;
    ToWalk(Quadruped *quad) : q(quad) {}
};

class QuadrupedFsm : public tinyfsm::Fsm<QuadrupedFsm> {

public:
    
  void react(tinyfsm::Event const &) { }; /* default reaction for unhandled events */
  virtual void react(ToDisable const &event) {};
  virtual void react(ToEnable const &event) {};
  virtual void react(ToIdle const &) {};
  virtual void react(ToLay const &) {};
  virtual void react(ToSit const &) {};
  virtual void react(ToStand const &) {};
  virtual void react(ToWalk const &) {};

  virtual void entry(void) { };  /* entry actions in some states */
  virtual void exit(void)  { };  /* no exit actions at all */

  static std::string getCurrentState();
};


#endif