

#include "cli.h"
#include "stdio.h"
#include "string.h"
#include "spi1.h"
#include "stdlib.h"
// #include "cmdp_spi.h"
#include "cli_defs.h"
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

void CLI_Interface_SPI(int argc, const char *const *argv);
void CLI_Interface_GPIO(int argc, const char *const *argv);

void SPI_Select(void);
void SPI_Unselect(void);

static uint8_t hex_to_byte(const char *str);
static int parse_bytes(int argc, const char *const *argv, uint8_t *buf, int max_len);

/**
 * @brief Функция обработки коммандной строки.
 * @param args
 */
void CLI_Process(int argc, const char *const *argv)
{
	// Получаем название функции.
	if (argc == 0)
	{
		CLI_Help(NULL);
		return;
	}

	const char *interface = argv[0];

	// Получаем остальную строку.
	// char *data = strtok(NULL, "");
	// char* data = '\0';
	const char *const *data = NULL;
	argc -= 1;
	if (argc > 0)
	{
		data = &argv[1];
	}

	if (strcmp(interface, "spi") == 0)
	{
		// Функция SPI.
		CLI_Interface_SPI(argc, data);
	}
	else if (strcmp(interface, "gpio") == 0)
	{
		// Функция GPIO.
		CLI_Interface_GPIO(argc, data);
	}
	else if (strcmp(interface, "help") == 0)
	{
		// Инструкция.
		CLI_Help(data[0]);
	}
	else
	{

		printf("\n\r");
		printf("  %s: Unknown interface \'%s\'!\n\r", MSG_ERROR_HEADER, interface);
		printf("\n\r");

		// Неизвестная функция.
		// Выводим Help по функциям
		CLI_Help(NULL);
	}
}

static int CLI_IsAllowedChar(uint8_t ch)
{
	if ((ch >= 'a' && ch <= 'z') ||
		(ch >= 'A' && ch <= 'Z') ||
		(ch >= '0' && ch <= '9') ||
		ch == ' ' || ch == '_' || ch == '-' || ch == '.')
	{
		return 1;
	}
	return 0;
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

			// CLI_Process(cli_line);

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

		if (!CLI_IsAllowedChar(ch))
		{
			return; // игнорируем всё лишнее
		}

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
	printf("\n\r");
	printf("\033[32m>\033[0m ");
	fflush(stdout);
}

/**
 * @brief Парсер Интерфейс SPI.
 * @param args Аргументы.
 */
