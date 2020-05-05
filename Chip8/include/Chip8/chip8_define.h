#ifndef C8_DEF_H
#define C8_DEF_H

#include <stdio.h>
#include <stdlib.h>
#include "../GL/glew.h"
#include "../GL/glut.h"
/*CPU Specifications*/
#define MEMORY_SIZE 4096 //Size of memory
#define STACK_SIZE 16 //Size of stack
#define GFX_X 64
#define GFX_Y 32 //Size of graphics
#define DELAY_TIMER_RATE 60
#define SOUND_TIMER_RATE 60
unsigned char chip8_fontset[80] = {
	0xF0,0x90,0x90,0x90,0xF0, /*"0"*/
	0x20,0x60,0x20,0x20,0x70, /*"1"*/
	0xF0,0x10,0xF0,0x80,0xF0, /*"2"*/
	0xF0,0x10,0xF0,0x10,0xF0, /*"3"*/
	0x90,0x90,0xF0,0x10,0x10, /*"4"*/
	0xF0,0x80,0xF0,0x10,0xF0, /*"5"*/
	0xF0,0x80,0xF0,0x90,0xF0, /*"6"*/
	0xF0,0x10,0x20,0x40,0x40, /*"7"*/
	0xF0,0x90,0xF0,0x90,0xF0, /*"8"*/
	0xF0,0x90,0xF0,0x10,0xF0, /*"9"*/
	0xF0,0x90,0xF0,0x90,0x90, /*"10"*/
	0xE0,0x90,0xE0,0x90,0xE0, /*"11"*/
	0xF0,0x80,0x80,0x80,0xF0, /*"12"*/
	0xE0,0x90,0x90,0x90,0xE0, /*"13"*/
	0xF0,0x80,0xF0,0x80,0xF0, /*"14"*/
	0xF0,0x80,0xF0,0x80,0x80  /*"15"*/
};

void createList(){
	glNewList(1,GL_COMPILE);
		//Blank
	glEndList();
	glNewList(2,GL_COMPILE);
		glScalef(0.02, 0.03, 0);
		glBegin(GL_QUADS);              
			glVertex2f(-0.5f, -0.5f);    // x, y
			glVertex2f( 0.5f, -0.5f);
			glVertex2f( 0.5f,  0.5f);
			glVertex2f(-0.5f,  0.5f);
		glEnd();
	glEndList();
}

class Chip8{
    public:

        unsigned short opcode;      //Use to store opcode
        unsigned char memory[MEMORY_SIZE]; //memory 
        unsigned char V[16];        //CPU Register
        unsigned short I;           //An index register
        unsigned short pc;          //Program Counter,range:0x000 to 0xFFF
                                    /*
                                        The systems memory map:
                                        0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
                                        0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
                                        0x200-0xFFF - Program ROM and work RAM
                                    */
        unsigned char gfx[GFX_Y][GFX_X];   //The graphics system.Totally 2048 pixels
        unsigned char delay_timer;
        unsigned char sound_timer;  //The Chip 8 has no Interupts and hardware registers, but there are two timer registers that count at 60 Hz. When set above zero they will count down to zero.
                                    //The system’s buzzer sounds whenever the sound timer reaches zero.
        unsigned short stack[STACK_SIZE];   //Use to store the program counter 
        unsigned short sp;          //The stack pointer
        //unsigned char key[16];      //The key pad
        char input;
        unsigned short drawFlag;    //1 is draw.
		
        Chip8();
        bool loadGame(char* filename);
        void emulateCycle();
        void drawGraphic();
        void printMemory();
		void showRegisters();
};
#endif