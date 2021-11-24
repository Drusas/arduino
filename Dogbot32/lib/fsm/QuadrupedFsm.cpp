#include "Disabled.h"
#include "Enabled.h"
#include "Idle.h"
#include "Laying.h"
#include "Sitting.h"
#include "Standing.h"
#include "Walking.h"
#include "QuadrupedFsm.h"

std::string QuadrupedFsm::getCurrentState() {
      if(is_in_state<Disabled>()) {
          return "Disabled";
      }
      else if (is_in_state<Enabled>()) {
          return "Enabled";
      }
      else if (is_in_state<Idle>()) {
          return "Idle";
      }
      else if (is_in_state<Laying>()) {
          return "Laying";
      }
      else if (is_in_state<Sitting>()) {
          return "Sitting";
      }
      else if (is_in_state<Standing>()) {
          return "Standing";
      }
      else if (is_in_state<Walking>()) {
          return "Walking";
      }
      else {
          return "UnknownState";
      }
  }