#include <LPC17xx.H>
#include "KBD.h"

uint32_t KBD_val = 0
;
void KBD_Init (void) {
	LPC_SC->PCONP |= (1 << 15); // Turn on Power to joy-stick
	
	// GPIO for Joystick 
	LPC_PINCON->PINSEL3 &= ~((3<<8) | (3<<14) | (3<<16) | (3<<18) | (3<<20));
	
	//Toggles inputs for Joystick
	LPC_GPIO1->FIODIR &= ~((1<<20) | (1<<23) | (1<<24) | (1<<25) | (1<<26));
	
}

uint32_t KBD_get (void) {
	uint32_t kbd_val;
	kbd_val = (LPC_GPIO1->FIOPIN >> 20) & KBD_MASK;
	return (kbd_val);
	}

	uint32_t get_button(void){
		uint32_t val = 0;
		val = KBD_get();
		val = (~val & KBD_MASK);
		return (val);
	}
	