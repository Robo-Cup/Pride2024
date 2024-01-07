#include "motor.h"
#include <ESP32Servo.h>

pModeState Motor::pMode = pModeState::UNDEFINED;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
//#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Set pMode
void Motor::setupPMode(pModeState pMode, int modePin) {
    pinMode(modePin, OUTPUT);
    if (pMode == pModeState::EASY_CODING) {
        digitalWrite(modePin, LOW);
    } else if (pMode == pModeState::BREAK_COAST_MODE) {
        digitalWrite(modePin, HIGH);
    } else {
        Serial.println("[Motor] Warning: pMode is undefined");
    }
    Motor::pMode = pMode;
}

// Motor constructor, takes in the pins for the motor
Motor::Motor(int getEnPin, int getPhPin, bool getInBreakMode) {
    enPin = getEnPin;
    phPin = getPhPin;
    inBreakMode = getInBreakMode;
};

// Setup motor pins
void Motor::setup() {
    // Set the pins as outputs
    pinMode(enPin, OUTPUT);
    pinMode(phPin, OUTPUT);

    // Set initial state of motor
    stop();
}

// Set motor speed [-1, 1]
void Motor::setSpeed(double speed) {
    double motorSpeed;
    if (pMode == pModeState::EASY_CODING) {
        //Serial.print(", EasyCodingMode");
        // Easy coding mode
        setDirection(speed > 0);
        motorSpeed = abs(speed) * 255.0;
        analogWrite(enPin, motorSpeed);
    } else if (pMode == pModeState::BREAK_COAST_MODE) {
        //Serial.print(", BreakCoastMode");
        // Break / Coast mode
        if (inBreakMode == false) {
            // Coast mode
            motorSpeed = abs(speed) * 255.0;
            if (speed > 0) {
                //Serial.print(", Forward");
                // Forward + Coast
                analogWrite(enPin, motorSpeed);
                digitalWrite(phPin, LOW);
            } else if (speed < 0) {
                // Reverse + Coast
                digitalWrite(enPin, LOW);
                analogWrite(phPin, motorSpeed);
            } else {
                //Serial.print(", STOP");
                // 0 + Coast
                analogWrite(enPin, 0);
                analogWrite(phPin, 0);
            }
        } else {
            // Break mode
            motorSpeed = (1.0 - abs(speed)) * 255.0;
            if (speed > 0) {
                // Forward + Break
                digitalWrite(enPin, HIGH);
                analogWrite(phPin, motorSpeed);
            } else if (speed < 0) {
                // Reverse + Break
                analogWrite(enPin, motorSpeed);
                digitalWrite(phPin, HIGH);
            } else {
                // 0 + Break
                digitalWrite(enPin, HIGH);
                digitalWrite(phPin, HIGH);
            }
        }
    }
    //Serial.print(", MotorSpeed: ");
    //Serial.println(motorSpeed);
}

// Stop motor
void Motor::stop() {
    if (pMode == pModeState::EASY_CODING) {
        // Easy coding mode
        digitalWrite(enPin, LOW);
        digitalWrite(phPin, LOW);
    } else if (pMode == pModeState::BREAK_COAST_MODE) {
        // Break / Coast mode
        if (inBreakMode == false) {
            // Coast mode
            digitalWrite(enPin, LOW);
            digitalWrite(phPin, LOW);
        } else {
            // Break mode
            digitalWrite(enPin, HIGH);
            digitalWrite(phPin, HIGH);
        }
    }
}

// Set motor break mode (true = break, false = coast)
void Motor::setBreakMode(bool breakMode) {
    if (pMode == pModeState::EASY_CODING) {
        Serial.println("[Motor] Warning: Pmode is on easy mode so break mode is not set");
    }
    inBreakMode = breakMode;
}

// Set motor direction (true = clockwise, false = counter clockwise)
void Motor::setDirection(bool direction) {
    if (pMode == pModeState::BREAK_COAST_MODE) {
        Serial.println("[Motor] Warning: Pmode is on break/coast mode yet setDirection for easy coding is called");
        return;
    }
    if (direction) {
        digitalWrite(phPin, HIGH);  // Set direction
    } else {
        digitalWrite(phPin, LOW);  // Set direction
    }
}