


#include "cli.h"
#include "stdio.h"
#include "string.h"
#include "spi1.h"
#include "stdlib.h"
#include "cmdp_spi.h"

#define SPI_HANDLE hspi1




enum {
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



void CLI_Process(char *cmd);
void CLI_SPI(char *args);
void CLI_GPIO(char *args);
void CLI_Help(void);
void CLI_CommandHelp(char *args);


void SPI_Select(void);
void SPI_Unselect(void);

static uint8_t hex_to_byte(const char *str);
static int parse_bytes(char *cmd, uint8_t *buf, int max_len);






void CLI_Process(char *cmd)
{
    if (strncmp(cmd, "spi ", 4) == 0)
    {
        CLI_SPI(cmd + 4);
    }
	else if (strncmp(cmd, "gpio ", 5) == 0)
	{
		CLI_GPIO(cmd + 5);
	}
	else if (strncmp(cmd, "help ", 5) == 0)
	{
		CLI_CommandHelp(cmd + 5);
	}
	else if (strncmp(cmd, "help", 4) == 0)
	{
		CLI_Help();
	}




}



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
            printf("\r\n");

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
                memmove(&cli_line[cursor-1], &cli_line[cursor], cli_len - cursor);
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


void CLI_SPI(char *args)
{
    uint8_t tx[64];
    uint8_t rx[64];

    memset(tx, 0, sizeof(tx));
    memset(rx, 0, sizeof(rx));

    // режим: tx или trx
    char *mode = strtok(args, " ");
    char *data = strtok(NULL, "");

    if (!mode)
    {
        printf("Usage:\r\n");
		printf("spi open\r\n");
		printf("spi close\r\n");
        printf("spi tx 01 02 03\r\n");
        printf("spi trx 9F\r\n");
        return;
    }

    int len = parse_bytes(data, tx, sizeof(tx));

    if (strcmp(mode, "tx") == 0)
    {

		if (!data)
		{
			printf("Usage:\r\n");
			printf("spi tx 01 02 03\r\n");
			return;
		}

		SPI_Select();

        HAL_SPI_Transmit(&SPI_HANDLE, tx, len, HAL_MAX_DELAY);

		SPI_Unselect();

        printf("TX: ");
        for (int i = 0; i < len; i++)
            printf("%02X ", tx[i]);
        printf("\r\n");
		return;
    }
    else if (strcmp(mode, "trx") == 0)
    {
		if (!data)
		{
			printf("Usage:\r\n");
			printf("spi trx 9F\r\n");
			return;
		}

		SPI_Select();

        HAL_SPI_TransmitReceive(&SPI_HANDLE, tx, rx, len, HAL_MAX_DELAY);

		SPI_Unselect();

        printf("TX: ");
        for (int i = 0; i < len; i++)
            printf("%02X ", tx[i]);

        printf("\r\nRX: ");
        for (int i = 0; i < len; i++)
            printf("%02X ", rx[i]);

        printf("\r\n");
		return;
    }
	else if (strcmp(mode, "open") == 0)
    {
		cmdp_spi_open();
		printf("Channel opened.\r\n");
		return;
	}
	else if (strcmp(mode, "close") == 0)
    {
		cmdp_spi_close();
		printf("Channel closed.\r\n");
		return;
	}
	else if (strcmp(mode, "speed") == 0)
	{
		if (!data)
		{
			//printf("Usage:\r\n");
			//printf("spi speed 0\r\n");
			CLI_CommandHelp("speed");
			return;
		}

		if (data[0] < '0' || data[0] > '7')
		{
			CLI_CommandHelp("speed");
			return;
		}

		cmdp_spi_speed(data[0]);
	}
	else if (strcmp(mode, "mode") == 0)
	{
		if (!data)
		{
			//printf("Usage:\r\n");
			//printf("spi speed 0\r\n");
			CLI_CommandHelp("mode");
			return;
		}

		if (data[0] < '0' || data[0] > '3')
		{
			CLI_CommandHelp("mode");
			return;
		}

		cmdp_spi_mode(data[0]);
	}
    else
    {
        printf("Unknown mode\r\n");
    }

}



void CLI_GPIO(char *args)
{
	printf("CLI GPIO\n\r");
}

void CLI_Help(void)
{
	printf("CLI Help\n\r");
}

void CLI_CommandHelp(char *args)
{
	printf("CLI Command Help \'%s\'\n\r", args);
}

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