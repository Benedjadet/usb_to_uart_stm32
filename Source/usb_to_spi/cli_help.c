

#include "cli_help.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

void CLI_Help(char *args)
{
	if (args == NULL)
	{
		// Выводим общую справку.
		printf("Usage: INTERFACE COMMAND [ARGS]\n\r");
		printf("\n\r");
		printf("Interfaces:\n\r");
		printf("  spi <CMD>\tSerial Peripheral Interface.\n\r");
		printf("  gpio <CMD>\tGeneral-Purpose Input/Output.\n\r");
		printf("  help <IF>\tInterface help page.\n\r");
		printf("\n\r");
		return;
	}

	char *interface = strtok(args, " ");

	if (strcmp(interface, "spi") == 0)
	{
		CLI_Halp_SPI();
	}
	else if (strcmp(interface, "gpio") == 0)
	{
		CLI_Halp_GPIO();
	}
	else
	{
		printf("\n\r");
		printf("  ERROR: Unknown interface \'%s\'!\n\r", interface);
		printf("\n\r");
	}
}

void CLI_Halp_SPI(void)
{
	printf("Usage: spi COMMAND [ARGS]\n\r");
	printf("\n\r");
	printf("Commands:\n\r");
	printf("  open\t\tOpen SPI channel.\n\r");
	printf("  close\t\tClose SPI channel.\n\r");
	printf("  speed <N>\tSet SPI bitrate.\n\r");
	uint32_t sysclk = HAL_RCC_GetSysClockFreq();
	UNUSED(sysclk);
	printf("\t\t  -0 Set Bitrate 281.25 kHz.\n\r");
	printf("\t\t  -1 Set Bitrate 562.5 kHz.\n\r");
	printf("\t\t  -2 Set Bitrate 1.125 MHz.\n\r");
	printf("\t\t  -3 Set Bitrate 2.25 MHz.\n\r");
	printf("\t\t  -4 Set Bitrate 4.5 MHz.\n\r");
	printf("\t\t  -5 Set Bitrate 9.0 MHz.\n\r");
	printf("\t\t  -6 Set Bitrate 18.0 MHz.\n\r");
	printf("\t\t  -7 Set Bitrate 36.0 MHz.\n\r");

	printf("  mode <N>\tSet SPI mode.\n\r");
	printf("\t\t  -0 Set Mode CPOL=0 CPHA=0.\n\r");
	printf("\t\t  -1 Set Mode CPOL=0 CPHA=1.\n\r");
	printf("\t\t  -2 Set Mode CPOL=1 CPHA=0.\n\r");
	printf("\t\t  -3 Set Mode CPOL=1 CPHA=1.\n\r");

	printf("  tx <DATA>\tTransmit data.\n\r");
	printf("\t\tExample: spi tx 00 01 02 03\n\r");

	printf("  trx <DATA>\tTranceive data.\n\r");
	printf("\t\tExample: spi trx 00 01 02 03\n\r");

	printf("\n\r");
}

void CLI_Halp_GPIO(void)
{
	printf("Usage: gpio COMMAND [ARGS]\n\r");
	printf("\n\r");
	printf("Commands:\n\r");
	printf("  read\t\tRead GPIO state.\n\r");
	printf("  write <DATA>\tWrite GPIO state.\n\r");
	printf("  mode <MODE>\tSet GPIO mode 0 - Input, 1 - Output (Default).\n\r");
	printf("\n\r");
}