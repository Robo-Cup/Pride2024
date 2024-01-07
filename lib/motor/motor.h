#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

enum class pModeState {
    UNDEFINED,
    EASY_CODING,
    BREAK_COAST_MODE,
};

class Motor {
    public:
        int enPin;
        int phPin;
        bool inBreakMode;
        static pModeState pMode;

        Motor(int enPin, int phPin, bool inBreakMode = false);
        void setup();
        void setSpeed(double speed);
        void stop();
        void setBreakMode(bool breakMode);
        static void setupPMode(pModeState pMode, int modePin);
    private:
        void setDirection(bool direction);
};

#endif 