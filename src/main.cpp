#include <Arduino.h>
#include "constants.hpp"

#include "motor.h"
#include "drivetrain.h"
#include "gyro.h"
#include "bluetooth_dashboard.h"

Motor frontLeftMotor(DrivetrainConstants::bluePin, DrivetrainConstants::greenPin);
Motor frontRightMotor(DrivetrainConstants::bluePin2, DrivetrainConstants::greenPin2);
Motor backRightMotor(DrivetrainConstants::bluePin3, DrivetrainConstants::greenPin3);
Motor backLeftMotor(DrivetrainConstants::bluePin4, DrivetrainConstants::greenPin4);

Drivetrain drivetrain(frontLeftMotor, frontRightMotor, backRightMotor, backLeftMotor);

void setup() {
  Serial.begin(115200);
  
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(LED_BUILTIN, OUTPUT);

  Gyro::setup();
  BluetoothPanel::init();
}

bool isOn = false;

void loop() {
  // Gyro::getData();
  if (isOn) {
    digitalWrite(LED_BUILTIN, HIGH);
    isOn = false;
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    isOn = true;
  }
  BluetoothPanel::update();
  delay(5000);
}
