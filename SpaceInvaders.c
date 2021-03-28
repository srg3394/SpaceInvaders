// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 1/17/2020 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2019

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2019

 Copyright 2019 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "DAC.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
void SysTick_Init(void);
void SysTick_Handler(void);
void EPortF_Init(void);
void GPIOPortF_Handler(void);
void PortF_Init(void);
void GameTask(void);
void Init(void);
void MoveEnemy(void);
void MoveShip(void);
void MoveLaser(void);
void DrawEnemy(void);
void DrawShip(void);
void DrawLaser(void);



uint32_t Inc_Data; // 0 to 4095
uint32_t Semaphore;
uint32_t Flag;
uint32_t fire;
uint32_t paused;
uint32_t score;
uint32_t lose = 0;
uint32_t language;
uint32_t collision;
uint32_t killCount = 0;

struct State {
		unsigned long x; //x coordinate
		unsigned long y; //y coordinate
		const unsigned short *image; //ptr -> image
		long life; //0 = dead; 1= alive
		int fireFlag;
		int oldx;
		int oldy;
		
};
typedef struct State STyp;
STyp Enemy[20];
STyp Ship[1];
STyp BlackImage[1];
STyp Laser[97];
int counter = 0;
int counter1 = 0;
int counter2 = 0;
void Init(void){
		int i;
		for(i=0;i<5;i++){
				Enemy[i].x = 20*i+10; // 128x160 pixels
				Enemy[i].y = 10; // 16x10 dimensions for enemey
				Enemy[i].image = SmallEnemy30pointA;
				Enemy[i].life = 1;
				Enemy[i].oldx = Enemy[i].x;
				Enemy[i].oldy = Enemy[i].y;
		}
		for(i=5;i<10;i++){
				Enemy[i].x = 20*counter+10; // 128x160 pixels
				Enemy[i].y = 20; // 16x10 dimensions for enemey
				Enemy[i].image = SmallEnemy30pointA;
				Enemy[i].life = 1;
				Enemy[i].oldx = Enemy[i].x;
				Enemy[i].oldy = Enemy[i].y;
				counter++;
		}
		for(i=10;i<15;i++){
				Enemy[i].x = 20*counter1+10; // 128x160 pixels
				Enemy[i].y = 30; // 16x10 dimensions for enemey
				Enemy[i].image = SmallEnemy30pointA;
				Enemy[i].life = 1;
				Enemy[i].oldx = Enemy[i].x;
				Enemy[i].oldy = Enemy[i].y;
				counter1++;
		}
		for(i=15;i<20;i++){
				Enemy[i].x = 20*counter2+10; // 128x160 pixels
				Enemy[i].y = 40; // 16x10 dimensions for enemey
				Enemy[i].image = SmallEnemy30pointA;
				Enemy[i].life = 1;
				Enemy[i].oldx = Enemy[i].x;
				Enemy[i].oldy = Enemy[i].y;
				counter2++;
		}
		for(i=0;i<1;i++){
				Ship[i].x = 54;
				Ship[i].y = 158;
				Ship[i].image = PlayerShip0;
				Ship[i].life = 1;
				Ship[i].oldx = Ship[i].x;
				Ship[i].oldy = Ship[i].y;
		}
		for(i=0;i<1;i++){
				BlackImage[i].image = black;
		}
		for(i=7;i<103;i++){
			  Laser[i].x = i;
				Laser[i].y = 143;
				Laser[i].oldy =143;
				Laser[i].fireFlag = 0;
				Laser[i].oldy = Laser[i].y;
		}	
}
void MoveEnemy(void){
		int i;
		for(i=0;i<5;i++){ //84x48
				if(Enemy[i].y <149)
					Enemy[i].y += 1;
				else
					Enemy[i].life = 0;
		}
		for(i=5;i<10;i++){
				if(Enemy[i].y <149)
					Enemy[i].y += 1;
				else
					Enemy[i].life = 0;
		}
		for(i=10;i<15;i++){
				if(Enemy[i].y <149)
					Enemy[i].y += 1;
				else
					Enemy[i].life = 0;
		}
		for(i=15;i<20;i++){
				if(Enemy[i].y <149)
					Enemy[i].y += 1;
				else
					Enemy[i].life = 0;
		}
		for(int i=0;i<20;i++){
				if(Enemy[i].y >= Ship[0].y-8){
						Enemy[i].life = 0;
						lose = 1;
				}		
		}	
		// check for collisions enemy 16x10, laser 4x4
		for(int i = 0;i<20;i++){
				for(int j = 7; i<103;j++){
						if(Laser[j].fireFlag == 1 && Enemy[i].life == 1){
							if((Laser[j].x >= Enemy[i].x && Laser[j].x <= Enemy[i].x +16)&&
								(Laser[j].y -4 <= Enemy[i].y && Laser[j].y-4 >= Enemy[i].y -10))
							{
									Laser[j].fireFlag = 0;
									Enemy[i].life = 0;
									score+=5;
									collision = 1;
				
								
							}	
						}	
				}	
		}
}
void MoveShip(void){
	while(!Semaphore){};
		Semaphore = 0;
		Ship[0].oldx = Ship[0].x;
		
		Ship[0].x = 0.0235657*(Inc_Data)+2.933616653; // update current position based on ADCMail

	// check boundaries first
	if (Ship[0].x > 102)
		Ship[0].x = 101;
		
//	if(Inc_Data <= 200 && Ship[0].x >= 7){
//			Ship[0].x -= 5;
//	}
//	
//	if(Inc_Data >= 3900 && Ship[0].x <= 103){
//			Ship[0].x += 5;
//	}		
}
void MoveLaser(void){
		if(fire){
				fire = 0;
				Laser[Ship[0].x].fireFlag = 1;
		}
		for(int i=7;i<103;i++){
				if(Laser[i].fireFlag == 1){
						if(Laser[i].y < 10){
								Laser[i].fireFlag = 0;
						}
						else{
								Laser[i].oldy = Laser[i].y;
								Laser[i].y -= 4;
						}
				}	
		}	
}	
//void DrawBlack(void){
//	ST7735_DrawBitmap(Ship[0].x,Ship[0].y,BlackImage[0].image,18,8);
//}	
void DrawEnemy(void){
		for(int j=0;j<5;j++){
			if(Enemy[j].life >0){
					ST7735_DrawBitmap(Enemy[j].x,Enemy[j].y,Enemy[j].image,16,10);
			}
			else{
					ST7735_DrawBitmap(Enemy[j].x,Enemy[j].y,black,16,10);
			}		
		}
		for(int j = 5;j<10;j++){
				if(Enemy[j].life >0){
					ST7735_DrawBitmap(Enemy[j].x,Enemy[j].y,Enemy[j].image,16,10);
				}
				else{
					ST7735_DrawBitmap(Enemy[j].x,Enemy[j].y,black,16,10);
				}	

		}
		for(int j = 10;j<15;j++){
				if(Enemy[j].life >0){
					ST7735_DrawBitmap(Enemy[j].x,Enemy[j].y,Enemy[j].image,16,10);
				}
				else{
					ST7735_DrawBitmap(Enemy[j].x,Enemy[j].y,black,16,10);
				}	

		}
		for(int j = 15;j<20;j++){
				if(Enemy[j].life >0){
					ST7735_DrawBitmap(Enemy[j].x,Enemy[j].y,Enemy[j].image,16,10);
				}
				else{
					ST7735_DrawBitmap(Enemy[j].x,Enemy[j].y,black,16,10);
				}	

	}
}
void DrawShip(void){
		ST7735_DrawBitmap(Ship[0].oldx,158,black,18,8);
		ST7735_DrawBitmap(Ship[0].x,158,PlayerShip0,18,8);
}
void DrawLaser(void){ //128x160
		for(int i = 7;i<103;i++){
			if(Laser[i].fireFlag == 1){
					ST7735_DrawBitmap(Laser[i].x,Laser[i].oldy,black,4,4);
					ST7735_DrawBitmap(Laser[i].x,Laser[i].y,LaserImage,4,4);
			}
			else{
					ST7735_DrawBitmap(Laser[i].x,Laser[i].oldy,black,4,4);
					ST7735_DrawBitmap(Laser[i].x,Laser[i].y,black,4,4);
					Laser[i].oldy = 143;
					Laser[i].y = 143;
			}			
		}	
		if(collision == 1){
				collision = 0;
				Sound_Killed();
				killCount++;
		}	
}	

