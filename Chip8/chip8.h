
#ifndef CHIP_8_H
#define CHIP_8_H

#include <stdio.h>
#include "include/GL/glew.h"
#include "include/GL/glut.h"
/*CPU Specifications*/
#define MEMORY_SIZE 4096 //Size of memory
#define STACK_SIZE 16 //Size of stack
#define GFX_X 64
#define GFX_Y 32 //Size of graphics
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
        unsigned char gfx[GFX_Y*GFX_X];   //The graphics system.Totally 2048 pixels
        unsigned char delay_timer;
        unsigned char sound_timer;  //The Chip 8 has no Interupts and hardware registers, but there are two timer registers that count at 60 Hz. When set above zero they will count down to zero.
                                    //The system’s buzzer sounds whenever the sound timer reaches zero.
        unsigned short stack[STACK_SIZE];   //Use to store the program counter 
        unsigned short sp;          //The stack pointer
        unsigned char key[16];      //The key pad
        char input;
        unsigned short drawFlag;    //1 is draw.
        Chip8();
        void loadGame(char* filename);
        void emulateCycle();
        void drawGraphic();
        void printMemory();
};
Chip8::Chip8(){
    int i,j;
    //Initialize memory and registers once.
    this->pc = 0x200;         //Program counter start at 0x200
    this->opcode = 0;         //Reset current opcode
    this->I = 0;              //Reset Index Register
    this->sp = 0;             //Rest Stack Pointer
    this->drawFlag = 0;
    this->input = 'G';

    //Clear display
    for(i=0;i<GFX_Y;i++)
        for(j=0;j<GFX_X;j++)
            this->gfx[i*GFX_X+j] = 0;

    //Clear stack
    this->sp = 0;
    for(i=0;i<STACK_SIZE;i++)
        this->stack[i] = 0;

    //Clear Register V0~VF
    for(i=0;i<=0xF;i++)
        this->V[i] = 0;

    //Clear memory
    for(i=0;i<MEMORY_SIZE;i++)
        this->memory[i]=0;
    //Load font set
    // for(i=0;i<80;++i)
    // {
    //     memory[i]=chip8_fontset[i];
    // }

    //Rest timers
    this->delay_timer = 60;
    this->sound_timer = 60;
}
void Chip8::loadGame(char* filename){
    //Open the code file in binary.
    FILE *f = fopen(filename,"rb");

    if(!f)
        printf("open failure!\n");  //Open failure.
    else
    {
        int bufferSize = 0,i;
        char buffer[4096];          //Use buffer to read the file

        
        //Get the bufferSize
        fseek(f, 0, SEEK_END);  //First seek the end of file.
        bufferSize = ftell(f);  //Get the length of file.
        printf("\n bufferSize = %d\n", bufferSize);
        fseek(f, 0, SEEK_SET);  //Seek back to the start of file.

        //Read the rom file.
        fread(buffer,bufferSize,1,f);
        //start filling the memory at location: 0x200 == 512
        for (i=0;i<bufferSize;i++)
            this->memory[i+512] = buffer[i];  
    }
}
void Chip8::emulateCycle(){
    int x,y;
    unsigned char rnd;
    //Fetch Opcode
    printf("----------------------------------------\n");
    printf("pc:%x\n",this->pc);
    printf("delay_timer:%d\n",this->delay_timer);
    printf("sound_timer:%d\n",this->sound_timer);
    this->opcode = this->memory[this->pc] << 8 | this->memory[this->pc+1];
    
    printf("opcode:%x ,",this->opcode);
    //Decode Opcode
    switch(this->opcode & 0xF000)
    {
        //0 part
        case 0x0000: 
            switch(this->opcode & 0x000F)
            {
                case 0x0000: //0x00E0 Clear the screen
                    //Execute Opcode
                     printf("Clear the screen\n");
                    //Clear display
                    for(int i=0;i<GFX_Y;i++)
                        for(int j=0;j<GFX_X;j++)
                            this->gfx[i*GFX_X+j] = 0;
                    this->drawFlag = 1;
                    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
                    //glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)
                    this->pc += 2;

                    break;
                case 0x000E: // 0x00EE: Returns from subroutine
                    // Execute opcode
                    printf("Returns from subroutine\n");
                    //for(int i=0;i<STACK_SIZE;i++)
                    //    printf("%x ",this->stack[i]);
                    //printf("\n");
                    this->pc = this->stack[this->sp-1] + 2;
                    this->sp--;
                    break;
                default:
                     printf ("Unknown opcode [0x0000]: 0x%X\n", this->opcode);  
            }
            break;

        //1 part
        case 0x1000:
            printf("JP %x\n", this->opcode & 0x0FFF);
            //printf("%x\n",this->pc); 
            this->pc = this->opcode & 0x0FFF;
            //printf("%x\n",this->pc); 
            break;

        //2 part
        case 0x2000:
            printf("Push pc to stack\n");
            this->stack[this->sp] = this->pc;
            ++this->sp;
            this->pc = this->opcode & 0x0FFF;
            break;
        //3 part
        case 0x3000:
            printf("SE V%x, %x\n", (this->opcode & 0x0F00) >> 8, this->opcode & 0x00FF);
            if(this->V[(this->opcode & 0x0F00) >> 8] == (this->opcode & 0x00FF))
                this->pc +=2;
            this->pc +=2;    
            break;
        //4 part
        case 0x4000:
            printf("SNE V%x, %x\n", (this->opcode & 0x0F00) >> 8, this->opcode & 0x00FF);
            if(this->V[(this->opcode & 0x0F00) >> 8] != (this->opcode & 0x00FF))
                this->pc +=2;
            this->pc +=2;    
            break;
        //5 part
        case 0x5000:
            printf("SE V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
            if(this->V[(this->opcode & 0x0F00) >> 8] == ((this->opcode & 0x00F0) >> 4))
                this->pc +=2;
            this->pc +=2;    
            break;
        //6 part
        case 0x6000:
            printf("LD V%x, %x\n", (this->opcode & 0x0F00) >> 8, this->opcode & 0x00FF);
            this->V[((this->opcode & 0x0F00) >> 8)] = this->opcode & 0x00FF;
            this->pc += 2;
            break;
        //7 part
        case 0x7000:
            printf("ADD V%x, %x\n",(this->opcode & 0x0F00) >> 8, this->opcode & 0x00FF);
            this->V[(this->opcode & 0x0F00) >> 8] += this->opcode & 0x00FF;
            this->pc+=2;
            break;
        //8 part
        case 0x8000:
            switch(this->opcode & 0x000F)
            {
                case 0x0000:
                    printf("LD V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    this->V[(this->opcode & 0x0F00) >> 8] = this->V[(this->opcode & 0x00F0) >> 4];
                    this->pc += 2;
                    break;
                case 0x0001:
                    printf("OR V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    this->V[(this->opcode & 0x0F00) >> 8] = this->V[(this->opcode & 0x0F00) >> 8] | this->V[(this->opcode & 0x00F0) >> 4];
                    this->pc += 2;
                    break;
                case 0x0002:
                    printf("AND V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    this->V[(this->opcode & 0x0F00) >> 8] = this->V[(this->opcode & 0x0F00) >> 8] & this->V[(this->opcode & 0x00F0) >> 4];
                    this->pc += 2;
                    break;
                case 0x0003:
                    printf("XOR V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    this->V[(this->opcode & 0x0F00) >> 8] = this->V[(this->opcode & 0x0F00) >> 8] ^ this->V[(this->opcode & 0x00F0) >> 4];
                    this->pc += 2;
                    break;
                case 0x0004:
                    printf("ADD V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    if(this->V[(this->opcode & 0x00F0) >> 4]> (0xFF - this->V[(this->opcode & 0x0F00) >> 8]))
                        this->V[0xF] = 1; //carry
                    else
                        this->V[0xF] = 0;
                    this->V[(this->opcode & 0x0F00) >> 8] += this->V[(this->opcode & 0x00F0) >> 4] - (0xFF - this->V[(this->opcode & 0x0F00) >> 8]);
                    this->pc += 2;
                    break;
                case 0x0005:
                    printf("SUB V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    if(this->V[(this->opcode & 0x00F0) >> 4] > this->V[(this->opcode & 0x0F00) >> 8])
                        this->V[0xF] = 1; 
                    else
                        this->V[0xF] = 0;
                    this->V[(this->opcode & 0x0F00) >> 8] -= this->V[(this->opcode & 0x00F0) >> 4];
                    this->pc += 2;
                    break;
                case 0x0006:
                     printf("SHR V%x{, V%x}\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    if(this->V[(this->opcode & 0x0F00) >> 8] & 0x0001 == 0x0001)
                        this->V[0xF] = 1;
                    else
                        this->V[0xF] = 0;
                    this->V[(this->opcode & 0x0F00) >> 8] /= 2;
                    this->pc += 2;
                    break;
                case 0x0007:
                     printf("SUBN V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    if(this->V[(this->opcode & 0x00F0) >> 4] < this->V[(this->opcode & 0x0F00) >> 8])
                        this->V[0xF] = 1; 
                    else
                        this->V[0xF] = 0;
                    this->V[(this->opcode & 0x0F00) >> 8] = this->V[(this->opcode & 0x00F0) >> 4] - this->V[(this->opcode & 0x0F00) >> 8];
                    this->pc += 2;
                    break;
                case 0x000E:
                     printf("SHL V%x{, V%x}\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    if((this->V[(this->opcode & 0x0F00) >> 8] & 0x8000)== 0x1000)
                        this->V[0xF] = 1;
                    else
                        this->V[0xF] = 0;
                    this->V[(this->opcode & 0x0F00) >> 8] *= 2;
                    this->pc += 2;
                    break;
                default:
                     printf ("Unknown opcode [0x8000]: 0x%X\n", this->opcode);  
                    break;
            }
            break;
        //9 part
        case 0x9000:
             printf("SNE V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
            if(this->V[(this->opcode & 0x0F00) >> 8] != ((this->opcode & 0x00F0) >> 4))
                this->pc +=2;
            this->pc +=2;    
            break;
        //A part    
        case 0xA000:
            //Execute Opcode
             printf("LD I, %x\n", this->opcode & 0x0FFF);
            this->I = (this->opcode & 0x0FFF);
            this->pc +=2;
            break;
        //B part
        case 0xB000:
            printf("JP V0, %x\n",this->opcode & 0x0FFF);
            this->pc = (this->opcode & 0x0FFF) + this->V[0x0];
            break;
        //C part
        case 0xC000:
            printf("RND V%x, byte\n", this->opcode & 0x0F00 >> 8);
            rnd= this->delay_timer % 0x0100;
            this->V[(this->opcode & 0x00FF) >> 8] = this->opcode & 0x00FF & rnd;
            this->pc +=2;
            break;
        //D part
        case 0xD000:
            printf("DRW V%x, V%x, %xibble\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4, this->opcode & 0x000F);
            
            y = this->V[((this->opcode & 0x00F0) >> 4)];
            x = (y)* (GFX_X-1) + this->V[((this->opcode & 0x0F00) >> 8)] ;
            this->V[0xF] = 0;
            for(int i=0;i<(this->opcode & 0x000F);i++){
                for(int j=0;j<8;j++){
                    //printf("%x\n",(memory[I+i] >> (j+1)) & 1);
                    //printf("%d,%d\n",i,x+y+i*GFX_X+j); 
                    this->gfx[x+y+i*GFX_X+j]=this->gfx[x+y+i*GFX_X+j]^((memory[this->I+i] >> (7-j+1)) & 1);
                    //if(this->gfx[x+y+i*GFX_X+j]!=this->gfx[x+y+i*GFX_X+j]^((memory[this->I+i] >> (7-j+1)) & 1))
                    if(this->gfx[x+y+i*GFX_X+j] == 0)
                        this->V[0xF] = 1;
                    }
            }
            this->pc +=2;
            this->drawFlag = 1; //Set draw flag.
            break;
        //E part
        case 0xE000:
            switch(this->opcode & 0x00FF){
                case 0x009E:
                    printf("SKP V%x\n",(this->opcode & 0x0F00) >> 8);
                    if((this->input - '0' ) == this->V[(this->opcode & 0x0F00) >> 8]){
                        this->pc += 2;
                        this->input = 'G';
                    }
                    this->pc += 2;
                    break;
                case 0x00A1:
                    printf("SKNP V%x\n",(this->opcode & 0x0F00) >> 8);
                    if(this->input != 'G')
                        if((this->input - '0' ) != this->V[(this->opcode & 0x0F00) >> 8]){
                            this->pc += 2;
                            this->input = 'G';
                        }
                    this->pc += 2;
                    break;
            }
            break;
        //F part
        case 0xF000:
            switch(this->opcode & 0x00FF)
            {
                case 0x0007:
                    printf("LD V%x, DT\n",(this->opcode & 0x0F00) >> 8);
                    this->V[(this->opcode & 0x0F00) >> 8] = this->delay_timer;
                    this->pc+=2;
                    break;
                case 0x000A:
                    if(this->input!='G'){
                        printf("Get input %c\n",this->input);
                        this->pc += 2;
                    }
                    break;
                case 0x0015:
                    printf("LD DT, V%x\n",(this->opcode & 0x0F00) >> 8);
                    this->delay_timer = this->V[(this->opcode & 0x0F00) >> 8];
                    this->pc += 2;
                    break;
                case 0x0018:
                    printf("LD ST, V%x\n",(this->opcode & 0x0F00) >> 8);
                    this->sound_timer = this->V[(this->opcode & 0x0F00) >> 8];
                    this->pc += 2;
                    break;
                case 0x001E:
                    printf("ADD I, V%x\n",(this->opcode & 0x0F00) >> 8);
                    this->I += (this->opcode & 0x0F00) >> 8;
                    this->pc += 2;
                    break;
                case 0x0029:
                    printf("LD F, V%x\n", (this->opcode & 0x0F00) >> 8);
                    this->I = this->V[ (this->opcode & 0x0F00) >> 8] * 8;
                    this->pc += 2;
                    break;
                case 0x0033:
                    printf("memory I V\n");
                    this->memory[this->I]     = this->V[(this->opcode & 0x0F00) >> 8] / 100;
                    this->memory[this->I + 1] = (this->V[(this->opcode & 0x0F00) >> 8] / 10) % 10;
                    this->memory[this->I + 2] = (this->V[(this->opcode & 0x0F00) >> 8] % 100) % 10;
                    this->pc += 2;
                    break;
                case 0x0055:
                    printf("LD [I], V%x\n",(this->opcode & 0x0F00) >> 8);
                    for(int i=0;i<=((this->opcode & 0x0F00) >> 8);i++)
                        this->memory[this->I+i] = this->V[i];
                    this->pc+=2;
                    break;
                case 0x0065:
                    printf("LD V%x, [I]\n",(this->opcode & 0x0F00) >> 8);
                    for(int i=0;i<=((this->opcode & 0x0F00) >> 8);i++)
                        this->V[i] = this->memory[this->I+i];
                    this->pc+=2;
                    break;
            }
            break;
        default:
             printf("Unknown opcode 0x%X\n",this->opcode);
    }
    printf("----------------------------------------\n");
    if(this->pc > 0xFFF)
        this->pc = 0x200;
    //Update timers
    //if(this->delay_timer > 0)
    
    if(this->delay_timer == 0)
        this->delay_timer = 60;
    else
        --this->delay_timer;
    //if(this->sound_timer > 0)
    //{
    //    if(this->sound_timer==1)
            //printf("BEEP!\n");
    if(this->sound_timer == 0)
        this->sound_timer = 60;
    else
        --this->sound_timer;            
    //}

}
void Chip8::drawGraphic(){
    int i,j;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)
    glPushMatrix();
    glRotatef(180,0,0,1);
    glRotatef(180,0.0,1.0,0.0);
    glTranslatef(-0.5, -0.5, 0);
    /*system("CLS");
    for(i=0;i<GFX_Y;i++){
        for(j=0;j<GFX_X;j++){
            printf("%d ", this->gfx[i*GFX_X+j]);
        }
        printf("\n");
    }*/
    
    for(i=0;i<GFX_Y;i++){
        glPushMatrix(); 
            //glRotatef(90,0,0,1);
            //glTranslatef(0,,0);
        for(j=0;j<GFX_X;j++){
            if(this->gfx[i*GFX_X+j]==1){
                glPushMatrix();
                    glTranslated((double)j/GFX_X,(double)i/GFX_Y,0);
                    glScalef(0.01, 0.03, 0);
                    glBegin(GL_QUADS);              
                        //glColor3f((double)i/GFX_Y, (double)j/GFX_Y, 1.0f); // 
                        glColor3f(1.0,1.0,1.0); // white
                        glVertex2f(-0.5f, -0.5f);    // x, y
                        glVertex2f( 0.5f, -0.5f);
                        glVertex2f( 0.5f,  0.5f);
                        glVertex2f(-0.5f,  0.5f);
                    glEnd();
                glPopMatrix();
            }
        }
        glPopMatrix();
    }
    glPopMatrix();
    
    glFlush();  // Render now    
    glutSwapBuffers();
    this->drawFlag = 0;
}
void Chip8::printMemory(){
    int i;
    for(i=0;i<MEMORY_SIZE;i++)
        printf("%x ",this->memory[i]);
    printf("\n");
}
#endif