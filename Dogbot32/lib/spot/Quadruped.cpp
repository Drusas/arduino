#include <iostream>
#include "Quadruped.h"

Quadruped::Quadruped() :
legRF(nullptr),
legLF(nullptr),
legRR(nullptr),
legLR(nullptr)
{}

Quadruped::Quadruped(ILegController *RF, ILegController *LF, ILegController *RR, ILegController *LR) :
legRF(RF),
legLF(LF),
legRR(RR),
legLR(LR)
{}

void Quadruped::stand() {
    std::cout << "Quadruped::stand" << std::endl;
    if (legLF != nullptr) {
        std::cout << "Quadruped::stand:not null" << std::endl;
        legLF->moveToXYZ(25, 60, 200);
        legLR->moveToXYZ(-25, 60, 200);
        legRF->moveToXYZ(25, 60, 200);
        legRR->moveToXYZ(-25, 60, 200);
    }
}

void Quadruped::sit() {
    std::cout << "Quadruped::sit" << std::endl;
    if (legLF != nullptr) {
        std::cout << "Quadruped::sit:not null" << std::endl;
        legRF->moveToXYZ(0, 60, 220);
        legLF->moveToXYZ(0, 60, 220);
        legRR->moveToXYZ(0, 60, 95);
        legLR->moveToXYZ(0, 60, 95);
    }
}

void Quadruped::lay() {
    std::cout << "Quadruped::lay" << std::endl;
    if (legLF != nullptr) {
        std::cout << "Quadruped::lay:not null" << std::endl;
        legRF->moveToXYZ(0, 60, 105);
        legLF->moveToXYZ(0, 60, 105);
        legRR->moveToXYZ(0, 60, 95);
        legLR->moveToXYZ(0, 60, 95);
    }
}

void Quadruped::setLegs(ILegController *RF, ILegController *LF, ILegController *RR, ILegController *LR) {
    legRF = RF;
    legLF = LF;
    legRR = RR;
    legLR = LR;
}

void Quadruped::setWalkParameters(ITask *task, Command *command) {
    gaitTask = task;
    cmd = command;
}

void Quadruped::walk() {
    std::cout << "Quadruped::walk" << std::endl;
    if ((legRF != nullptr) && (gaitTask != nullptr)) {
        std::cout << "Quadruped::walk: not null" << std::endl;
        legRF->setEnabled(true);
        legLF->setEnabled(true);
        legRR->setEnabled(true);
        legLR->setEnabled(true);
        cmd->horizontalVelocity[0] = 100.5;
        gaitTask->setEnabled(true);
    }
}

void Quadruped::stop() {
    std::cout << "Quadruped::stop" << std::endl;
    if ((legRF != nullptr) && (gaitTask != nullptr)) {
        std::cout << "Quadruped::stop:not null" << std::endl;
        cmd->horizontalVelocity[0] = 0.0;
        gaitTask->setEnabled(false);
        legRF->setEnabled(false);
        legLF->setEnabled(false);
        legRR->setEnabled(false);
        legLR->setEnabled(false);
    }
}