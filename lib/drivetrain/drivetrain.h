#ifndef DRIVETRAIN_H
#define DRIVETRAIN_H

#include "motor.h"

class Drivetrain {
    public:
        Drivetrain(Motor& topLeftMotor, Motor& topRightMotor, Motor& bottomRightMotor, Motor& bottomLeftMotor);
        ~Drivetrain();
        void drive(double angle_rad, double rotation, double power);
    private:
        Motor topLeftMotor;
        Motor topRightMotor;
        Motor bottomRightMotor;
        Motor bottomLeftMotor;
};

#endif