/*
 * MFRC522.h
 */

#ifndef MFRC522_H_
#define MFRC522_H_

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wpadded"

#include "MFRC522Base.h"
#include "stm32f10x.h"

class MFRC522: public MFRC522Base
{
public:
	MFRC522(SPI_TypeDef* SPI, GPIO_TypeDef* SS_GPIO, uint16_t SS_GPIO_Pin);
	virtual ~MFRC522();

	virtual void PCD_WriteRegister(PCD_Register reg, uint8_t value);
	virtual void PCD_WriteRegister(PCD_Register reg, uint8_t count, uint8_t *values);
	virtual uint8_t PCD_ReadRegister(PCD_Register reg);
	virtual void PCD_ReadRegister(PCD_Register reg, uint8_t count, uint8_t *values, uint8_t rxAlign = 0);
	virtual void PCD_Init();
	virtual void PCD_Reset();

private:
	uint8_t spiTransfer(uint8_t data);

private:
	SPI_TypeDef* _SPI;
	GPIO_TypeDef* _SS_GPIO;
	uint16_t _SS_GPIO_Pin;
};

#pragma GCC diagnostic pop

#endif /* MFRC522_H_ */
