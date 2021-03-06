#ifndef C8_IMP_H
#define C8_IMP_H
Chip8::Chip8(){
    int i,j;
    //Initialize memory and registers once.
    this->pc = 0x200;         //Program counter start at 0x200
    this->opcode = 0;         //Reset current opcode
    this->I = 0;              //Reset Index Register
    this->sp = 0;             //Rest Stack Pointer
    this->drawFlag = 0;
    this->input = 'G';
	srand(234);
	
    //Clear display
    for(i=0;i<GFX_Y;i++)
        for(j=0;j<GFX_X;j++)
            this->gfx[i][j] = 0;

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
    for(i=0;i<80;++i)
        this->memory[i]=chip8_fontset[i];

    //Rest timers
    this->delay_timer = DELAY_TIMER_RATE - 1;
    this->sound_timer = SOUND_TIMER_RATE - 1;
}
bool Chip8::loadGame(char* filename){
	
    printf("Loading Game : %s...",filename);
    //Open the code file in binary.
    FILE *f = fopen(filename,"rb");

    if(!f){
        printf("open failure!\n");  //Open failure.
		return false;
	}
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
		printf("Done!\n");
		return true;
    }
}
void Chip8::emulateCycle(){
    int g_x,g_y;
    unsigned char rnd,x,y;
	unsigned char memory_tmp;
	int erase,n;
	char move;
    //Fetch Opcode
    
    this->opcode = this->memory[this->pc] << 8 | this->memory[this->pc+1];
	x = (this->opcode & 0x0F00) >> 8;
	y = (this->opcode & 0x00F0) >> 4;
    //showRegisters();
    //Decode Opcode
    switch(this->opcode & 0xF000)
    {
        //0 part
        case 0x0000: 
            switch(this->opcode & 0x000F)
            {
                case 0x0000: //0x00E0 Clear the screen
                    //Execute Opcode
                    // printf("Clear the screen\n");
                    //Clear display
                    for(int i=0;i<GFX_Y;i++)
                        for(int j=0;j<GFX_X;j++)
                            this->gfx[i][j] = 0;
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    //this->drawFlag = 1;
                    this->pc += 2;

                    break;
                case 0x000E: // 0x00EE: Returns from subroutine
                    // Execute opcode
                    //printf("Returns from subroutine\n");
                    this->pc = this->stack[this->sp-1] + 2;
                    this->sp--;
                    break;
                default:
                    printf ("Unknown opcode [0x0000]: 0x%X\n", this->opcode); 
					break;					 
            }
            break;

        //1 part
        case 0x1000:
            //printf("JP %x\n", this->opcode & 0x0FFF);
            this->pc = this->opcode & 0x0FFF;
            break;

        //2 part
        case 0x2000:
            //printf("Push pc to stack\n");
            this->stack[this->sp] = this->pc;
            ++this->sp;
            this->pc = this->opcode & 0x0FFF;
            break;
        //3 part
        case 0x3000:
            //printf("SE V%x, %x\n", (this->opcode & 0x0F00) >> 8, this->opcode & 0x00FF);
            this->pc += 2*((this->V[x] == (this->opcode & 0x00FF))+1);    
            break;
        //4 part
        case 0x4000:
            //printf("SNE V%x, %x\n", (this->opcode & 0x0F00) >> 8, this->opcode & 0x00FF);
            this->pc += 2*((this->V[x] != (this->opcode & 0x00FF))+1);    
            break;
        //5 part
        case 0x5000:
            //printf("SE V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
            this->pc += 2*((this->V[x] == this->V[y])+1);    
            break;
        //6 part
        case 0x6000:
            //printf("LD V%x, %x\n", (this->opcode & 0x0F00) >> 8, this->opcode & 0x00FF);
            this->V[x] = (this->opcode & 0x00FF);
            this->pc += 2;
            break;
        //7 part
        case 0x7000:
            //printf("ADD V%x, %x\n",(this->opcode & 0x0F00) >> 8, this->opcode & 0x00FF);
            this->V[x] += (this->opcode & 0x00FF);
            this->pc+=2;
            break;
        //8 part
        case 0x8000:
            switch(this->opcode & 0x000F)
            {
                case 0x0000:
                    //printf("LD V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    this->V[x] = this->V[y];
                    this->pc += 2;
                    break;
                case 0x0001:
                    //printf("OR V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    this->V[x] = this->V[x] | this->V[y];
                    this->pc += 2;
                    break;
                case 0x0002:
                    //printf("AND V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    this->V[x] = this->V[x] & this->V[y];
                    this->pc += 2;
                    break;
                case 0x0003:
                    //printf("XOR V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    this->V[x] = this->V[x] ^ this->V[y];
                    this->pc += 2;
                    break;
                case 0x0004:
                    //printf("ADD V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    if(this->V[y]> (0xFF - this->V[x])){
                        this->V[0xF] = 1; //carry
						this->V[x] = this->V[y] - (0x100 - this->V[x]);
					}
                    else{
                        this->V[0xF] = 0;
						this->V[x] += this->V[y]; // - (0xFF - this->V[(this->opcode & 0x0F00) >> 8]);
                    }
					this->pc += 2;
                    break;
                case 0x0005:
                    //printf("SUB V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
                    if(this->V[x] > this->V[y]){
                        this->V[0xF] = 1; 
						this->V[x] -= this->V[y];
					}
                    else{
                        this->V[0xF] = 0;
						this->V[x] += (0x100 - this->V[y]);
					}
                    //printf("%x\n",this->V[(this->opcode & 0x0F00) >> 8]);
					this->pc += 2;
                    break;
                case 0x0006:
                    // printf("SHR V%x{, V%x}\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
					this->V[0xF] = ((this->V[x] & 0x0001) == 0x0001) ;
					printf("here\n");
                    this->V[x] /= 2;
                    this->pc += 2;
                    break;
                case 0x0007:
                    // printf("SUBN V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
					this->V[0xF] = (this->V[y] < this->V[x]);
                    this->V[x] = this->V[y] - this->V[x];
                    this->pc += 2;
                    break;
                case 0x000E:
                    // printf("SHL V%x{, V%x}\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
					this->V[0xF] = ((this->V[x] & 0x0080)== 0x0080);
                    this->V[x] *= 2;
                    this->pc += 2;
                    break;
                default:
                    printf ("Unknown opcode [0x8000]: 0x%X\n", this->opcode);  
                    break;
            }
            break;
        //9 part
        case 0x9000:
           //  printf("SNE V%x, V%x\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4);
            if(this->V[x] != this->V[y])
                this->pc +=2;
            this->pc +=2;    
            break;
        //A part    
        case 0xA000:
            //Execute Opcode
            // printf("LD I, %x\n", this->opcode & 0x0FFF);
            this->I = (this->opcode & 0x0FFF);
            this->pc +=2;
            break;
        //B part
        case 0xB000:
            //printf("JP V0, %x\n",this->opcode & 0x0FFF);
            this->pc = (this->opcode & 0x0FFF) + this->V[0x0];
            break;
        //C part
        case 0xC000:
            //printf("RND V%x, byte\n", this->opcode & 0x0F00 >> 8);
            rnd= rand() & 0x00FF;
			//printf("%d\n",rnd);
            this->V[x] = (this->opcode & 0x00FF) & rnd;
            this->pc +=2;
            break;
        //D part
        case 0xD000:
            //printf("DRW V%x, V%x, %xibble\n", (this->opcode & 0x0F00) >> 8, (this->opcode & 0x00F0) >> 4, this->opcode & 0x000F);
            
            g_x = this->V[y];
            g_y = this->V[x];
			
            this->V[0xF] = 0;
			n = (this->opcode & 0x000F);
            for(int i=0;i<n;++i){
				memory_tmp = memory[this->I+i];
                for(int j=7;j>=0;--j){ 
					int tmpy = ((g_y+j)>GFX_X)?(g_y+j-GFX_X-1):(g_y+j);
					//tmpy = (tmpy<0)?(GFX_X+tmpy):tmpy;
					erase = this->gfx[g_x+i][tmpy]^( memory_tmp & 1);
                    switch(this->gfx[g_x+i][tmpy] - erase){
						case 1:
							this->V[0xF] = 1;
							break;
					}
					this->gfx[g_x+i][tmpy] = erase;
					memory_tmp = memory_tmp >> 1;
				}
            }
            this->pc +=2;
            this->drawFlag = 1; //Set draw flag.
            break;
        //E part
        case 0xE000:
			move = this->input;
            switch(this->opcode & 0x00FF){
                case 0x009E:
					switch(move){
						case 0x0:
						case 0x1:
						case 0x2:
						case 0x3:
						case 0x4:
						case 0x5:
						case 0x6:
						case 0x7:
						case 0x8:
						case 0x9:
						case 0xa:
						case 0xb:
						case 0xc:
						case 0xd:
						case 0xe:
						case 0xf:
							if(move == this->V[x]){
								//printf("SE Move %c\n",this->input);
								this->pc += 2;
								this->input = 'G';
							}
							break;
						default:
							break;
					}
					this->pc += 2;
                    break;
                case 0x00A1:
                    switch(move){
						case 0x0:
						case 0x1:
						case 0x2:
						case 0x3:
						case 0x4:
						case 0x5:
						case 0x6:
						case 0x7:
						case 0x8:
						case 0x9:
						case 0xa:
						case 0xb:
						case 0xc:
						case 0xd:
						case 0xe:
						case 0xf:
							if(move != this->V[x]){
								this->pc += 2;
								
							}
							else
								this->input = 'G';
							break;
						default:
							this->pc += 2;
							break;
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
                    //printf("LD V%x, DT\n",(this->opcode & 0x0F00) >> 8);
                    this->V[x] = this->delay_timer;
                    this->pc+=2;
                    break;
                case 0x000A:
					//printf("Getting\n");
                    if(this->input!='G'){
                        //printf("Get input %c\n",this->input);
						this->V[x] = this->input - '0';
                        this->pc += 2;
                    }
                    break;
                case 0x0015:
                    //printf("LD DT, %d\n",this->V[(this->opcode & 0x0F00) >> 8]);
                    this->delay_timer = this->V[x] ;
                    this->pc += 2;
                    break;
                case 0x0018:
                    //printf("LD ST, V%x\n",(this->opcode & 0x0F00) >> 8);
                    this->sound_timer = this->V[x] ;
                    this->pc += 2;
                    break;
                case 0x001E:
                    //printf("ADD I, V%x\n",(this->opcode & 0x0F00) >> 8);
                    this->I += this->V[x];
                    this->pc += 2;
                    break;
                case 0x0029:
                    //printf("LD F, V%x\n", (this->opcode & 0x0F00) >> 8);
                    this->I = this->V[x] * 5;
                    this->pc += 2;
                    break;
                case 0x0033:
                    //printf("memory I V\n");
                    this->memory[this->I]     = this->V[x] / 100;
                    this->memory[this->I + 1] = (this->V[x] / 10) % 10;
                    this->memory[this->I + 2] = (this->V[x] % 100) % 10;
                    this->pc += 2;
                    break;
                case 0x0055:
                    //printf("LD [I], V%x\n",(this->opcode & 0x0F00) >> 8);
                    for(int i=0;i<=x;i++)
                        this->memory[this->I+i] = this->V[i];
                    this->pc+=2;
                    break;
                case 0x0065:
                    //printf("LD V%x, [I]\n",(this->opcode & 0x0F00) >> 8);
                    for(int i=0;i<=x;i++)
                        this->V[i] = this->memory[this->I+i];
                    this->pc+=2;
                    break;
            }
            break;
        default:
             printf("Unknown opcode 0x%X\n",this->opcode);
			 break;
    }
	
    //Update timers
    switch(this->delay_timer){
		case 0:
			this->delay_timer = DELAY_TIMER_RATE;
			break;
		default:
			--this->delay_timer;
			break;
	}
	switch(this->sound_timer){
		case 0:
			this->sound_timer = DELAY_TIMER_RATE;
			break;
		default:
			--this->sound_timer;
			break;
	}

}
void Chip8::drawGraphic(){
    int i,j;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // Clear the color buffer (background)
    
	glColor3f(1.0,1.0,1.0); // white
	glPushMatrix();
		glRotatef(180,0,0,1);
		glRotatef(180,0.0,1.0,0.0);
		glTranslatef(-0.5, -0.5, 0);
		
		for(i=0;i<GFX_Y;i++){
			for(j=0;j<GFX_X;j++){
				glPushMatrix();
					glTranslated((double)j/GFX_X,(double)i/GFX_Y,0);
					glCallList(this->gfx[i][j]+1);
				glPopMatrix();
			}
		}
    glPopMatrix();
    
    //glFlush();  // Render now    
    this->drawFlag = 0;
}
void Chip8::showRegisters(){
	printf("----------------------------------------\n");
    printf("pc:%x\n",this->pc);
    printf("delay_timer:%d\n",this->delay_timer);
    printf("sound_timer:%d\n",this->sound_timer);
	printf("I:%x\n",this->I);
	printf("V: ");
	for(int i=0;i<16;i++)
		printf("%x ", this->V[i]);
    printf("\nopcode:%x \n",this->opcode);
	//printMemory();
}
void Chip8::printMemory(){
    int i;
    for(i=0;i<MEMORY_SIZE;i++)
        printf("%x ",this->memory[i]);
    printf("\n");
}
#endif