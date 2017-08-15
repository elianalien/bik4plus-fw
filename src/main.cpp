/* bik4plus-fw, a firmware for BIK4+ Bike Sharing Lock
 * main.cpp
 *
 * Copyright (C) 2017 Tuwuh Sarwoprasojo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Beeper.h"
#include "MFRC522.h"
#include "OmniLock.h"
#include "PacmanLock.h"
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

	// External event: lock request
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

	// Unlock request (simulate server request)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
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

	uint8_t uidWhiteList[][3] =
	{
			{0xE5, 0x95, 0x15},
			{0x5D, 0x0C, 0x88},
			{0x8C, 0x1B, 0xDC},
			{0xA2, 0x4F, 0xA0}
	};
	uint8_t whiteListCount = sizeof(uidWhiteList) / sizeof(uidWhiteList[0]);
	MFRC522::MIFARE_Key key = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

//	OmniLock::HardwareConfig hw;
//	hw.GpioMotorA = GPIOA;
//	hw.PinMotorA = GPIO_Pin_10;
//	hw.GpioMotorB = GPIOA;
//	hw.PinMotorB = GPIO_Pin_11;
//	hw.GpioSwitchStep = GPIOA;
//	hw.PinSwitchStep = GPIO_Pin_15;
//	hw.GpioSwitchClosed = GPIOA;
//	hw.PinSwitchClosed = GPIO_Pin_12;
//	OmniLock omni(hw);
//	omni.Initialize();

	PacmanLock::HardwareConfig hw;
	hw.GpioMotorA = GPIOA;
	hw.PinMotorA = GPIO_Pin_10;
	hw.GpioMotorB = GPIOA;
	hw.PinMotorB = GPIO_Pin_11;
	hw.GpioSwitchGateOpen = GPIOA;
	hw.PinSwitchGateOpen = GPIO_Pin_3;
	hw.GpioSwitchFree = GPIOA;
	hw.PinSwitchFree = GPIO_Pin_2;
	hw.GpioSwitchLocked = GPIOA;
	hw.PinSwitchLocked = GPIO_Pin_1;
	PacmanLock pacman(hw);
	pacman.Unlock();

	MFRC522 rfid(SPI1, GPIOA, GPIO_Pin_4);
	rfid.PCD_Init();

	Beeper beeper(GPIOB, GPIO_Pin_8);
	beeper.BeepOnce();

	// RFID test
//	while (1) {
//		bool tagFound = false;
//		uint32_t timeout = Ticks::Get() + Ticks::MsToTicks(500);
//		while (!Ticks::HasElapsed(timeout) && !tagFound) {
//			if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
//				MFRC522::Uid uid = rfid.uid;
//				MFRC522::StatusCode status;
//				status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 7, &key, &uid);
//				if (status == MFRC522::STATUS_OK) {
//					if (memcmp(uidByteOn, uid.uidByte, 3) == 0) {
//						tagFound = true;
//					}
//					rfid.PCD_StopCrypto1();
//				}
//			}
//		}
//		if (tagFound) {
//			beeper.BeepOnce(50);
//		}
//		Ticks::DelayMs(50);
//	}

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

		if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)) {
			// Unlock request (from server)
			if (pacman.IsLocked()) {
				beeper.Beep(1, 50, 30);
				pacman.Unlock();
				beeper.Beep(2, 50, 30);
			}
		} else {
			// Lock request (from pole switch)
			if (!pacman.IsGateOpen() && !pacman.IsLocked()) {
				rfid.PCD_Init();
				bool tagFound = false;
				uint32_t timeout = Ticks::Get() + Ticks::MsToTicks(500);
				while (!Ticks::HasElapsed(timeout) && !tagFound) {
					if (!(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()))
						continue;

					MFRC522::Uid uid = rfid.uid;
					MFRC522::StatusCode status;
					for (unsigned int idx = 0; idx < whiteListCount; idx++) {
						if (memcmp(uidWhiteList[idx], uid.uidByte, 3) != 0)
							continue;

						status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 7, &key, &uid);
						if (status == MFRC522::STATUS_OK) {
							tagFound = true;
							rfid.PCD_StopCrypto1();
						}
						break;
					}
				}
				rfid.PCD_SoftPowerDown();

				if (tagFound) {
					beeper.Beep(1, 50, 30);
					pacman.Lock();
					beeper.BeepOnce(300);
				} else {
					beeper.Beep(5, 50, 30);
				}
			}
		}

//		RTC_SetAlarm(RTC_GetCounter() + 1);
//		RTC_WaitForLastTask();

//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, DISABLE);

//		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFE);
//		GPIO_ReadOutputData(GPIOE);  // Somehow this prevents MemManage fault
//		SystemInit();
	}
}

#pragma GCC diagnostic pop

