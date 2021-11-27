#include <iostream>
#include "Quadruped.h"

float Quadruped::LegPositions[3][4];

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

void Quadruped::updateLegXYZPositions() {
    std::cout << "Quadruped::updateLegXYZPositions" << std::endl;
    if ((legRF != nullptr) && (gaitTask != nullptr)) {
        legRF->getXYZPosition(0, Quadruped::LegPositions);
        legLF->getXYZPosition(1, Quadruped::LegPositions);
        legRR->getXYZPosition(2, Quadruped::LegPositions);
        legLR->getXYZPosition(3, Quadruped::LegPositions);
    }
}

void Quadruped::updateLegAngles() {
    std::cout << "Quadruped::updateLegAngles" << std::endl;
    if ((legRF != nullptr) && (gaitTask != nullptr)) {
        legRF->getJointAngles(0, Quadruped::LegPositions);
        legLF->getJointAngles(1, Quadruped::LegPositions);
        legRR->getJointAngles(2, Quadruped::LegPositions);
        legLR->getJointAngles(3, Quadruped::LegPositions);
    }
}

void Quadruped::setLegAngles(uint8_t idx, float hx, float hy, float knee) {
    std::cout << "Quadruped::setLegAngles" << std::endl;
    if ((legRF != nullptr) && (gaitTask != nullptr)) {
        switch (idx) {
            case 0:
                legRF->moveToAngles(hx, hy, knee);
                break;
            case 1:
                legLF->moveToAngles(hx, hy, knee);
                break;
            case 2:
                legRR->moveToAngles(hx, hy, knee);
                break;
            case 3:
                legLR->moveToAngles(hx, hy, knee);
                break;
            default:
                std::cout << "invalid leg index: " << idx << std::endl;
        }
    }
}



void Quadruped::setLegPosition(uint8_t idx, float x, float y, float z) {
    std::cout << "Quadruped::setLegAngles" << std::endl;
    if ((legRF != nullptr) && (gaitTask != nullptr)) {
        switch (idx) {
            case 0:
                legRF->moveToXYZ(x, y, z);
                break;
            case 1:
                legLF->moveToXYZ(x, y, z);
                break;
            case 2:
                legRR->moveToXYZ(x, y, z);
                break;
            case 3:
                legLR->moveToXYZ(x, y, z);
                break;
            default:
                std::cout << "invalid leg index: " << idx << std::endl;
        }
    }
}