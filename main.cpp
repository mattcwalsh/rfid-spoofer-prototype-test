/*	Author: Matthew Walsh
 *	Lab Section: 025
 *	Assignment: Custom Lab Project
 *	Exercise Description: RFID Spoofer Prototype
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
// #include <avr/interrupt.h>
// #include <avr/eeprom.h>
// #include "lcd.h"
#include "bit.h"

/*******************************************************************/
/*
unsigned char SetBit(unsigned char pin, unsigned char number, unsigned char bin_value) {
	return (bin_value ? pin | (0x01 << number) : pin & ~(0x01 << number));
}

unsigned char GetBit(unsigned char port, unsigned char number) {
	return ( port & (0x01 << number) );
}
*/
/*******************************************************************/

#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))

// Define LCD outputs.
#define DATA_BUS PORTC
#define CONTROL_BUS PORTD
#define RS 6
#define E 7

// For 8 MHz crystal.
void delay_ms(int miliSec) {
	int i, j;
	for(i=0; i<miliSec; i++)
	for(j=0; j<775; j++) {
		asm("nop");
	}
}

// Reordered for debugging purposes.
void LCD_WriteCommand (unsigned char Command) {
	CLR_BIT(CONTROL_BUS, RS);
	DATA_BUS = Command;
	SET_BIT(CONTROL_BUS, E);
	asm("nop");
	CLR_BIT(CONTROL_BUS, E);
	delay_ms(2);
}

// Reordered for debugging purposes.
void LCD_WriteData(unsigned char Data) {
	SET_BIT(CONTROL_BUS,RS);
	DATA_BUS = Data;
	SET_BIT(CONTROL_BUS,E);
	asm("nop");
	CLR_BIT(CONTROL_BUS,E);
	delay_ms(1);
}

// Reordered for debugging purposes.
void LCD_Cursor (unsigned char column) {
	if ( column < 17 ) {
		LCD_WriteCommand(0x80 + column - 1);
		} else {
		LCD_WriteCommand(0xB8 + column - 9);
	}
}

// Reordered for debugging purposes.
void LCD_ClearScreen(void) {
	LCD_WriteCommand(0x01);
}

// Reordered for debugging purposes.
void LCD_init(void) {
	delay_ms(100);
	LCD_WriteCommand(0x38);
	LCD_WriteCommand(0x06);
	LCD_WriteCommand(0x0f);
	LCD_WriteCommand(0x01);
	delay_ms(10);
}

// Reordered for debugging purposes.
void LCD_DisplayString(unsigned char column ,const unsigned char *string) {
	LCD_ClearScreen();
	unsigned char c = column;
	while(*string) {
		LCD_Cursor(c++);
		LCD_WriteData(*string++);
	}
}

/*******************************************************************/

unsigned char keyIcon[8] = {
	0b01110, 0b01110,
	0b00100, 0b01100,
	0b00100, 0b01100,
	0b00000, 0b00000
};

unsigned char keyLine[8] = {
	0b01110, 0b01110,
	0b00100, 0b01100,
	0b00100, 0b01100,
	0b00000, 0b11111
};

unsigned char sigIcon[8] = {
	0b01110, 0b10001,
	0b00100, 0b01010,
	0b00000, 0b00100,
	0b00100, 0b00100
};

unsigned char inpIcon[8] = {
	0b00100, 0b00100,
	0b00100, 0b11111,
	0b01110, 0b00100,
	0b00000, 0b11111
};

void LCD_addChar(unsigned char *icon) {
	LCD_WriteCommand(0x40);
	int i;
	for (i = 0; i < 8; i++) {
		LCD_WriteData(icon[i]);
	}
	LCD_WriteCommand(0x80);
}

/*******************************************************************/

/*
void eeprom_transfer(double in[]) { 
    // transfers an array to eeprom
    // first word is size, the rest is the array.
	int i;
    eeprom_write_word((uint16_t) 0, maxaddr);
    for (i = 2; i <= (maxaddr); i += 1) {
        eeprom_write_word((uint16_t*) i,(int) in[i - 2]);
    }
}

void eeprom_load_size() {
    // gets the maxaddr from eeprom if set
    int temp = eeprom_read_word((uint16_t*) 0);
    if (0 < temp && temp < MAX_ADDR) {
        maxaddr = temp;
    }
}

void eeprom_load_array(double out[], int size) {
	int i;
    for (i = 2; i <= (size); i += 1){
        out[i-2] = eeprom_read_word((uint16_t*) i);
    }
} */

/*******************************************************************/

void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;

	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

/*******************************************************************/

enum State { start, menu1, menu2, menu3, menu4, on1, off1, on2, off2, on3, off3, on4, off4, beep, wait } state;

