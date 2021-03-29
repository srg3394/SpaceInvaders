// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 1/17/2020 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "dac.h"
#include "../inc/tm4c123gh6pm.h"

const unsigned short Wave[32] = {8, 9, 11, 12, 13, 14, 14, 15, 15, 15, 14, 
14, 13, 12, 11, 9, 8, 7, 5, 4, 3, 2, 2, 
1, 1, 1, 2, 2, 3, 4, 5, 7};


uint32_t value = 0;
// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void){
  NVIC_ST_CTRL_R = 0; // intializes SysTick Timer
	NVIC_ST_RELOAD_R = 0x00FFFFFF;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R = 0x07; // enables systick, clock, and interrupts
	DAC_Init(); // intializes digital outputs by calling DAC_Init
}


// **************Sound_Play*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Play called again
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Sound_Play(uint32_t period){
	//period = 80,000,000/(frequency * number of samples)
	if (period == 0){
	NVIC_ST_RELOAD_R =0;
	NVIC_ST_CURRENT_R =0;	//no sound if period is 0
	return;	
	}
	
	NVIC_ST_RELOAD_R = period-1;
	NVIC_ST_CURRENT_R = 0;
	//SysTick_Handler(); -> called automatically
	
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x70000000; // priority 2          
  NVIC_ST_CTRL_R = 0x07; // enable SysTick with core clock and interrupts
}
/*
  A - 440.00 Hz
B - 493.88 Hz
C - 261.63 Hz
D - 293.66 Hz
E - 329.63 Hz
F - 349.23 Hz
G - 392.00 Hz
*/

void SysTick_Handler(void){ // ISR
	DAC_Out(Wave[value]);
	value = (value + 1) % 32; // modulus to repeat after cycle of 32
}
