//
// slcan: Parse incoming and generate outgoing slcan messages
//

#include <string.h>
#include "error.h"
#include "cmdp_spi.h"

// Parse an incoming slcan command from the USB CDC port
int32_t cmdp_spi_parse(uint8_t *buf, uint8_t len)
{
	// Handle each incoming command.
	switch (buf[0])
	{

	// Help.
	case 'H':
	case 'h':
		return cmdp_spi_help();

	// Open channel.
	case 'O':
		return cmdp_spi_open();

	// Close channel.
	case 'C':
		return cmdp_spi_close();

	// Set nominal bitrate.
	case 'S':
		return cmdp_spi_speed(buf[1]);

	// Set mode.
	case 'M':
		return cmdp_spi_mode(buf[1]);

	case 'g':
		return cmdp_spi_gpio(&buf[1], len - 1);

	// Report firmware version.
	case 'v':
	case 'V':
	{
		return cmdp_version();
	}

	// Transmit data frame command
	case 'T':
	case 't':
		return cmdp_spi_transmit(&buf[1], len - 1);

	// Transmit remote frame command
	case 'r':
	case 'R':
		cmdp_spi_receive(&buf[1], len - 1);
		return 0;

	// Invalid command
	default:
		cmdp_spi_invalid(buf[0]);
		return -1;
	}

	return 0;
}



void ascii_to_hex_int(uint8_t* buf, uint32_t len)
{
	// Convert from ASCII (2nd character to end).
	for (uint8_t i = 1; i < len; i++)
	{
		// Lowercase letters
		if (buf[i] >= 'a')
			buf[i] = buf[i] - 'a' + 10;
		// Uppercase letters
		else if (buf[i] >= 'A')
			buf[i] = buf[i] - 'A' + 10;
		// Numbers
		else
			buf[i] = buf[i] - '0';
	}
}



__weak int32_t cmdp_spi_open(void)
{
	return 0;
}

__weak int32_t cmdp_spi_close(void)
{
	return 0;
}

__weak int32_t cmdp_spi_speed(uint8_t speed)
{
	UNUSED(speed);
	return 0;
}

__weak int32_t cmdp_spi_mode(uint8_t mode)
{
	UNUSED(mode);
	return 0;
}

__weak int32_t cmdp_spi_gpio(uint8_t *data, uint32_t len)
{
	UNUSED(data);
	UNUSED(len);
	return 0;
}

__weak int32_t cmdp_spi_transmit(uint8_t *data, uint32_t len)
{
	UNUSED(data);
	UNUSED(len);
	return 0;
}

__weak int32_t cmdp_spi_receive(uint8_t *data, uint32_t len)
{
	UNUSED(data);
	UNUSED(len);
	return 0;
}

__weak int32_t cmdp_version(void)
{
	return 0;
}

__weak int32_t cmdp_spi_invalid(uint8_t cmd)
{
	UNUSED(cmd);
	return 0;
}

__weak int32_t cmdp_spi_help(void)
{
	return 0;
}