/*
 * MFRC522.cpp
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wpadded"

#include "MFRC522.h"
#include "Ticks.h"

#define SS_HIGH() GPIO_SetBits(_SS_GPIO, _SS_GPIO_Pin);
#define SS_LOW() GPIO_ResetBits(_SS_GPIO, _SS_GPIO_Pin);

MFRC522::MFRC522(SPI_TypeDef* SPI, GPIO_TypeDef* SS_GPIO, uint16_t SS_GPIO_Pin):
	_SPI(SPI), _SS_GPIO(SS_GPIO), _SS_GPIO_Pin(SS_GPIO_Pin)
{
}

MFRC522::~MFRC522()
{
}

uint8_t MFRC522::spiTransfer(uint8_t data)
{
	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1, data);
	while (!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	return (uint8_t) SPI_I2S_ReceiveData(SPI1);
}

/**
 * Writes a byte to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522::PCD_WriteRegister(	PCD_Register reg,	///< The register to write to. One of the PCD_Register enums.
									uint8_t value			///< The value to write.
								) {
	SS_LOW();
	spiTransfer(reg);
	spiTransfer(value);
	SS_HIGH();
} // End PCD_WriteRegister()

/**
 * Writes a number of bytes to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522::PCD_WriteRegister(	PCD_Register reg,	///< The register to write to. One of the PCD_Register enums.
									uint8_t count,			///< The number of bytes to write to the register
									uint8_t *values		///< The values to write. Byte array.
								) {
	SS_LOW();
	spiTransfer(reg);
	for (unsigned int index = 0; index < count; index++) {
		spiTransfer(values[index]);
	}
	SS_HIGH();
} // End PCD_WriteRegister()

/**
 * Reads a byte from the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
uint8_t MFRC522::PCD_ReadRegister(	PCD_Register reg	///< The register to read from. One of the PCD_Register enums.
								) {
	uint8_t value = 0;
	SS_LOW();
	spiTransfer(0x80 | reg);
	value = spiTransfer(0);
	SS_HIGH();
	return value;
} // End PCD_ReadRegister()

/**
 * Reads a number of bytes from the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
void MFRC522::PCD_ReadRegister(	PCD_Register reg,	///< The register to read from. One of the PCD_Register enums.
								uint8_t count,			///< The number of bytes to read
								uint8_t *values,		///< Byte array to store the values in.
								uint8_t rxAlign		///< Only bit positions rxAlign..7 in values[0] are updated.
								) {
	if (count == 0) {
		return;
	}

	uint8_t address = 0x80 | reg;				// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
	uint8_t index = 0;							// Index in values array.

	SS_LOW();
	count--;								// One read is performed outside of the loop
	spiTransfer(address);					// Tell MFRC522 which address we want to read
	if (rxAlign) {		// Only update bit positions rxAlign..7 in values[0]
		// Create bit mask for bit positions rxAlign..7
		uint8_t mask = (0xFF << rxAlign) & 0xFF;
		// Read value and tell that we want to read the same address again.
		uint8_t value = spiTransfer(address);
		// Apply mask to both current value of values[0] and the new data in value.
		values[0] = (values[0] & ~mask) | (value & mask);
		index++;
	}
	while (index < count) {
		values[index] = spiTransfer(address);	// Read value and tell that we want to read the same address again.
		index++;
	}
	values[index] = spiTransfer(0);			// Read the final byte. Send 0 to stop reading.
	SS_HIGH();
} // End PCD_ReadRegister()

/**
 * Initializes the MFRC522 chip.
 */
void MFRC522::PCD_Init() {
	bool hardReset = false;

//	// Set the chipSelctPin as digital output, do not select the slave yet
//	pinMode(_chipSelectPin, OUTPUT);
//	digitalWrite(_chipSelectPin, HIGH);
//
//	// If a valid pin number has been set, pull device out of power down / reset state.
//	if (_resetPowerDownPin != UINT8_MAX) {
//		// Set the resetPowerDownPin as digital output, do not reset or power down.
//		pinMode(_resetPowerDownPin, OUTPUT);
//
//		if (digitalRead(_resetPowerDownPin) == LOW) {	// The MFRC522 chip is in power down mode.
//			digitalWrite(_resetPowerDownPin, HIGH);		// Exit power down mode. This triggers a hard reset.
//			// Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74μs. Let us be generous: 50ms.
//			delay(50);
//			hardReset = true;
//		}
//	}

	if (!hardReset) { // Perform a soft reset if we haven't triggered a hard reset above.
		PCD_Reset();
	}

	// Reset baud rates
	PCD_WriteRegister(TxModeReg, 0x00);
	PCD_WriteRegister(RxModeReg, 0x00);
	// Reset ModWidthReg
	PCD_WriteRegister(ModWidthReg, 0x26);

	// When communicating with a PICC we need a timeout if something goes wrong.
	// f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
	// TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
	PCD_WriteRegister(TModeReg, 0x80);			// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
	PCD_WriteRegister(TPrescalerReg, 0xA9);		// TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25μs.
	PCD_WriteRegister(TReloadRegH, 0x03);		// Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
	PCD_WriteRegister(TReloadRegL, 0xE8);
	PCD_WriteRegister(RFCfgReg, RxGain_max);
	PCD_WriteRegister(TxASKReg, 0x40);		// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
	PCD_WriteRegister(ModeReg, 0x3D);		// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
	PCD_AntennaOn();						// Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
} // End PCD_Init()

/**
 * Performs a soft reset on the MFRC522 chip and waits for it to be ready again.
 */
void MFRC522::PCD_Reset() {
	PCD_WriteRegister(CommandReg, PCD_SoftReset);	// Issue the SoftReset command.
	// The datasheet does not mention how long the SoftRest command takes to complete.
	// But the MFRC522 might have been in soft power-down mode (triggered by bit 4 of CommandReg)
	// Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74μs. Let us be generous: 50ms.
	Ticks::DelayMs(50);
	// Wait for the PowerDown bit in CommandReg to be cleared
	while (PCD_ReadRegister(CommandReg) & (1<<4)) {
		// PCD still restarting - unlikely after waiting 50ms, but better safe than sorry.
	}
} // End PCD_Reset()

#pragma GCC diagnostic pop
