#include "dev_mode.h"

#include <Arduino.h>
#include <nrf52.h>
#include <nrf_nvic.h>

bool dev_mode_enabled()
{
	return ((NRF_UICR->PSELRESET[0] & UICR_PSELRESET_CONNECT_Msk) == (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos));
}

void set_reset_pin_enabled(bool resetEnabled)
{
	// Storage buffers and variables to hold the UICR register content
	static uint32_t uicr_buffer[59] = {0x00000000};
	static uint32_t pselreset_0 = 0x00000000;
	static uint32_t pselreset_1 = 0x00000000;
	static uint32_t approtect = 0x00000000;
	static uint32_t nfcpins = 0x00000000;
#ifdef SOFTDEVICE_PRESENT
	do
	{
		uint8_t dummy = 0;
		uint32_t err_code = sd_nvic_critical_region_enter(&dummy);
		(void)err_code; //APP_ERROR_CHECK(err_code);
	} while (0);
#else
	__disable_irq();
#endif
	uint32_t uicr_temp[sizeof(NRF_UICR_Type)/4];
	NRF_UICR_Type *uicr_copy = (NRF_UICR_Type*)uicr_temp;
	uint32_t *uicr_dest = ((uint32_t *)NRF_UICR_BASE);
	// Read and buffer UICR register content prior to erase
	// uint32_t uicr_address = 0x10001014;
	for (int i = 0; i < sizeof(NRF_UICR_Type)/4; i++)
	{
		uicr_temp[i] = uicr_dest[i];
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
		{
		}
		// Set UICR address to the next register
		// uicr_address += 0x04;
	}
	uicr_copy->PSELRESET[0] = resetEnabled ? 21 : 0xFFFFFFFF;
	uicr_copy->PSELRESET[1] = resetEnabled ? 21 : 0xFFFFFFFF;
	// pselreset_0 = NRF_UICR->PSELRESET[0];
	// pselreset_1 = NRF_UICR->PSELRESET[1];
	// approtect = NRF_UICR->APPROTECT;
	// nfcpins = NRF_UICR->NFCPINS;
	//Modify the Bootloader start address  to correspond to the new bootloader
	// uicr_buffer[0] = 0x00078000;
	// Enable Erase mode
	NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een << NVMC_CONFIG_WEN_Pos; //0x02;
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	{
	}
	// Erase the UICR registers
	NRF_NVMC->ERASEUICR = NVMC_ERASEUICR_ERASEUICR_Erase << NVMC_ERASEUICR_ERASEUICR_Pos; //0x00000001;
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	{
	}
	// Enable WRITE mode
	NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos; //0x01;
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	{
	}
	// Write the modified UICR content back to the UICR registers

	for (int j = 0; j < sizeof(NRF_UICR_Type)/4; j++)
	{
		// Skip writing to registers that were 0xFFFFFFFF before the UICR register were erased.
		if (uicr_temp[j] != 0xFFFFFFFF)
		{
			uicr_dest[j] = uicr_temp[j];
			// Wait untill the NVMC peripheral has finished writting to the UICR register
			while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
			{
			}
		}
	}

	NRF_UICR->APPROTECT = approtect;
	NRF_UICR->NFCPINS = nfcpins;

#ifdef SOFTDEVICE_PRESENT
	uint32_t err_code = sd_nvic_critical_region_exit(0);
	(void)err_code; //APP_ERROR_CHECK(err_code);
#else
	__enable_irq();
#endif
}

void set_dev_mode_enabled_and_reboot()
{
	sd_softdevice_disable();
	set_reset_pin_enabled(true);
		//  if (((NRF_UICR->PSELRESET[0] & UICR_PSELRESET_CONNECT_Msk) != (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos)) ||
		//         ((NRF_UICR->PSELRESET[1] & UICR_PSELRESET_CONNECT_Msk) != (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos))){
	// 	NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
	// while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	// {
	// }
	// NRF_UICR->PSELRESET[0] = 21;
	// while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	// {
	// }
	// NRF_UICR->PSELRESET[1] = 21;
	// while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	// {
	// }
	// NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
	// while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	// {
	// }
	NVIC_SystemReset();
	//  }
}

void toggle_dev_mode_enabled_and_reboot()
{
	sd_softdevice_disable();
	set_reset_pin_enabled(!dev_mode_enabled());
	//  if (((NRF_UICR->PSELRESET[0] & UICR_PSELRESET_CONNECT_Msk) != (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos)) ||
	//         ((NRF_UICR->PSELRESET[1] & UICR_PSELRESET_CONNECT_Msk) != (UICR_PSELRESET_CONNECT_Connected << UICR_PSELRESET_CONNECT_Pos))){
	// 	NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
	// while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	// {
	// }
	// NRF_UICR->PSELRESET[0] = 21;
	// while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	// {
	// }
	// NRF_UICR->PSELRESET[1] = 21;
	// while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	// {
	// }
	// NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
	// while (NRF_NVMC->READY == NVMC_READY_READY_Busy)
	// {
	// }
	NVIC_SystemReset();
	//  }
}