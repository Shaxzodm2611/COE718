/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher
 * Note(s): __USE_LCD   - enable Output on LCD, uncomment #define in code to use
 *  				for demo (NOT for analysis purposes)
 *----------------------------------------------------------------------------
 * Copyright (c) 2008-2011 Keil - An ARM Company.
 * Name: Anita Tino
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "Blinky.h"
#include "LPC17xx.h"                       
#include "GLCD.h"
#include "LED.h"
#include "Board_ADC.h" 
#include "KBD.h"
#define __FI        1                      /* Font index 16x24               */
#define __USE_LCD   1										/* Uncomment to use the LCD */

//ITM Stimulus Port definitions for printf //////////////////
#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

struct __FILE { int handle;  };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
}
/////////////////////////////////////////////////////////

char text[10];
char text_l[10];

//static volatile uint16_t AD_dbg;
//uint16_t AD_dbg;
//uint16_t ADC_last;                      // Last converted value
/* Import external variables from IRQ.c file                                  */
extern uint8_t  clock_ms;
/*----------------------------------------------------------------------------
   LED Mapping
 *----------------------------------------------------------------------------*/
static void KBD_to_LED(uint32_t button){
	switch(button){
		case KBD_UP:
			LED_Out(0x01);
			break;
		case KBD_DOWN:
			LED_Out(0x03);
			break;
		case KBD_RIGHT:
			LED_Out(0x07);
			break;
		case KBD_LEFT:
			LED_Out(0x0F);
			break;
		case KBD_SELECT:
			LED_Out(0x1F);
			break;
		case 0:
			LED_Out(0x00); // Pointless just 0x00
	}
}


/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  /*
	int32_t  res;
  uint32_t AD_sum   = 0U;
  uint32_t AD_cnt   = 0U;
  uint32_t AD_value = 0U;
  uint32_t AD_print = 0U;
	*/
  LED_Init();
	KBD_Init();
	
	uint32_t current_button;
	uint32_t prev_button = UINT32_MAX;
	unsigned char *joystick_dir_text;
	/* LED Initialization            */
  //ADC_Initialize();                                /* ADC Initialization            */

#ifdef __USE_LCD
  GLCD_Init();                               /* Initialize graphical LCD (if enabled */

  GLCD_Clear(White);                         /* Clear graphical LCD display   */
  GLCD_SetBackColor(Blue);
  GLCD_SetTextColor(Yellow);
  GLCD_DisplayString(0, 0, __FI, "    COE718 Lab 1    ");
	GLCD_SetTextColor(White);
  GLCD_DisplayString(1, 0, __FI, " Shaxzod Mirkomilov "); // Char[
  GLCD_DisplayString(2, 0, __FI, "        Demo        ");
  GLCD_SetBackColor(White);
  GLCD_SetTextColor(Blue);
	GLCD_DisplayString(5, 0, __FI, "   JOYSTICK VAL:    ");
#endif

  //SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock/100);       /* Generate interrupt each 10 ms */

  while (1) {                                /* Loop forever                  */
			current_button = get_button();
			if (current_button != prev_button){
				prev_button = current_button;
				switch(current_button){
					case KBD_UP:
						joystick_dir_text = "UP";
						KBD_to_LED(KBD_UP);
						break;
					case KBD_DOWN:
						joystick_dir_text = "DOWN";
						KBD_to_LED(KBD_DOWN);
						break;
					case KBD_LEFT:
						joystick_dir_text="LEFT";
						KBD_to_LED(KBD_LEFT);
						break;
					case KBD_RIGHT:
						joystick_dir_text="RIGHT";
						KBD_to_LED(KBD_RIGHT);
						break;
					case KBD_SELECT:
						joystick_dir_text="SELECT";
						KBD_to_LED(KBD_SELECT);
						break;
					case 0:
						break;
					default:
						joystick_dir_text="NONE";
						KBD_to_LED(0);
						break;
				}
				#ifdef __USE_LCD
					GLCD_SetBackColor(White);
					GLCD_SetTextColor(Red);
					GLCD_DisplayString(7,7,__FI, "           "); // Clears joystick param
					GLCD_DisplayString(7,7, __FI, joystick_dir_text);
				#endif
			}
			if (clock_ms) {
				clock_ms = 0;
				printf("Joystick: %s, mask: 0x%02X\r\n", joystick_dir_text, (unsigned int)current_button);
			}
	}
}


