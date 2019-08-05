#include "uart.h"
#include "parameters.h"

#include <bluefruit.h>

static BLEUart gUartService;

void configuration_handle_command(const char * str, size_t len);

static void prph_bleuart_rx_callback(uint16_t conn_handle)
{
	(void)conn_handle;

	// Wait for new data to arrive
#define MAX_BLE_UART_PACKET 200
	char str[MAX_BLE_UART_PACKET] = {0};

	int avail = gUartService.available();
	if (avail <= 0)
	{
		return;
	}

	if (avail > MAX_BLE_UART_PACKET)
	{
		Serial.println("ERROR: Buffer Overflow");
	}

	memset(str, 0, MAX_BLE_UART_PACKET);
	int len = gUartService.read(str, avail);

	// Serial.print("[Prph] RX: ");
	// Serial.println(str);

	configuration_handle_command(str, len);
}

void uart_setup()
{
	// Configure and start the BLE Uart service
	gUartService.begin();
	gUartService.setRxCallback(prph_bleuart_rx_callback);
}

BLEService &uart_get_service()
{
	return gUartService;
}

Stream &uart_stream()
{
	return gUartService;
}