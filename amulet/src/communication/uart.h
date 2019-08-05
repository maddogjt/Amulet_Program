#pragma once

#include <Arduino.h>

class BLEService;

void uart_setup();
BLEService &uart_get_service();
Stream &uart_stream();