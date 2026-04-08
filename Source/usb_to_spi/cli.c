

#include "cli.h"
#include "stdio.h"
#include "string.h"
#include "spi1.h"
#include "stdlib.h"
// #include "cmdp_spi.h"
#include "cli_help.h"
#include "cli_if.h"

#define SPI_HANDLE hspi1

enum
{
	ESC_IDLE,
	ESC_START,
	ESC_BRACKET
} esc_state = ESC_IDLE;

static char cli_line[CLI_MAX_LINE];
static uint8_t cli_len = 0;
static uint8_t cursor = 0;

static char history[CLI_HISTORY_SIZE][CLI_MAX_LINE];
static int history_count = 0;
static int history_index = -1;

void CLI_Process(char *args);
void CLI_Interface_SPI(char *args);
void CLI_Interface_GPIO(char *args);

void SPI_Select(void);
void SPI_Unselect(void);

static uint8_t hex_to_byte(const char *str);
static int parse_bytes(char *cmd, uint8_t *buf, int max_len);

/**
 * @brief Функция обработки коммандной строки.
 * @param args
 */
void CLI_Process(char *args)
{
	// Получаем название функции.
	char *func = strtok(args, " ");

	// Получаем остальную строку.
	char *data = strtok(NULL, "");

	if (strcmp(func, "spi") == 0)
	{
		// Функция SPI.
		CLI_Interface_SPI(data);
	}
	else if (strcmp(args, "gpio") == 0)
	{
		// Функция GPIO.
		CLI_Interface_GPIO(data);
	}
	else if (strcmp(args, "help") == 0)
	{
		// Инструкция.
		CLI_Help(data);
	}
	else
	{
		// Неизвестная функция.
		// Выводим Help по функциям
		CLI_Help(NULL);
	}
}

/**
 * @brief Функция обработки ввода символа.
 * @param ch Введенный пользователем символ.
 */
void CLI_Input(uint8_t ch)
{
	static uint8_t esc_state = 0;

	if (esc_state == ESC_IDLE)
	{
		if (ch == 0x1B) // ESC
		{
			esc_state = ESC_START;
			return;
		}

		// ENTER
		if (ch == '\r' || ch == '\n')
		{
			printf("\n\r");

			cli_line[cli_len] = 0;

			if (cli_len > 0)
			{
				// сохранить в history
				strcpy(history[history_count % CLI_HISTORY_SIZE], cli_line);
				history_count++;
			}

			history_index = history_count;

			CLI_Process(cli_line);

			cli_len = 0;
			cursor = 0;

			printf("> ");
			fflush(stdout);
			return;
		}

		// BACKSPACE
		if (ch == 0x7F || ch == 0x08)
		{
			if (cursor > 0)
			{
				memmove(&cli_line[cursor - 1], &cli_line[cursor], cli_len - cursor);
				cli_len--;
				cursor--;

				printf("\b \b");
				fflush(stdout);
			}
			return;
		}

		// обычный символ
		if (cli_len < CLI_MAX_LINE - 1)
		{
			cli_line[cursor++] = ch;
			cli_len++;

			printf("%c", ch);
			fflush(stdout);
		}
	}
	else if (esc_state == ESC_START)
	{
		if (ch == '[')
		{
			esc_state = ESC_BRACKET;
		}
		else
		{
			esc_state = ESC_IDLE;
		}
	}
	else if (esc_state == ESC_BRACKET)
	{
		if (ch == 'A') // UP
		{
			if (history_count > 0 && history_index > 0)
			{
				history_index--;
				strcpy(cli_line, history[history_index % CLI_HISTORY_SIZE]);
				cli_len = strlen(cli_line);
				cursor = cli_len;

				printf("\r> %s\033[K", cli_line);
				fflush(stdout);
			}
		}
		else if (ch == 'B') // DOWN
		{
			if (history_index < history_count - 1)
			{
				history_index++;
				strcpy(cli_line, history[history_index % CLI_HISTORY_SIZE]);
			}
			else
			{
				history_index = history_count;
				cli_line[0] = 0;
			}

			cli_len = strlen(cli_line);
			cursor = cli_len;

			printf("\r> %s\033[K", cli_line);
			fflush(stdout);
		}

		esc_state = ESC_IDLE;
	}
}

