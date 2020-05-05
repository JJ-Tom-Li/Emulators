#include <stdio.h>
#include <stdlib.h>
#include "include/Chip8/chip8.h"  //The chip8 cpu core implementation
#include <windows.h>  // for MS Windows
/*
    This program is CHIP-8 Emulator.
*/
#define CPU_RATE 500
//Initialize chip8 system.
Chip8 myChip8 = Chip8();
void keyboard(unsigned char key, int x, int y){
    switch(key){
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
			myChip8.input = key - '0';
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            myChip8.input = key - '7'-' '; //Turn input key into hex number.
            break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
            myChip8.input = key - '7';
            break;
		default:
			 myChip8.input = 'G';
			break;
    }
}

void display() {
    //Update the screen
	myChip8.drawGraphic();
	
	
    glutSwapBuffers();
    //glutPostRedisplay();
}
void timer(int t){
	glutTimerFunc(1000.0/CPU_RATE, timer, 0);
	
	//Emulate one cycle
    myChip8.emulateCycle();
	
    //Update the screen if the draw flag is set.
	if(myChip8.drawFlag==1)
		display();
}
int main(int argc, char **argv)
{
	char *filename = argv[1];
    if(!myChip8.loadGame(filename))
		exit(0); //File no found.
    glutInit(&argc, argv);          // Initialize GLUT
    glutInitWindowPosition(10, 10); // Position the window.
    glutInitWindowSize(1280,720);   // Set the window's initial width & height
    glutCreateWindow("Chip8"); 		// Create a window with the given title
    glutKeyboardFunc(keyboard);		// The keyboard input
    glutDisplayFunc(display); 
	createList();
	glutTimerFunc( 1000.0/CPU_RATE, timer, 0); 
    glutMainLoop();           // Enter the event-processing loop
    
     
    return 0;
}
