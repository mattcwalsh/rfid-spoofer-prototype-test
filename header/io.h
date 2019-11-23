#ifndef __io_h__
#define __io_h__

// Code taken from io.c to solve compiling errors.
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

#endif
