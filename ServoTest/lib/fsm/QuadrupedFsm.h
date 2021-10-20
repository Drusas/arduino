#ifndef _QUADRUPED_FSM_H
#define _QUADRUPED_FSM_H

#include <tinyfsm.hpp>

struct ModeEvent : tinyfsm::Event {};
struct Disable : ModeEvent {};
struct Enable : ModeEvent {};
struct ToIdle : ModeEvent {};
struct ToSit : ModeEvent {};
struct ToStand : ModeEvent {};
struct ToWalk : ModeEvent {};

class QuadrupedFsm : public tinyfsm::Fsm<QuadrupedFsm> {
public:
    
  void react(tinyfsm::Event const &) { }; /* default reaction for unhandled events */
  virtual void react(Disable const &event) {};
  virtual void react(Enable const &event) {};
  virtual void react(ToIdle const &) {};
  virtual void react(ToSit const &) {};
  virtual void react(ToStand const &) {};
  virtual void react(ToWalk const &) {};

  virtual void entry(void) { };  /* entry actions in some states */
  void exit(void)  { };  /* no exit actions at all */
};


#endif