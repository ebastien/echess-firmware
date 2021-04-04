#ifndef _ECHESS_STATE_H
#define _ECHESS_STATE_H

#include <variant>

#include "board.h"

class StateWaiting {
public:
  StateWaiting() {}
};

class StateMoving {
public:
  StateMoving() {}
};

class StateCastling {
public:
  StateCastling() {}
};

class StateTaking {
public:
  StateTaking() {}
};

class StateInvalid {
public:
  StateInvalid() {}
};

class Machine {
  typedef std::variant<StateInvalid, StateWaiting, StateMoving, StateCastling, StateTaking> State;
  typedef std::vector<Change> BoardInput;
  struct UIInput {};

  State s_;

public:
  Machine() {};

  void transition(const BoardInput&);
  void transition(const UIInput&);
};

#endif
