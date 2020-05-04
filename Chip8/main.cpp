#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"  //The chip8 cpu core implementation
#include <windows.h>  // for MS Windows
//#include "include/GL/glew.h"
//#include "include/GL/glut.h"
/*
    This program is CHIP-8 Emulator.
*/
#define FPS 240
char *filename; 
//Initialize chip8 system and load game into memory.
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
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            myChip8.input = key;
            break;
		default:
			 myChip8.input = 'G';
			break;
    }
}
void timer(int t){
	glutTimerFunc(1000.0/FPS, timer, 0);
	//Emulate one cycle
    myChip8.emulateCycle();
    
    //Update the screen if the draw flag is set.
    //if(myChip8.drawFlag==1)
    //    myChip8.drawGraphic();

    //glutPostRedisplay();
    //Sleep((double)1000/120);
}
void display() {
	
    //Emulate one cycle
    //myChip8.emulateCycle();
    
    //Update the screen if the draw flag is set.
    //if(myChip8.drawFlag==1)
    myChip8.drawGraphic();

    glutPostRedisplay();
	//glutTimerFunc(1000.0/60, display, 0);
    //Sleep((double)1000/120);
}
int main(int argc, char **argv)
{
    
    filename = argv[1];
    int a = 1; 
    printf("Loading Game : %s...",filename);
    myChip8.loadGame(filename);
    printf("Done!\n");
    glutInit(&a, argv);                 // Initialize GLUT
    glutInitWindowPosition(10, 10); // Position the window's initial top-left corner
    glutInitWindowSize(800,800);   // Set the window's initial width & height
    glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display); // Register display callback handler for window re-paint
    //glutIdleFunc(display);
	createList();
	glutTimerFunc( 1000.0/FPS, timer, 0); ///註冊一個計時器函式, 第一次進入時間是 10 msec 後呼叫
	timer(0) ;
    glutMainLoop();           // Enter the event-processing loop
    
     
    return 0;
}