void Tick() {

	double* s;
	&s = state;

	switch(state) {
		case start:
			state = menu1;
			break;

	/***************************************/

		case menu1:
			eeprom_transfer(s);
			if (buttonR) {
				state = menu2;
				break;
			}
			else if (buttonS) {
				state = on1;
				break;
			}
			else if (buttonI) {
				state = beep;
				break;
			}
			else {
				state = menu1;
				break;
			}

		case menu2:
			eeprom_transfer(s);
			if (buttonR) {
				state = menu3;
				break;
			}
			else if (buttonL) {
				state = menu1;
				break;
			}
			else if (buttonS) {
				state = on2;
				break;
			}
			else if (buttonI) {
				state = beep;
				break;
			}
			else {
				state = menu2;
				break;
			}

		case menu3:
			eeprom_transfer(s);
			if (buttonR) {
				state = menu4;
				break;
			}
			else if (buttonL) {
				state = menu2;
				break;
			}
			else if (buttonS) {
				state = on3;
				break;
			}
			else if (buttonI) {
				state = beep;
				break;
			}
			else {
				state = menu3;
				break;
			}

		case menu4:
			eeprom_transfer(s);
			if (buttonL) {
				state = menu3;
				break;
			}
			else if (buttonS) {
				state = on4;
				break;
			}
			else if (buttonI) {
				state = beep;
				break;
			}
			else {
				state = menu4;
				break;
			}

	/***************************************/

		case on1:
			if (buttonS) {
				state = off1;
				break;
			}
			else {
				state = menu1;
				break;
			}

		case off1:
			if (buttonS) {
				state = on1;
				break;
			}
			else {
				state = menu1;
				break;
			}

	/***************************************/

		case on2:
			if (buttonS) {
				state = off2;
				break;
			}
			else {
				state = menu2;
				break;
			}

		case off2:
			if (buttonS) {
				state = on2;
				break;
			}
			else {
				state = menu2;
				break;
			}

	/***************************************/

		case on3:
			if (buttonS) {
				state = off3;
				break;
			}
			else {
				state = menu3;
				break;
			}

		case off3:
			if (buttonS) {
				state = on3;
				break;
			}
			else {
				state = menu3;
				break;
			}

	/***************************************/

		case on4:
			if (buttonS) {
				state = off4;
				break;
			}
			else {
				state = menu4;
				break;
			}

		case off4:
			if (buttonS) {
				state = on4;
				break;
			}
			else {
				state = menu4;
				break;
			}

	/***************************************/

		case beep:
			if (buttonI) {
				state = beep;
			}
			else {
				state = menu1;
			}
			break;

		case wait:	
			break;	
	}

/*******************************************************************/

	switch(state) {
		case start:
			output = 0x00;
			break;

		case menu1:
			output = 0x00;
			break;

		case menu2:
			output = 0x00;
			break;

		case menu3:
			output = 0x00;
			break;

		case menu4:
			output = 0x00;
			break;

	/***************************************/

		case on1:
			output = led;
			break;
		case off1:
			output = 0x00;
			break;

	/***************************************/

		case on2:
			output = led;
			break;
		case off2:
			output = 0x00;
			break;

	/***************************************/

		case on3:
			output = led;
			break;
		case off3:
			output = 0x00;
			break;

	/***************************************/

		case on4:
			output = led;
			break;
		case off4:
			output = 0x00;
			break;

	/***************************************/

		case beep:
			break;

		case wait:
			break;
	}
}

/*******************************************************************/

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;

	state = start;

	TimerSet(200);
	TimerOn();

	LCD_init();
	LCD_ClearScreen();
	LCD_Cursor(17);

	//eeprom_load_size();

    while (1) {
		Tick();

		LCD_Cursor(1);
		
		// LCD_WriteData(ICON);

		if (state == menu1) {
			LCD_DisplayString(1, "KEY1: 0F2C014B34");
		}
		else if (state == menu2) {
			LCD_DisplayString(1, "KEY2: 0F2C23C8AE");
		}
		else if (state == menu3) {
			LCD_DisplayString(1, "KEY3: 0F2C4505F4");
		}
		else if (state == menu4) {
			LCD_DisplayString(1, "KEY4: 0F2C5322FB");
		}

		else if ((state == on1) || (state == on2) || (state == on2) || (state == on2)) {
			LCD_ClearScreen();
			LCD_addChar(sigIcon);
			LCD_WriteData(ICON);
		}

		else if ((state == off1) || (state == off2) || (state == off2) || (state == off2)) {
			LCD_ClearScreen();
			LCD_addChar(sigIcon);
			LCD_WriteData(ICON);
		}

		else if (state == beep) {
			LCD_ClearScreen();
			LCD_addChar(inpIcon);
			LCD_WriteData(ICON);
		}

		else {
			continue;
		}

		while(!TimerFlag);
		TimerFlag = 0;
    }
    return 1;
}