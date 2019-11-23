/*	Author: Matthew Walsh
 *	Partner(s) Name: Amir Mohseni
 *	Lab Section:
 *	Assignment: Lab #11  Exercise #1
 *	Exercise Description: Task Scheduler
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>
// #ifdef _SIMULATE_
#include "simAVRHeader.h"

// Added .h files.
#include <stdio.h>
#include "scheduler.h"
#include "bit.h"
#include "io.h"
#include "keypad.h"
#include "timer.h"

// #endif

/*************************************************************************************/

unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;

enum pauseButtonSM_States { pauseButton_wait, pauseButton_press, pauseButton_release };

int pauseButtonSMTick(int state) {
	unsigned char press = ~PINA & 0x01;

	switch(state) {
		case pauseButton_wait:
			state = press == 0x01 ? pauseButton_press : pauseButton_wait;
			break;
		case pauseButton_press:
			state = pauseButton_release;
			break;
		case pauseButton_release:
			state = press == 0x00 ? pauseButton_wait : pauseButton_press;
			break;
		default:
			state = pauseButton_wait;
			break;
	}
	
	switch(state) {
		case pauseButton_wait:
			break;
		case pauseButton_press:
			pause = (pause == 0) ? 1 : 0;
			break;
		case pauseButton_release:
			break;
	}

	return state;
}

/*************************************************************************************/

enum toggleLED0_States { toggleLED0_wait, toggleLED0_blink };

int toggleLED0SMTick(int state) {
	switch (state) {
		case toggleLED0_wait: state = !pause? toggleLED0_blink: toggleLED0_wait; break;
		case toggleLED0_blink: state = pause? toggleLED0_wait: toggleLED0_blink; break;
		default: state = toggleLED0_wait; break;
	}
	switch(state) {
		case toggleLED0_wait: break;
		case toggleLED0_blink:
			led0_output = (led0_output == 0x00) ? 0x01 : 0x00;
			break;
	}
	return state;
}

/*************************************************************************************/

enum toggleLED1_States {toggleLED1_wait, toggleLED1_blink };

int toggleLED1SMTick(int state) {
	switch (state) {
		case toggleLED1_wait: state = !pause? toggleLED1_blink: toggleLED1_wait; break;
		case toggleLED1_blink: state = pause? toggleLED1_wait: toggleLED1_blink; break;
		default: state = toggleLED1_wait; break;
	}
	switch(state) {
		case toggleLED1_wait: break;
		case toggleLED1_blink:
			led1_output = (led1_output == 0x00) ? 0x01 : 0x00;
			break;
	}
	return state;
}

/*************************************************************************************/

enum display_States { display_display };

int displaySMTick(int state) {
	unsigned char output;

	switch (state) {
		case display_display: state = display_display; break;
		default: state = display_display; break;
	}
	switch(state) {
		case display_display:
			output = led0_output | led1_output << 1;
			break;
	}
	PORTB = output;
	return state;
}

/*************************************************************************************/

// Exercise 1.

/*
int main(void) {
	unsigned char x;
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0xF0; PORTA = 0x0F;
	while(1) {
		x = GetKeypadKey();
		switch(x) {
			case '\0': PORTB = 0x1F; break;
			case '1': PORTB = 0x01; break;
			case '2': PORTB = 0x02; break;
			case '3': PORTB = 0x03; break;
			case '4': PORTB = 0x04; break;
			case '5': PORTB = 0x05; break;
			case '6': PORTB = 0x06; break;
			case '7': PORTB = 0x07; break;
			case '8': PORTB = 0x08; break;
			case '9': PORTB = 0x09; break;
			case 'A': PORTB = 0x0A; break;
			case 'B': PORTB = 0x0B; break;
			case 'C': PORTB = 0x0C; break;
			case 'D': PORTB = 0x0D; break;
			case '*': PORTB = 0x0E; break;
			case '0': PORTB = 0x00; break;
			case '#': PORTB = 0x0F; break;
			default: PORTB = 0x1B; break;
		}
	}
}
*/

/*************************************************************************************/

// Exercise 2.

int main(void) {
	unsigned char x;
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0xF0; PORTA = 0x0F;

	unsigned char str[27] = "CS120B is Legend... wait for it DARY!";
	unsigned char cursor = 1;

	while(1) {
		x = GetKeypadKey();
		switch(x) {
			case '\0': PORTB = 0x1F; break;
			case '1': PORTB = 0x01; break;
			case '2': PORTB = 0x02; break;
			case '3': PORTB = 0x03; break;
			case '4': PORTB = 0x04; break;
			case '5': PORTB = 0x05; break;
			case '6': PORTB = 0x06; break;
			case '7': PORTB = 0x07; break;
			case '8': PORTB = 0x08; break;
			case '9': PORTB = 0x09; break;
			case 'A': PORTB = 0x0A; break;
			case 'B': PORTB = 0x0B; break;
			case 'C': PORTB = 0x0C; break;
			case 'D': PORTB = 0x0D; break;
			case '*': PORTB = 0x0E; break;
			case '0': PORTB = 0x00; break;
			case '#': PORTB = 0x0F; break;
			default: PORTB = 0x1B; break;
		}
	}
}

/*
int main() {
    //Insert DDR and PORT initializations.
	DDRA = 0x00; PORTA = 0xFF;	// Keypad input.
	DDRB = 0xFF; PORTB = 0x00;	// LED output.
    DDRC = 0xFF; PORTC = 0x00;	// LCD screen output.
	DDRD = 0xFF; PORTD = 0x00;	// LCD screen output.

	static task task1, task2, task3, task4;
	task *tasks[] = { &task1, &task2, &task3, &task4 };
	const unsigned short numTasks = sizeof(task)/sizeof(task*);

	unsigned long GCD = tasks[0]->period;

	// For loops not allowed in this version of C.
	for (int i = 1; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i]->period);
	}

	int j = 1;
	while (j < numTasks) {
		GCD = findGCD(GCD, tasks[j]->period);
		j++;
	}

	// task1.state = start;
	task1.state = pauseButton_wait;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	// task1.TickFct = &pauseButtonToggleSMTick;
	task1.TickFct = &pauseButtonSMTick;

	// task2.state = start;
	task2.state = toggleLED0_wait;
	task2.period = 500;
	task2.elapsedTime = task2.period;
	task2.TickFct = &toggleLED0SMTick;

	// task3.state = start;
	task3.state = toggleLED1_wait;
	task3.period = 1000;
	task3.elapsedTime = task3.period;
	task3.TickFct = &toggleLED1SMTick;

	// task4.state = start;
	task4.state = display_display;
	task4.period = 10;
	task4.elapsedTime = task4.period;
	task4.TickFct = &displaySMTick; 	// This might be wrong.

	TimerSet(GCD);
	TimerOn();

	unsigned short i;
    // Insert your solution below. 
    while (1) {
		for (i = 0; i < numTasks; i++) {
			if (tasks[i]->elapsedTime == tasks[i]->period) {
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;

			}
			tasks[i]->elapsedTime += GCD;
			
			while(!TimerFlag);
			TimerFlag = 0;

		}
    }

    return 1;
}
*/

