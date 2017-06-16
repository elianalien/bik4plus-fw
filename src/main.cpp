/* bik4plus-fw
 * main.cpp
 *
 * (c) 2017 Tuwuh Sarwoprasojo
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Beeper.h"
#include "MFRC522.h"
#include "OmniLock.h"
#include "Ticks.h"

#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_spi.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

void initRtc()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	BKP_DeInit();

	RCC_LSICmd(ENABLE);
	while (!RCC_GetFlagStatus(RCC_FLAG_LSIRDY));

	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();

	// LSI clock freq is "around" 40 kHz (30 ~ 60 kHz)
	RTC_SetPrescaler(40000 - 1);
	RTC_WaitForLastTask();

	// Enable RTC clock output: RTCCLK/64 on PC13
//	PWR_BackupAccessCmd(ENABLE);
//	BKP_TamperPinCmd(DISABLE);
//	BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);

	// Connect RTC Alarm event to EXTI
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Event;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
}

void initGpio()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;

	// LED
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	// MCO (alternate function for PA8)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	RCC_MCOConfig(RCC_MCO_SYSCLK);

	// SPI pins: PA5 (SCK), PA6 (MISO), PA7 (MOSI)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// PA4 (NSS): software controlled
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);

	// External event
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);

	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line11;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Event;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
}

void initSpi()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_Init(SPI1, &SPI_InitStruct);
	SPI_SSOutputCmd(SPI1, ENABLE);
	SPI_Cmd(SPI1, ENABLE);
}

int main(int argc, char* argv[])
{
	Ticks::Init();
	initGpio();
	initRtc();
	initSpi();

	GPIO_ReadOutputData(GPIOA);  // Somehow this nonsense prevents MemManage fault

//	uint8_t uidByteOn[] = {0xE5, 0x95, 0x15};
//	uint8_t uidByteOff[] = {0x5D, 0x0C, 0x88};
	uint8_t uidByteOn[] = {0x8C, 0x1B, 0xDC};
	uint8_t uidByteOff[] = {0xA2, 0x4F, 0xA0};
	MFRC522::MIFARE_Key key = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

	OmniLock::HardwareConfig hw;
	hw.GpioMotorA = GPIOA;
	hw.PinMotorA = GPIO_Pin_10;
	hw.GpioMotorB = GPIOA;
	hw.PinMotorB = GPIO_Pin_11;
	hw.GpioSwitchStep = GPIOA;
	hw.PinSwitchStep = GPIO_Pin_15;
	hw.GpioSwitchClosed = GPIOA;
	hw.PinSwitchClosed = GPIO_Pin_12;
	OmniLock omni(hw);

	omni.Initialize();

	MFRC522 rfid(SPI1, GPIOA, GPIO_Pin_4);
	rfid.PCD_Init();

	Beeper beeper(GPIOB, GPIO_Pin_8);
	beeper.BeepOnce();

	// Infinite loop
	while (1) {
//		GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction) GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));
//		GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
//		for (unsigned count = 0; count < 10; count++) {
//			GPIO_WriteBit(GPIOB, GPIO_Pin_12,
//					(BitAction) !GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12));
//
//			Ticks::DelayMs(50);
//		}

		rfid.PCD_Init();
		if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
			MFRC522::Uid uid = rfid.uid;
			MFRC522::StatusCode status;
			status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 7, &key, &uid);
			if (status == MFRC522::STATUS_OK) {
				if (memcmp(uidByteOn, uid.uidByte, 3) == 0) {
					GPIO_SetBits(GPIOB, GPIO_Pin_12);
					beeper.Beep(3, 50, 50);
					omni.Unlock();
				} else if (memcmp(uidByteOff, uid.uidByte, 3) == 0) {
					GPIO_ResetBits(GPIOB, GPIO_Pin_12);
					omni.Lock();
					beeper.BeepOnce(500);
				}
				rfid.PCD_StopCrypto1();
			}
		}
		rfid.PCD_SoftPowerDown();

		RTC_SetAlarm(RTC_GetCounter() + 1);
		RTC_WaitForLastTask();

//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);

		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);
//		GPIO_ReadOutputData(GPIOE);  // Somehow this prevents MemManage fault
		SystemInit();
	}
}

#pragma GCC diagnostic pop

