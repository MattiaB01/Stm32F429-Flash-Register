/*
 * flash.c
 *
 *  Created on: Sep 13, 2022
 *      Author: Mattia
 */

//keys as described in the data-sheets
/*The following values must be programmed consecutively to unlock the FLASH_CR register
and allow programming/erasing it:
a) KEY1 = 0x45670123
b) KEY2 = 0xCDEF89AB*/
#include "stm32f429xx.h"
#include <stdint.h>
#include "flash.h"

#define FLASH_FKEY1 0x45670123
#define FLASH_FKEY2 0xCDEF89AB


void unlock()
{

	// Wait for the flash memory not to be busy
	while ((FLASH->SR & FLASH_SR_BSY) != 0 );
	// Check if the controller is unlocked already
	if ((FLASH->CR & FLASH_CR_LOCK) != 0 ){
	    // Write the first key
	    FLASH->KEYR = FLASH_FKEY1;
	    // Write the second key
	    FLASH->KEYR = FLASH_FKEY2;
	}
}


//erase sector n.11
void erase_sec11()
{
	/*
	To erase a sector, follow the procedure below:
	1. Check that no Flash memory operation is ongoing by checking the BSY bit in the
	FLASH_SR register
	2. Set the SER bit and select the sector out of the 12 sectors (for STM32F405xx/07xx and
	STM32F415xx/17xx) and out of 24 (for STM32F42xxx and STM32F43xxx) in the main
	memory block you wish to erase (SNB) in the FLASH_CR register
	3. Set the STRT bit in the FLASH_CR register
	4. Wait for the BSY bit to be cleared */

	while ((FLASH->SR & FLASH_SR_BSY)!=0) {};

	//FLASH->CR |= (1U<<1);
	FLASH->CR |= FLASH_CR_SER;

	//Sector 11
	FLASH->CR |= (1U<<3); 	//1
	FLASH->CR |= (1U<<4); 	//1
	FLASH->CR &=~ (1U<<5); 	//0
	FLASH->CR |= (1U<<6); 	//1
	FLASH->CR &=~ (1U<<7); 	//0

	FLASH->CR |= FLASH_CR_STRT;

	while ((FLASH->SR & FLASH_SR_BSY)!=0) {};




}


void program(char *data, uint32_t address)
{
	/*
	Standard programming
	The Flash memory programming sequence is as follows:
	1. Check that no main Flash memory operation is ongoing by checking the BSY bit in the
	FLASH_SR register.
	2. Set the PG bit in the FLASH_CR register
	3. Perform the data write operation(s) to the desired memory address (inside main
	memory block or OTP area):
	– Byte access in case of x8 parallelism
	– Half-word access in case of x16 parallelism
	– Word access in case of x32 parallelism
	– Double word access in case of x64 parallelism
	4. Wait for the BSY bit to be cleared.
	Note: Successive write operations are possible without the need of an erase operation when
	changing bits from ‘1’ to ‘0’. Writing ‘1’ requires a Flash memory erase operation.
	If an erase and a program operation are requested simultaneously, the erase operation is
	performed first.
	*/


	//1
	while ((FLASH->SR & FLASH_SR_BSY) !=0) {};

	//2
	FLASH->CR |= FLASH_CR_PG;

	//3
	//SIZE
	FLASH->CR |= FLASH_CR_PSIZE_1; //32bit


	//data

	uint8_t rest,words;
	words= strlen(data)/4;
	rest = strlen(data)%4;

	for (uint8_t i=0; i<words; i++ )
	{
		 *(uint32_t *) (address) = *(uint32_t *) data;

		data +=4;
		 address+=4;
	}

	switch(rest){
					case 1:
						*(uint32_t *)data = (*(uint32_t *)data) | 0xFFFFFF00;
						break;
					case 2:
						*(uint32_t *)data = (*(uint32_t *)data) | 0xFFFF0000;
						break;
					case 3:
						*(uint32_t *)data = (*(uint32_t *)data) | 0xFF000000;
						break;
	}

	if (rest!=0)
	{
		*(uint32_t*)(address)=*(uint32_t *) data;
	}



    //4
	while ((FLASH->SR & FLASH_SR_BSY) != 0) {};
}



void program_8(char *data, uint32_t address)
{



	//1
	while ((FLASH->SR & FLASH_SR_BSY) !=0) {};

	//8bit Size
	FLASH->CR &= ~(1U<<8);
	FLASH->CR &= ~(1U<<9);
	//2
	FLASH->CR |= FLASH_CR_PG;


	uint8_t rest,words;
	words= strlen(data);
	rest = strlen(data)%4;


	for (uint8_t i=0; i<words; i++ )
	{
		 *(uint8_t *) (address) = *(uint8_t *) data;

		data++;
		address++;
	}

    //4
	while ((FLASH->SR & FLASH_SR_BSY) != 0) {};
}



void read(uint32_t address, char *buffer,uint8_t nWords)
{

	uint32_t *d;
	d=buffer;
	//per leggere 4 word da 32bit
	for (uint8_t i=0; i<nWords; i++)
	{

	*d = *(uint32_t*) address;
	d++;
	address+=4;
	}


}


void lock()
{
	while ((FLASH->SR & FLASH_SR_BSY) != 0 );
	FLASH->CR |= FLASH_CR_LOCK;

}
