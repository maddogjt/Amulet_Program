#include "uart.h"
#include "parameters.h"

#include <bluefruit.h>

static BLEUart gUartService;

void prph_bleuart_rx_callback(uint16_t conn_handle);

void uart_setup()
{
	// Configure and start the BLE Uart service
	gUartService.begin();
	gUartService.setRxCallback(prph_bleuart_rx_callback);
}

BLEUart &uart_get_service()
{
	return gUartService;
}