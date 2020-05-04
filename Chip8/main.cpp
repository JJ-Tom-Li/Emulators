#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"  //The chip8 cpu core implementation
#include <windows.h>  // for MS Windows
//#include "include/GL/glew.h"
//#include "include/GL/glut.h"
/*
    This program is CHIP-8 Emulator.
*/
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
			break;
    }
}
void display() {
	
    //Emulate one cycle
    myChip8.emulateCycle();
    
    //Update the screen if the draw flag is set.
    if(myChip8.drawFlag==1)
        myChip8.drawGraphic();

    glutPostRedisplay();
    Sleep((double)1000/240);
}
int main(int argc, char **argv)
{
    //Set up render system and register input callbacks
    //setupGraphics();
    //setupInput();
    
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
    glutIdleFunc(display);
	createList();
    glutMainLoop();           // Enter the event-processing loop
    
     
    return 0;
}
