// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
 
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
 
// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5
void ADC_Init(void){ 
       SYSCTL_RCGCGPIO_R |= 0x08; // activate clock for Port D
       while((SYSCTL_PRGPIO_R&0x08)==0){};
       GPIO_PORTD_DIR_R &= ~0x04; //make PD2 input
       GPIO_PORTD_AFSEL_R |= 0x04; //enable alternate fun on PD2
       GPIO_PORTD_DEN_R &= ~0x04; //disable digital I/O on PD2
       GPIO_PORTD_AMSEL_R |= 0x04; //enable analog fun on PD2
       SYSCTL_RCGCADC_R  |= 0x01; //activate ADC0
       volatile int delay = SYSCTL_RCGCADC_R;
       delay = SYSCTL_RCGCADC_R;
       delay = SYSCTL_RCGCADC_R; //time to stabilize
      delay = SYSCTL_RCGCADC_R; 
       ADC0_PC_R = 0x01; //configure for 125K samples per second
       ADC0_SSPRI_R = 0x0123; //Sequencer 3 is highest priority
       ADC0_ACTSS_R &= ~0x0008; //disable sample sequencer 3
       ADC0_EMUX_R &= ~0xF000; // seq3 is software trigger
       ADC0_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + 5; //connects channel 5 to ADC
       ADC0_SSCTL3_R = 0x0006; // no TS0 D0, yes IE0, END0
       ADC0_IM_R &= ~0x0008; //disable SS3 interrupts
       ADC0_ACTSS_R |= 0x0008;    //enable sample sequencer 3   
}
 
 
//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
uint32_t ADC_In(void){  
       uint32_t data;
       ADC0_PSSI_R = 0x0008; //telling sequencer 3 to start sample conversion 
       while((ADC0_RIS_R & 0x08)==0){}; //if 0, busy so stay -> if 1, continue
       data = ADC0_SSFIFO3_R & 0xFFF; //get 12 bits of data
       ADC0_ISC_R = 0x0008; //clears flag         
  return data; 
}
 
Sent from Mail for Windows 10
 