void CLI_Header(void)
{
	printf("\n\r");
	printf("STM32 SPI CLI v1.0\n\r");
	printf("Build: %s %s\n\r", __DATE__, __TIME__);
	printf("Type 'help'\n\r");
	printf("\n\r> ");
	fflush(stdout);
}

/**
 * @brief Парсер Интерфейс SPI.
 * @param args Аргументы.
 */
void CLI_Interface_SPI(char *args)
{
	// Получаем команду.
	char *command = strtok(args, " ");

	// Получаем аргументы.
	char *data = strtok(NULL, "");

	if (!command)
	{
		CLI_Help("spi");
		return;
	}

	if (strcmp(command, "tx") == 0)
	{
		// Если нет данных.
		if (!data)
		{
			CLI_Help("spi");
			return;
		}

		uint8_t tx[64];
		uint8_t rx[64];

		memset(tx, 0, sizeof(tx));
		memset(rx, 0, sizeof(rx));

		int len = parse_bytes(data, tx, sizeof(tx));

		// SPI_Select();

		// HAL_SPI_Transmit(&SPI_HANDLE, tx, len, HAL_MAX_DELAY);

		// SPI_Unselect();

		uint32_t res = CLI_IF_SPI_Transmit(tx, len);

		if (res == CLI_IF_ERR_SPI_CLOSED)
		{
			printf("\n\r");
			printf("  ERROR: SPI port not opened!\n\r");
			printf("\n\r");
			return;
		}
		else if (res == CLI_IF_ERR_SPI_HARDWARE)
		{
			printf("\n\r");
			printf("  ERROR: SPI internal error!\n\r");
			printf("\n\r");
			return;
		}

		printf("\n\r");
		printf("  TX: ");
		for (int i = 0; i < len; i++)
			printf("%02X ", tx[i]);
		printf("\n\r");
		printf("\n\r");
		return;
	}
	else if (strcmp(command, "trx") == 0)
	{
		// Если нет данных.
		if (!data)
		{
			CLI_Help("spi");
			return;
		}

		uint8_t tx[64];
		uint8_t rx[64];

		memset(tx, 0, sizeof(tx));
		memset(rx, 0, sizeof(rx));

		int len = parse_bytes(data, tx, sizeof(tx));

		uint32_t res = CLI_IF_SPI_Tranceive(tx, rx, len);

		if (res == CLI_IF_ERR_SPI_CLOSED)
		{
			printf("\n\r");
			printf("  ERROR: SPI port not opened!\n\r");
			printf("\n\r");
			return;
		}
		else if (res == CLI_IF_ERR_SPI_HARDWARE)
		{
			printf("\n\r");
			printf("  ERROR: SPI internal error!\n\r");
			printf("\n\r");
			return;
		}

		printf("\n\r");
		printf("  TX: ");
		for (int i = 0; i < len; i++)
			printf("%02X ", tx[i]);

		printf("\n\r");
		printf("  RX: ");
		for (int i = 0; i < len; i++)
			printf("%02X ", rx[i]);

		printf("\n\r");
		printf("\n\r");
		return;
	}
	else if (strcmp(command, "open") == 0)
	{
		uint32_t res = CLI_IF_SPI_Open();
		if (res == CLI_IF_ERR_SPI_OPENED)
		{
			printf("\n\r");
			printf("  ERROR: SPI port is already open!\n\r");
			printf("\n\r");
			return;
		}
		else if (res != CLI_IF_OK)
		{
			printf("\n\r");
			printf("  ERROR: SPI Can't open port!\n\r");
			printf("\n\r");
			return;
		}

		printf("\n\r");
		printf("  INFO: SPI Port opened.\n\r");
		printf("\n\r");
		return;
	}
	else if (strcmp(command, "close") == 0)
	{
		uint32_t res = CLI_IF_SPI_Close();
		if (res == CLI_IF_ERR_SPI_CLOSED)
		{
			printf("\n\r");
			printf("  ERROR: SPI port is already closed!\n\r");
			printf("\n\r");
			return;
		}
		else if (res != CLI_IF_OK)
		{
			printf("\n\r");
			printf("  ERROR: SPI Can't close port!\n\r");
			printf("\n\r");
			return;
		}

		printf("\n\r");
		printf("  INFO: SPI Port closed.\n\r");
		printf("\n\r");
		return;
	}
	else if (strcmp(command, "speed") == 0)
	{
		if (!data)
		{
			CLI_Help("spi");
			return;
		}

		char *endptr;
		long int num;

		num = strtol(data, &endptr, 10);
		if (endptr == data)
		{
			CLI_Help("spi");
			return;
		}
		else if (*endptr != '\0')
		{
			CLI_Help("spi");
			return;
		}

		uint32_t res = CLI_IF_SPI_Speed((uint8_t)num);

		if (res == CLI_IF_ERR_SPI_OPENED)
		{
			printf("\n\r");
			printf("  ERROR: SPI Port not closed!\n\r");
			printf("\n\r");
		}
		else if (res == CLI_IF_ERR_VALUE)
		{
			printf("\n\r");
			printf("  ERROR: SPI Speed Incorrect value \'%ld\'!\n\r", num);
			printf("\n\r");
		}
		return;
	}
	else if (strcmp(command, "mode") == 0)
	{
		if (!data)
		{
			CLI_Help("spi");
			return;
		}

		char *endptr;
		long int num;

		num = strtol(data, &endptr, 10);

		if (endptr == data)
		{
			CLI_Help("spi");
			return;
		}
		else if (*endptr != '\0')
		{
			CLI_Help("spi");
			return;
		}

		uint32_t res = CLI_IF_SPI_Mode((uint8_t)num);

		if (res == CLI_IF_ERR_SPI_OPENED)
		{
			printf("\n\r");
			printf("  ERROR: SPI Port not closed!\n\r");
			printf("\n\r");
		}
		else if (res == CLI_IF_ERR_VALUE)
		{
			printf("\n\r");
			printf("  ERROR: SPI Mode Incorrect value \'%ld\'!\n\r", num);
			printf("\n\r");
		}
		return;
	}
	else
	{
		printf("\n\r");
		printf("  ERROR: Unknown command \'%s\'!\n\r", command);
		printf("\n\r");
	}
}

