#pragma once

#include <Arduino.h>

void ble_setup(bool advertise, bool scan, bool uart = false);
void ble_loop();
void startPowerAmuletSuperpower();
