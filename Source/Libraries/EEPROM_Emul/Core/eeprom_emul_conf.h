#ifndef __EEPROM_EMUL_CONF_H
#define __EEPROM_EMUL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif



/**
 * Данные хранятся в памяти EEPROM следующим образом.
 * Какждая ячейка занимает 8 байт, которые распределены следующим образом:
 *
 * Byte 7..4 - Data
 * Byte 3..2 - CRC
 * Byte 1..0 - Virtual address.
 *
 */

// Start address of the 1st page in flash, for EEPROM emulation.
#define START_PAGE_ADDRESS      (0x080FA000UL)

//Number of 10Kcycles requested, minimum 1 for 10Kcycles (default) for instance
//10 to reach 100Kcycles. This factor will increase pages number.
#define CYCLES_NUMBER           (1U)

// Number of guard pages avoiding frequent transfers
// (must be multiple of 2): 0,2,4..
#define GUARD_PAGES_NUMBER      (2U)


// CRC polynomial lenght 16 bits.
#define CRC_POLYNOMIAL_LENGTH   (16)

// Polynomial to use for CRC calculation.
#define CRC_POLYNOMIAL_VALUE    (0x8005U)

// Disable swaping memory banks.
#define OB_USER_DUALBANK_SWAP_DISABLE (1)

// Number of variables to handle in eeprom.
#define NB_OF_VARIABLES         (1000U)

#ifdef __cplusplus
}
#endif

#endif /* __EEPROM_EMUL_CONF_H */