/**
 * @brief Парсер Интерфейса GPIO.
 * @param args Аргументы.
 */
void CLI_Interface_GPIO(char *args)
{

	char *command = strtok(args, " ");
	char *data = strtok(NULL, "");


	if (!command)
	{
		CLI_Help("gpio");
		return;
	}

	if (strcmp(command, "read") == 0)
	{
		uint8_t gpio = 0;

		if (CLI_IF_GPIO_Read(&gpio) != CLI_IF_OK)
		{
			printf("\n\r");
			printf("  ERROR: GPIO Can't read!\n\r");
			printf("\n\r");
			return;
		}

		printf("\n\r");
		printf("  GPIO: %02X.\n\r", gpio);
		printf("\n\r");

		return;
	}
	else if (strcmp(command, "write") == 0)
	{

		uint8_t gpio = 0;
		int len = parse_bytes(data, &gpio, 1);

		if (len == 0)
		{
			CLI_Help("gpio");
			return;
		}

		uint32_t res = CLI_IF_GPIO_Write(gpio);

		if (res == CLI_IF_ERR_VALUE)
		{
			printf("\n\r");
			printf("  ERROR: GPIO Incorrect value \'%s\'!\n\r", data);
			printf("\n\r");
			return;
		}
		else if (CLI_IF_GPIO_Write(gpio) != CLI_IF_OK)
		{
			printf("\n\r");
			printf("  ERROR: GPIO Can't write!\n\r");
			printf("\n\r");
			return;
		}

		printf("\n\r");
		printf("  INFO: GPIO Write command.\n\r");
		printf("\n\r");
		return;
	}
}

/**
 * @brief Преобразует строку типа "1F CA 03" в массив байт.
 * @param cmd Строка в HEX формате.
 * @param buf Массив назначения.
 * @param max_len Длина массива.
 * @return Количество байт в массиве.
 */
static int parse_bytes(char *cmd, uint8_t *buf, int max_len)
{
	int len = 0;

	char *token = strtok(cmd, " ");

	while (token && len < max_len)
	{
		buf[len++] = hex_to_byte(token);
		token = strtok(NULL, " ");
	}

	return len;
}

/**
 * @brief Преобразует строку HEX "C8" в байт.
 * @param str Указатель на строку.
 * @return Целочисленное значение.
 */
static uint8_t hex_to_byte(const char *str)
{
	return (uint8_t)strtol(str, NULL, 16);
}

void SPI_Select(void)
{
}

void SPI_Unselect(void)
{
}