#ifndef BluetoothDashboard_H
#define BluetoothDashboard_H

#include <Arduino.h>
#include <BluetoothSerial.h>

struct BluetoothPanel {
  static void init();
  static void update();
};

#endif