//void PortB_Init(void){
//		SYSCTL_RCGCGPIO_R |= 0x02;
//		volatile int nop1 =1;
//		nop1++;
//		GPIO_PORTE_DIR_R &= 0xFC; // sets PE0-1 to inputs 
//		GPIO_PORTE_DEN_R |= 0x03;
//}	
//int Sound(void){
//		PLL_Init(Bus80MHz);
//		DAC_Init();
//		EPortF_Init();
//		EnableInterrupts(); // test to see if buttons work
//		
//		while(1){
//				
//		}	
//}	
int main(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
	ST7735_InitR(INITR_REDTAB);
	ADC_Init();
	Sound_Init();
	SysTick_Init();
	PortF_Init();
	Init();
	Timer1_Init(&GameTask, 80000000/30);	 //30Hz
//	DisableInterrupts();
 // Output_Init();
	
 // ST7735_FillScreen(0x0000);            // set screen to black
	
	 // 0 = english, 1 = spanish
	ST7735_SetCursor(7,5);
	ST7735_OutString("English");
	ST7735_SetCursor(7,10);
	ST7735_OutString("Espanol");
	
	while (GPIO_PORTF_DATA_R == 0x11){};
	if ((GPIO_PORTF_DATA_R&0x10)>>4 == 0){
		language = 0;
	}		
	else{
		language = 1;
	}
//	while (((GPIO_PORTF_DATA_R&0x10)>>4 == 0)	|| ((GPIO_PORTF_DATA_R&0x01) == 0)){};
//	while ((GPIO_PORTF_DATA_R&0x10)>>4 == 1){}
	
	ST7735_FillScreen(0x0000);
	
	DrawShip();
	DrawEnemy();
	DrawLaser();	
	EPortF_Init(); //PF4 shoot, PF0 pause/play
	EnableInterrupts();
	
	
	
  while(killCount != 20){
		
		while(!Flag){}; //wait 30 Hz
		Flag = 0;
			
		DrawShip();
		DrawEnemy();
		DrawLaser();
		
		language*=1;
		
		if(lose == 1){
				DisableInterrupts();
				NVIC_DIS0_R = 1<<21; //disable timer 1
				break;
		}	
			
		
  }
	ST7735_FillScreen(0x0000);
	if(lose == 1 && language == 0){
			ST7735_SetCursor(6,3);
			ST7735_OutString("You lose!");
			ST7735_SetCursor(2,5);
			ST7735_OutString("Score: ");
			LCD_OutDec(score);
	}
	if(lose == 1 && language == 1){
			ST7735_SetCursor(6,3);
			ST7735_OutString("You lose!");
			ST7735_SetCursor(2,5);
			ST7735_OutString("Score: ");
			LCD_OutDec(score);
	}	
	if(lose == 1 && language == 1){
			ST7735_SetCursor(6,3);
			ST7735_OutString("Enemigos Ganan!");
			ST7735_SetCursor(2,5);
			ST7735_OutString("Puntaje: ");
			LCD_OutDec(score);
	}		
	if(lose == 0 && language == 0){
			ST7735_SetCursor(6,3);
			ST7735_OutString("You win!");
			ST7735_SetCursor(2,5);
			ST7735_OutString("Score: ");
			LCD_OutDec(score);
	}	
	if(lose == 0 && language == 1){
			ST7735_SetCursor(6,3);
			ST7735_OutString("Tu Ganas!");
			ST7735_SetCursor(2,5);
			ST7735_OutString("Puntaje: ");
			LCD_OutDec(score);
	}	
		
}


// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = 2666666; //30 Hz 
	NVIC_ST_CURRENT_R = 0;
	
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 2
	
	NVIC_ST_CTRL_R = 0x07;
}
void SysTick_Handler(void){
	 // 1 means new data
  //GPIO_PORTB_DATA_R ^= 0x10; // toggle PB4
  Inc_Data = ADC_In();      // Sample ADC
  Semaphore = 1;  	// Synchronize with other threads
}
void GPIOPortF_Handler(void){
	if ((GPIO_PORTF_RIS_R & 0x10)>>4 == 1) {		// switch 1 >> shoot
		Sound_Shoot();		
		fire = 1;				// set global flag to 1
	}
	if ((GPIO_PORTF_RIS_R & 0x01) == 1){				// switch 2 >> pause
			paused = 1;								// pause game
			DisableInterrupts();			// when paused, disable interrupts
			NVIC_DIS0_R = 1<<21;			// disable Timer1
			
			while ((GPIO_PORTF_DATA_R & 0x01) == 0){};	// wait for switch release
			while ((GPIO_PORTF_DATA_R & 0x01) == 1){};	// wait for unpause
			paused = 0;								// resume game
			EnableInterrupts();				// enable interrupts when resumed
			NVIC_EN0_R = 1<<21;
		}
	GPIO_PORTF_ICR_R = 0x11; //acknowledge PF0 and PF4 flag
}	
void EPortF_Init(void){
	volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x00000020; 	// activate clock for Port F;
  delay = 100; delay=100; delay=100;
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;       		// allow changes to PF4-0
	GPIO_PORTF_AMSEL_R &=	~0x11;			// disable analog on PF4,0																	
	GPIO_PORTF_PCTL_R &= ~0x000F000F; // configure PF4,0 as GPIO
  GPIO_PORTF_DIR_R &= ~0x11;    	  // make PF4,0 in
	GPIO_PORTF_AFSEL_R &= ~0x11;			// disable alt function on PF4,0
  GPIO_PORTF_PUR_R |= 0x11;     	  // enable weak pull-up on PF4,0
  GPIO_PORTF_DEN_R |= 0x11;     	  // enable digital I/O on PF4,0
	GPIO_PORTF_IS_R &= ~0x11;     	  // PF4,0 are edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x11;    	  // PF4,0 are not both edges
  GPIO_PORTF_IEV_R &= ~0x11;    	  // PF4,0 falling edge event
  GPIO_PORTF_ICR_R = 0x11;      	  // clear flag4
  GPIO_PORTF_IM_R |= 0x11;      	  // arm interrupt on PF4
	NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00400000; // priority 2
	NVIC_EN0_R = 1<<30; 
}
// enable interrupt 30 in NVIC
void PortF_Init(void){ volatile uint32_t delay;
  SYSCTL_RCGCGPIO_R |= 0x00000020; 	// activate clock for Port F;
  delay = 100; delay=100; delay=100;
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;       	// allow changes to PF4-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_DIR_R = 0x0E;      	// PF4,PF0 in, PF3-1 out
  GPIO_PORTF_PUR_R = 0x11;      	// enable pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R = 0x1F;      	// enable digital I/O on PF4-0
}

void GameTask(void){
		MoveEnemy();
		MoveShip();
		MoveLaser();
		Flag =1;
}