void CLI_Interface_SPI(int argc, const char *const *argv)
{

	if (argc == 0)
	{
		CLI_Help("spi");
		return;
	}

	// Получаем команду.
	const char *command = argv[0];

	// Получаем аргументы.
	const char *const *data = NULL;
	argc -= 1;
	if (argc > 0)
	{
		data = &argv[1];
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

		int len = parse_bytes(argc, data, tx, sizeof(tx));

		uint32_t res = CLI_IF_SPI_Transmit(tx, len);

		if (res == CLI_IF_ERR_SPI_CLOSED)
		{
			printf("\n\r");
			printf("  %s: SPI port not opened!\n\r", MSG_ERROR_HEADER);
			printf("\n\r");
			return;
		}
		else if (res == CLI_IF_ERR_SPI_HARDWARE)
		{
			printf("\n\r");
			printf("  %s: SPI internal error!\n\r", MSG_ERROR_HEADER);
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

		int len = parse_bytes(argc, data, tx, sizeof(tx));

		uint32_t res = CLI_IF_SPI_Tranceive(tx, rx, len);

		if (res == CLI_IF_ERR_SPI_CLOSED)
		{
			printf("\n\r");
			printf("  %s: SPI port not opened!\n\r", MSG_ERROR_HEADER);
			printf("\n\r");
			return;
		}
		else if (res == CLI_IF_ERR_SPI_HARDWARE)
		{
			printf("\n\r");
			printf("  %s: SPI internal error!\n\r", MSG_ERROR_HEADER);
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
			printf("  %s: SPI port is already open!\n\r", MSG_ERROR_HEADER);
			printf("\n\r");
			return;
		}
		else if (res != CLI_IF_OK)
		{
			printf("\n\r");
			printf("  %s: SPI Can't open port!\n\r", MSG_ERROR_HEADER);
			printf("\n\r");
			return;
		}

		printf("\n\r");
		printf("  %s: SPI Port opened.\n\r", MSG_INFO_HEADER);
		printf("\n\r");
		return;
	}
	else if (strcmp(command, "close") == 0)
	{
		uint32_t res = CLI_IF_SPI_Close();
		if (res == CLI_IF_ERR_SPI_CLOSED)
		{
			printf("\n\r");
			printf("  %s: SPI port is already closed!\n\r", MSG_ERROR_HEADER);
			printf("\n\r");
			return;
		}
		else if (res != CLI_IF_OK)
		{
			printf("\n\r");
			printf("  %s: SPI Can't close port!\n\r", MSG_ERROR_HEADER);
			printf("\n\r");
			return;
		}

		printf("\n\r");
		printf("  %s: SPI Port closed.\n\r", MSG_INFO_HEADER);
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

		uint8_t num = (uint8_t)strtol(data[0], &endptr, 10);
		if (endptr == data[0])
		{
			CLI_Help("spi");
			return;
		}
		else if (*endptr != '\0')
		{
			CLI_Help("spi");
			return;
		}

		uint32_t res = CLI_IF_SPI_Speed(num);

		if (res == CLI_IF_ERR_SPI_OPENED)
		{
			printf("\n\r");
			printf("  %s: SPI Port not closed!\n\r", MSG_ERROR_HEADER);
			printf("\n\r");
			return;
		}

		if (res == CLI_IF_ERR_VALUE)
		{
			printf("\n\r");
			printf("  %s: SPI Speed Incorrect value \'%s\'!\n\r", MSG_ERROR_HEADER, data[0]);
			printf("\n\r");
			return;
		}

		printf("\n\r");
		printf("  %s: SPI Speed Set value \'%s\'!\n\r", MSG_INFO_HEADER, data[0]);
		printf("\n\r");

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

		uint8_t num = (uint8_t)strtol(data[0], &endptr, 10);

		if (endptr == data[0])
		{
			CLI_Help("spi");
			return;
		}
		else if (*endptr != '\0')
		{
			CLI_Help("spi");
			return;
		}

		uint32_t res = CLI_IF_SPI_Mode(num);

		if (res == CLI_IF_ERR_SPI_OPENED)
		{
			printf("\n\r");
			printf("  %s: SPI Port not closed!\n\r", MSG_ERROR_HEADER);
			printf("\n\r");
			return;
		}

		if (res == CLI_IF_ERR_VALUE)
		{
			printf("\n\r");
			printf("  %s: SPI Mode Incorrect value \'%s\'!\n\r", MSG_ERROR_HEADER, data[0]);
			printf("\n\r");
			return;
		}

		printf("\n\r");
		printf("  %s: SPI Mode Set value \'%s\'!\n\r", MSG_INFO_HEADER, data[0]);
		printf("\n\r");
		return;
	}
	else
	{
		printf("\n\r");
		printf("  %s: Unknown command \'%s\'!\n\r", MSG_ERROR_HEADER, command);
		printf("\n\r");

		CLI_Help("spi");
	}
}

/**
 * @brief Парсер Интерфейса GPIO.
 * @param args Аргументы.
 */
void CLI_Interface_GPIO(int argc, const char *const *argv)
{

	if (argc == 0)
	{
		CLI_Help("gpio");
		return;
	}

	const char *command = argv[0];

	const char *const *data = NULL;
	argc -= 1;
	if (argc > 0)
	{
		data = &argv[1];
	}

	if (strcmp(command, "read") == 0)
	{
		uint8_t gpio = 0;

		if (CLI_IF_GPIO_Read(&gpio) != CLI_IF_OK)
		{
			printf("\n\r");
			printf("  %s: GPIO Can't read!\n\r", MSG_ERROR_HEADER);
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
		int len = parse_bytes(argc, data, &gpio, 1);

		if (len == 0)
		{
			CLI_Help("gpio");
			return;
		}

		uint32_t res = CLI_IF_GPIO_Write(gpio);

		if (res == CLI_IF_ERR_VALUE)
		{
			printf("\n\r");
			printf("  %s: GPIO Incorrect value \'%s\'!\n\r", MSG_ERROR_HEADER, data[0]);
			printf("\n\r");
			return;
		}
		else if (res != CLI_IF_OK)
		{
			printf("\n\r");
			printf("  %s: GPIO Can't write!\n\r", MSG_ERROR_HEADER);
			printf("\n\r");
			return;
		}

		printf("\n\r");
		printf("  %s: GPIO Write command.\n\r", MSG_INFO_HEADER);
		printf("\n\r");
		return;
	}
	else if (strcmp(command, "mode") == 0)
	{

		uint8_t mode = 0;
		if (argc == 0)
		{
			if (CLI_IF_GPIO_ModeGet(&mode) != CLI_IF_OK)
			{
				printf("\n\r");
				printf("  %s: GPIO Can't get mode!\n\r", MSG_ERROR_HEADER);
				printf("\n\r");
				return;
			}

			printf("\n\r");
			printf("  GPIO: mode %02X.\n\r", mode);
			printf("\n\r");
			return;
		}

		parse_bytes(argc, data, &mode, 1);

		uint32_t res = CLI_IF_GPIO_ModeSet(mode);

		if (res == CLI_IF_ERR_VALUE)
		{
			printf("\n\r");
			printf("  %s: GPIO Incorrect value \'%s\'!\n\r", MSG_ERROR_HEADER, data[0]);
			printf("\n\r");
			return;
		}
		else if (res != CLI_IF_OK)
		{
			printf("\n\r");
			printf("  %s: GPIO Can't set mode!\n\r", MSG_ERROR_HEADER);
			printf("\n\r");
			return;
		}

		printf("\n\r");
		printf("  %s: GPIO Mode command.\n\r", MSG_INFO_HEADER);
		printf("\n\r");
		return;
	}

	else
	{

		printf("\n\r");
		printf("  %s: Unknown command \'%s\'!\n\r", MSG_ERROR_HEADER, command);
		printf("\n\r");

		CLI_Help("gpio");
	}
}

/**
 * @brief Преобразует строку типа "1F CA 03" в массив байт.
 * @param argv Строка в HEX формате.
 * @param buf Массив назначения.
 * @param max_len Длина массива.
 * @return Количество байт в массиве.
 */
static int parse_bytes(int argc, const char *const *argv, uint8_t *buf, int max_len)
{
	int len = 0;

	// const char *token = argv[len];

	// while (token && len < max_len )
	// {
	// 	buf[len++] = hex_to_byte(token);
	// 	token = argv[len];
	// }


	for ( int i = 0; i < argc && i < max_len ; i++)
	{
		buf[i] = hex_to_byte(argv[i]);
		len++;
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