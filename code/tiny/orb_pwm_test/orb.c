/*
Cycles through each LED of the orb
- using PWM to fade between them
- uses a variation on the TCNT0/slot method discussed here:
http://jeelabs.org/2010/10/03/software-pwm-at-1-khz/

2011-11-19 <nick.oleary@gmail.com> 
http://opensource.org/licenses/mit-license.php
*/
#define F_CPU 1000000

#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <string.h>

#define l1rm 0x02
#define l1gm 0x04
#define l1bm 0x01

#define l2rm 0x01
#define l2gm 0x02
#define l2bm 0x04

#define l3rm 0x08
#define l3gm 0x10
#define l3bm 0x20

unsigned char current[9];

unsigned char fadeTarget[9];
int fadeSteps[9];
int fadeStep;
int fadeStepCount = 20;
char fading;
int stepCount;

unsigned long timer;

unsigned char portASlots[32];
unsigned char portCSlots[32];

void setupSlots(int r1,int g1,int b1,int r2,int g2,int b2,int r3,int g3,int b3);

void jumpTo(int r1,int g1,int b1,int r2,int g2,int b2,int r3,int g3,int b3) {
	current[0] = r1;
	current[1] = g1;
	current[2] = b1;
	current[3] = r2;
	current[4] = g2;
	current[5] = b2;
	current[6] = r3;
	current[7] = g3;
	current[8] = b3;
	setupSlots(r1,g1,b1,r2,g2,b2,r3,g3,b3);
}

void setupSlots(int r1,int g1,int b1,int r2,int g2,int b2,int r3,int g3,int b3) {
	int i;

	current[0] = r1;
	current[1] = g1;
	current[2] = b1;
	current[3] = r2;
	current[4] = g2;
	current[5] = b2;
	current[6] = r3;
	current[7] = g3;
	current[8] = b3;

	memset(portASlots,0,32);
	memset(portCSlots,0,32);

	for (i=0;i<32;i++) {
		portASlots[i] = ((r1>i)?0:l1rm)|((g1>i)?0:l1gm)|((b1>i)?0:l1bm);
		portCSlots[i] = ((r2>i)?0:l2rm)|((g2>i)?0:l2gm)|((b2>i)?0:l2bm)|
			((r3>i)?0:l3rm)|((g3>i)?0:l3gm)|((b3>i)?0:l3bm);
	}
}

void fadeTo(int r1,int g1,int b1,int r2,int g2,int b2,int r3,int g3,int b3) {
	int i;

	fadeTarget[0] = r1;
	fadeTarget[1] = g1;
	fadeTarget[2] = b1;
	fadeTarget[3] = r2;
	fadeTarget[4] = g2;
	fadeTarget[5] = b2;
	fadeTarget[6] = r3;
	fadeTarget[7] = g3;
	fadeTarget[8] = b3;

	for (i=0;i<9;i++) {
		fadeSteps[i] = (fadeTarget[i]-current[i])/fadeStepCount;
	}
	fadeStep = 0;
	fading = 1;

}
void stepFade() {
	int i;
	fadeStep++;
	if (fadeStep == fadeStepCount) {
		for (i=0;i<9;i++) {
			current[i] = fadeTarget[i];
		}
		fading = 0;

	} else {
		for (i=0;i<9;i++) {
			current[i] += fadeSteps[i];
			if (fadeSteps[i] > 0 && current[i] > fadeTarget[i]) current[i] = fadeTarget[i];
			else if (fadeSteps[i] < 0 && current[i] < fadeTarget[i]) current[i] = fadeTarget[i];
		}
	}
	jumpTo(current[0],current[1],current[2],current[3],current[4],current[5],current[6],current[7],current[8]);
}

void setup(void) {
	DDRA = 0x07;
	DDRC = 0x3F;
	fading = 0;
	jumpTo(0,0,0,0,0,0,0,0,0);
	PORTA = (PORTA&0xF8)|0x03;
	PORTC = (PORTC&0xC0)|0x3f;
	timer = 20000;
	TCCR0A = 0x02;

}

int state = 0;

int main(void) {
	setup();

	while(1){
		if (timer == 0) {
			timer = 100000;
			if (state == 0) {
				fadeTo(31,0,0, 31,0,0, 31,0,0);
			} else if (state == 1) {
				fadeTo(31,31,0, 31,31,0, 31,31,0);
			} else if (state == 2) {
				fadeTo(0,31,0 ,0,31,0, 0,31,0);
			} else if (state == 3) {
				fadeTo(0,31,31, 0,31,31, 0,31,31);
			} else if (state == 4) {
				fadeTo(0,0,31, 0,0,31, 0,0,31);
			} else if (state == 5) {
				fadeTo(31,0,31, 31,0,31, 31,0,31);
			} else if (state == 6) {
				fadeTo(16,0,16, 16,0,16, 16,0,16);
			} else if (state == 7) {
				fadeTo(8,0,8, 8,0,8, 8,0,8);
			} else if (state == 8) {
				fadeTo(4,0,4, 4,0,4, 4,0,4);
			} else if (state == 9) {
				fadeTo(2,0,2, 2,0,2, 2,0,2);
			}
			state++;
			if (state == 10) state = 0;
		}

		PORTA = (PORTA&0xF8)|portASlots[TCNT0%32];
		PORTC = (PORTC&0xC0)|portCSlots[TCNT0%32];

		if (fading) {
			if (TCNT0 == 255) {
				stepCount++;
			}
			if (stepCount == 16) {
				stepFade();
				stepCount = 0;
			}
		}
		timer--;
	}
	return 0;
}
