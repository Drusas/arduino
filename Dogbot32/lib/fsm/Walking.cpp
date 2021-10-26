#include "Disabled.h"
#include <iostream>
#include "Standing.h"
#include "Walking.h"

void Walking::entry() {
    std::cout << "state:walking" << std::endl;
}

void Walking::exit() {

}

void Walking::react(ToStand const &event) {
    std::cout << "state:walking:react:tostand" << std::endl;
    stopWalking(event.q);
    transit<Standing>();
}

void Walking::react(ToDisable const &event) {
    std::cout << "state:walking:react:todisable" << std::endl;
    stopWalking(event.q);
    transit<Disabled>();
}

void Walking::stopWalking(Quadruped *q) {
    if (q != nullptr) {
        q->stop();
    }
}