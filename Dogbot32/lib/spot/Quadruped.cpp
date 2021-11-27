#include <iostream>
#include "Quadruped.h"

Quadruped::Quadruped() :
controller(nullptr),
legRF(nullptr),
legLF(nullptr),
legRR(nullptr),
legLR(nullptr),
gaitTask(nullptr),
cmd(nullptr)
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
        float (*stance)[4] = controller->spotConfig->standStance;
        legRF->moveToXYZ(stance[0][0], stance[1][0], stance[2][0]);
        legLF->moveToXYZ(stance[0][1], stance[1][1], stance[2][1]);
        legRR->moveToXYZ(stance[0][2], stance[1][2], stance[2][2]);
        legLR->moveToXYZ(stance[0][3], stance[1][3], stance[2][3]);
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

void Quadruped::configure(ITask *task, Controller *cntlr, Command *command, ILegController *RF, ILegController *LF, ILegController *RR, ILegController *LR) {
    gaitTask = task;
    controller = cntlr;
    cmd = command;
    legRF = RF;
    legLF = LF;
    legRR = RR;
    legLR = LR;
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

// void Quadruped::updateLegXYZPositions() {
//     std::cout << "Quadruped::updateLegXYZPositions" << std::endl;
//     if ((legRF != nullptr) && (gaitTask != nullptr)) {
//         legRF->getXYZPosition(&Quadruped::LegPositions[0]);
//         legRF->getXYZPosition(&Quadruped::LegPositions[3]);
//         legRF->getXYZPosition(&Quadruped::LegPositions[6]);
//         legRF->getXYZPosition(&Quadruped::LegPositions[9]);
//     }
// }

// void Quadruped::updateLegJointAngles() {
//     std::cout << "Quadruped::updateLegJointAngles" << std::endl;
//     if ((legRF != nullptr) && (gaitTask != nullptr)) {
//         legRF->getJointAngles(&Quadruped::LegPositions);
//         legRF->getJointAngles(&Quadruped::LegPositions);
//         legRF->getJointAngles(&Quadruped::LegPositions);
//         legRF->getJointAngles(&Quadruped::LegPositions);
//     }
// }