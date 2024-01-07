#include "drivetrain.h"
#include "motor.h"
#include <cmath>

Drivetrain::Drivetrain(Motor& topLeftMotor, Motor& topRightMotor, Motor& bottomRightMotor, Motor& bottomLeftMotor) :
    topLeftMotor(topLeftMotor), topRightMotor(topRightMotor), bottomRightMotor(bottomRightMotor), bottomLeftMotor(bottomLeftMotor) {
    this->topLeftMotor = topLeftMotor;
    this->topRightMotor = topRightMotor;
    this->bottomRightMotor = bottomRightMotor;
    this->bottomLeftMotor = bottomLeftMotor;
}

Drivetrain::~Drivetrain() {
    topLeftMotor.stop();
    topRightMotor.stop();
    bottomRightMotor.stop();
    bottomLeftMotor.stop();
    delete &topLeftMotor;
    delete &topRightMotor;
    delete &bottomRightMotor;
    delete &bottomLeftMotor;
}

/**
 * @param angle_rad Angle in radians [0, 6.28319]
 * @param rotation Rotation [-1, 1]
 * @param power Power [0, 1]
*/
void Drivetrain::drive(double angle_rad, double rotation, double power) {
    if (power == 0) {
        topLeftMotor.stop();
        topRightMotor.stop();
        bottomRightMotor.stop();
        bottomLeftMotor.stop();
        return;
    }

     double p1 = - (cos(angle_rad + M_PI_4)*(-fabs(rotation) + 1)) / (cos(M_PI_4));
    double p2 = - (cos(angle_rad + M_3PI_4)*(-fabs(rotation) + 1)) / (cos(M_PI_4));
    //Serial.println(", P1: " + String(p1) + ", P2: " + String(p2));
    double pFrontLeft = p2 + rotation;
    double pFrontRight = p1 - rotation;
    double pBackLeft = p1 + rotation;
    double pBackRight = p2 - rotation;

    double array[4] = { fabs(pFrontLeft), fabs(pFrontRight), fabs(pBackLeft), fabs(pBackRight)};
    double s = *std::max_element(array, array + 4)/power;
    //Serial.println(", S: " + String(s));
    double motorFrontLeft = pFrontLeft/s;
    double motorFrontRight = pFrontRight/s;
    double motorBackLeft = pBackLeft/s;
    double motorBackRight = pBackRight/s;

    //Serial.println(", MotorFrontLeft: " + String(motorFrontLeft) + ", MotorFrontRight: " + String(motorFrontRight) + ", MotorBackLeft: " + String(motorBackLeft) + ", MotorBackRight: " + String(motorBackRight));
    topLeftMotor.setSpeed(motorFrontLeft);
    topRightMotor.setSpeed(motorFrontRight);
    bottomRightMotor.setSpeed(motorBackRight);
    bottomLeftMotor.setSpeed(motorBackLeft);
}