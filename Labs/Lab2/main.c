#include "LPC17xx.h"
#include "Board_LED.h"
#include "Board_GLCD.h"
#include "GLCD_Config.h"
#include "GLCD_Fonts.h"
#include <stdio.h>
//------- ITM Stimulus Port definitions for printf ------------------- //
//-------------------- FROM LAB MANUAL [1] --------------------------- //

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
// Sticking with LEDs 0 and 2 - P1.28 and P2.2 [3] 

#define LED_A_MASK (1 << 28)
#define LED_B_MASK (1 << 2)
#define LED_A_DIRECT (*(volatile unsigned long *)0x233806F0)
#define LED_B_DIRECT (*(volatile unsigned long *) 0x23380A88)

// Demo features are disabled during performance analysis.
// Set to 1 for the target-board demo with LCD output and one-second delays.

#define TOGGLE_DEMO 0

#if TOGGLE_DEMO
volatile uint32_t msTicks = 0;

void SysTick_Handler(void){
	msTicks++;
}

static void delay(uint32_t seconds) {
	/*
	@brief use SysTick API to pause for 'seconds' seconds
	*/
	uint32_t target_ms = seconds * 1000;
	uint32_t start_ticks = msTicks;
	
	while ((msTicks - start_ticks) < target_ms) {
	
	}
}

static void lcd_initialize(void) {
	GLCD_Initialize();
	GLCD_SetBackgroundColor(GLCD_COLOR_WHITE);
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);

	GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
	GLCD_DrawString(64, 0, "COE718 Lab 2");
	GLCD_DrawString(40, 24, "LED bit banding");

	GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
	GLCD_DrawString(0, 72, "P1.28 and P2.2");
	GLCD_DrawString(0, 96, "Method:");
	GLCD_DrawString(0, 144, "State:");
}

static void lcd_show_status(const char *method, const char *state) {
	GLCD_SetForegroundColor(GLCD_COLOR_RED);
	GLCD_DrawString(0, 120, "                    ");
	GLCD_DrawString(0, 120, method);
	GLCD_DrawString(0, 168, "                    ");
	GLCD_DrawString(0, 168, state);
}
#endif

#if TOGGLE_DEMO
	#define DEMO_DELAY() delay(1)
	#define DEMO_LCD_INIT() lcd_initialize()
	#define DEMO_LCD_STATUS(method, state) lcd_show_status(method, state)
#else
	#define DEMO_DELAY() ((void)0)
	#define DEMO_LCD_INIT() ((void)0)
	#define DEMO_LCD_STATUS(method, state) ((void)0)
#endif


// BITBAND METHOD FROM LAB MANUAL [1]
#define ADDRESS(x)    (*((volatile unsigned long *)(x)))
#define BitBand(x, y) 	ADDRESS(((unsigned long)(x) & 0xF0000000) | 0x02000000 |(((unsigned long)(x) & 0x000FFFFF) << 5) | ((y) << 2))

// Methods for LED Operation
__attribute__((noinline))
static void leds_mask(volatile uint32_t on){
	int value;
	if(on == 0){
			value = 0;
	} else {
		value = 1;
	}
	LPC_GPIO1->FIOPIN = (LPC_GPIO1->FIOPIN & ~LED_A_MASK) | (value << 28);
	LPC_GPIO2->FIOPIN = (LPC_GPIO2->FIOPIN & ~LED_B_MASK) | (value << 2);
}

// NEED TO ADD THIS TO HAVE COMPILER NOT OPTIMIZE AWAY CONDITIONAL
__attribute__((noinline))
static void leds_bitband_func(volatile uint32_t on){
	int value;
	if(on == 0){
			value = 0;
	} else {
		value = 1;
	}
	BitBand(&LPC_GPIO1->FIOPIN, 28) = value;
	BitBand(&LPC_GPIO2->FIOPIN, 2) = value;

}

__attribute__((noinline))
static void leds_bitband_direct(volatile uint32_t on){
	int value;
	if(on == 0){
			value = 0;
	} else {
		value = 1;
	}
	LED_A_DIRECT = value;
	LED_B_DIRECT = value;
}

int main(void){
	LED_Initialize();
	SystemInit();	
	DEMO_LCD_INIT();
	
	
	#if TOGGLE_DEMO
		if(SysTick_Config(SystemCoreClock/1000)) {
			while(1){}
		}
		while(1){
	#endif
		
		// Masking
		DEMO_LCD_STATUS("Masking", "ON");
		leds_mask(1);
		DEMO_DELAY();
		DEMO_LCD_STATUS("Masking", "OFF");
		leds_mask(0);
		DEMO_DELAY();
		
		// BitBand Func
		DEMO_LCD_STATUS("BitBand function", "ON");
		leds_bitband_func(1);
		DEMO_DELAY();
		DEMO_LCD_STATUS("BitBand function", "OFF");
		leds_bitband_func(0);
		DEMO_DELAY();
		
		// BitBand Direct
		DEMO_LCD_STATUS("Direct bit band", "ON");
		leds_bitband_direct(1);
		DEMO_DELAY();
		DEMO_LCD_STATUS("Direct bit band", "OFF");
		leds_bitband_direct(0);
		DEMO_DELAY();
	#if TOGGLE_DEMO
		}
	#endif
}

