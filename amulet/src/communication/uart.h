#pragma once

#include <Arduino.h>

class BLEUart;

void uart_setup();
BLEUart &uart_get_service();
