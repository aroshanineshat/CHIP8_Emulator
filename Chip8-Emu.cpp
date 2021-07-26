#include "Chip8-Emu.h"


/***
 * 
 * At the start of the emulation, this function will print the status
 * of the rom, the size of the file, the path of the rom.
 * 
 ***/
void CHIP8::printInit(){
  std::cout << "----------------------------------" << std::endl;
  std::cout << "Yet Another CHIP8 Emulator" << std::endl;
  std::cout << "Memory Size:\t" << this->chipMemorySize << std::endl;
  std::cout << "Rom File:\t" << this->romPath << std::endl;
  std::cout << "Rom Size:\t" << this->romSize << " Bytes!" << std::endl;
  std::cout << "----------------------------------" << std::endl;
}

/***
 * Setting the frame buffer data to 0
 * */
void CHIP8::clear_display(){
 for (int x = 0; x < 64; x++){
    for (int y = 0; y < 32; y++){
        frame_buffer[(y * 64) + x] = 0;
    }
  }
}

void CHIP8::initial_display(){
  this->gen.seed(this->rd());
  std::uniform_int_distribution<> distrib(0, 10);

  for (int x = 0; x < 64; x++){
    for (int y = 0; y < 32; y++){
      uint16_t rand_int = distrib(gen);
      frame_buffer[(y * 64) + x] = rand_int % 2;
    }
  }
}

/***
 * Allocating the free space to the memory and loading the rom into it
 * */
int CHIP8::loadROM(std::string romPath){
  std::cout << "Loading the ROM..." << std::endl;
  this->chipMemorySize = CHIP_MEMORY_SIZE;
  //I divide the memory size by 2 because the opcodes are 2 bytes each
  this->memory = new uint8_t[this->chipMemorySize];

  /***
   * next lines are to get the size of the rom
   * */
  this->romPath = romPath;
  std::ifstream romFile (this->romPath, std::ios::binary);
  if (romFile.is_open()){
    std::cout << "ROM opened." << std::endl;
  }
  romFile.seekg(0, std::ios::end);
  this->romSize = romFile.tellg();
  /***
   * reading the binary rom into the memory
   * */
  romFile.seekg(0, std::ios::beg); //go to the beginning of the file
  char *buffer = new char[this->romSize];
  romFile.read(buffer, this->chipMemorySize);
  romFile.close();
  memcpy(this->memory + 0x200, buffer, this->romSize);
  delete[] buffer;
  std::cout << "Loaded the ROM!" << std::endl;
  this->printInit();
  /***
   * Copying the font data into the memory
   * */
  std::cout << "Copying the fonts..." << std::endl;
  memcpy(this->memory, this->fontMem, 80);
  std::cout << "Fonts copied!" << std::endl;
  return 0;
}

CHIP8::CHIP8()  {
  std::cout << "Initializing the CHIP8 emulator ..." <<std::endl;
  //setting all the V registers to 0
  for(int i = 0; i < 16; i++){
    this->V_registers[i] = 0;
  }
  //clearing the display
  this->clear_display();

  //set the program counter to 0x200
  this->pc = PC_INITIAL;
}

//delete the memory and exit
CHIP8::~CHIP8(){
  delete[] this->memory;
}

void CHIP8::stepPC(){
  this->pc += PC_STEP;
}

int CHIP8::run(){

    uint16_t opcode = (this->memory[this->pc] << 8) | (this->memory[this->pc+1]); //getting current opcode

    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t kk = (opcode & 0x00FF);
    uint16_t nnn = (opcode & 0x0FFF);

    std::cout << this->pc << "-" << std::hex << opcode << std::endl;

    uint16_t opcode_temp = opcode;
    if ((opcode & 0xF000) >> 12 == 0){
      if (nnn != 0x0E0 && nnn != 0x0EE) {
        opcode_temp = opcode_temp & 0xF000;
      }
    }else
    if ((opcode & 0xF000) >> 12 == 0xF){
        opcode_temp = opcode_temp & 0xF0FF;
    }else
    if ((opcode & 0xF000) >> 12 == 0xE){
        opcode_temp = opcode_temp & 0xF0FF;
    }else if ((opcode & 0xF000) >> 12 == 0xD){
        opcode_temp = opcode_temp & 0xF000;
    } else {
       opcode_temp = opcode_temp & 0xF00F;
    }

    bool opcode_exists = false;
    for (auto &i : this->Opcode_list){
      if (i == opcode_temp) {
        opcode_exists = true;
      }
    }
    if (opcode_exists == false){
      Opcode_list.push_back(opcode_temp);
    }

    switch ((opcode & 0xF000) >> 12) 
    {
      case (0):{
        //0x0NNN is ignored
        switch(nnn){
          case (0x0E0): { // clear the display
            this->clear_display();
            this->stepPC();
            break;
          }
          case (0x0EE): { // get front of the stack pointer and removes it from the stack 
            this->pc = this->stack_ptr[0];
            this->stack_ptr.erase(this->stack_ptr.begin() + 0);
            break;
          }
          default:{
            this->pc = nnn;
            break;
          }
        }
        break;
      } // 0x0E0
      case (1): { //JMP addr //Tested
        //1nnn will set the pc to nnn
        this->pc = nnn;
        break;
      } //0x1nnn
      case (2): { //CALL addr //Tested
        //puts the current pc + 1 in the stack pointer and then calls
        //nnn
        this->stepPC();
        this->stack_ptr.push_back(this->pc);
        this->pc = nnn;
        break;
      } //0x2nnn
      case (3): { // 3xkk, SE Vx, byte //Tested
        //if  Vx == kk, pc = pc + 2
        //std::cout << "--->>>Vx = " << (int)this->V_registers[x] << std::endl;
        if (this->V_registers[x] == kk) {
          this->stepPC();
        }
        this->stepPC();
        break;
      } //0x3xkk
      case (4): { //4xkk - SNE Vx, byte
        //if Vx != kk, pc = pc + 2
        if (this->V_registers[x] != kk) {
          this->stepPC();
        }
        //std::cout << "V" << (int)x << "=" << (int)this->V_registers[x] << std::endl;
        //std::cout << "kk=" << (int)kk << std::endl;
        this->stepPC();
        break;
      } //0x4xkk
      case (5): { //5xy0 - SE Vx, Vy
        //if Vx == Vy, pc = pc+2
        if (this->V_registers[x] == this->V_registers[y]) {
          this->stepPC();
        }
        this->stepPC();
        break;
      } //0x5xy0
      case (6): { //6xkk - LD Vx, byte //Tested
        //put kk in Vx
        this->V_registers[x] = kk;
        this->stepPC();
        break;
      } //0x6xkk
      case (7): { //7xkk - ADD Vx, byte //Tested
        //Vx = Vx + kk
        this->V_registers[x] = (this->V_registers[x] + kk) % 256;
        this->stepPC();
        break;
      } //0x7xkk
      case (8): {
        char oper = opcode & 0x000F;
        std::cout << "oper is " << std::hex << (int)oper<<std::endl;

        switch (oper)
        { 
          case (0):{//8xy0 - LD Vx, Vy
            // Vx = Vy
            this->V_registers[x] = this->V_registers[y];
            //std::cout << "--->>>Vx = " << (int)this->V_registers[x] << ",Vy = " << (int)this->V_registers[y] << std::endl;
            break;
          }
          case (1):{//8xy1 - OR Vx, Vy
            // bitwise or of Vx, Vy
            this->V_registers[x] |= this->V_registers[y];
            break;
          }
          case (2): {//8xy2, AND Vx, Vy
            //bitwise AND of Vx, Vy
            this->V_registers[x] &= this->V_registers[y];
            break;
          }
          case (3): {//8xy3 - XOR Vx, V
            //bitwise Xor of Vx, Vy
            this->V_registers[x] ^= this->V_registers[y];
            break;
          }
          case (4): { // 8xy4 - ADD Vx, Vy
            //Add two Vx and Vy. store carry at VF
            

            int Vx_t = (int)this->V_registers[x];
            int Vy_t = (int)this->V_registers[y];
            
            Vx_t = Vx_t + Vy_t;
            //0xFF
            //0b000000000
            this->V_registers[x] = Vx_t & 0xFF; // 8bits
            this->V_registers[0xF] = ((int)(Vx_t & 0x100)) >> 8; // carry
            
            break;
          }
          case (5): { //8xy5 - SUB Vx, Vy //Tested
            //Set Vx = Vx - Vy, set VF = NOT borrow.
            //If Vx > Vy, then VF is set to 1, otherwise 0. 
            //Then Vy is subtracted from Vx, and the results stored in Vx.
            int Vx_t = (int)this->V_registers[x];
            int Vy_t = (int)this->V_registers[y];
            std::cout << "V" << x << "is " << std::hex << Vx_t<<std::endl;
            std::cout << "V" << y << "is " << std::hex << Vy_t<<std::endl;
            Vx_t = Vx_t - Vy_t;
            this->V_registers[0xF] = (Vx_t>=Vy_t)? 1:0;
            //this->V_registers[0xF] = (this->V_registers[x]>this->V_registers[y])? 1:0;
            //this->V_registers[x] -= this->V_registers[y]; 
            this->V_registers[x] = Vx_t & 0xFF; // 8bits
            std::cout << "V" << x << "is " << std::hex << (int)this->V_registers[x]<<std::endl;
            std::cout << "V" << 0xF << "is " << std::hex << (int)this->V_registers[0xF]<<std::endl;
            break;
          }
          case (6): { 
            /***
             * 8xy6 - SHR Vx {, Vy}
             * Set Vx = Vy SHR 1.
             * If the least-significant bit of Vy is 1, 
             * then VF is set to 1, otherwise 0.
             * */
            this->V_registers[0xF] = this->V_registers[y] & 0x1;
            this->V_registers[x] = V_registers[y] >> 1;
            break;
          }
          case (7): { //8xy7
            /***
             * Set register VX to the value of VY minus VX
             * If Vy > Vx, then VF is set to 1, otherwise 0. 
             * Then Vx is subtracted from Vy, and the results stored in Vx.
             * */
            int Vx_t = (int)this->V_registers[x];
            int Vy_t = (int)this->V_registers[y];
            Vy_t = Vy_t - Vx_t;
            this->V_registers[0xF] = (Vy_t>=Vx_t)? 1:0;
            //this->V_registers[0xF] = (this->V_registers[x]>this->V_registers[y])? 1:0;
            //this->V_registers[x] -= this->V_registers[y]; 
            this->V_registers[x] = Vy_t & 0xFF; // 8bits

            //this->V_registers[0xF] = (this->V_registers[y]>this->V_registers[x])? 1:0;
            //std::cout <<"->"<< std::hex << this->V_registers[y] << std::endl;
            //std::cout <<"->"<< std::hex << this->V_registers[x] << std::endl;
            //this->V_registers[x]   = this->V_registers[y] - this->V_registers[x]; 
            //std::cout <<"->"<< std::hex << this->V_registers[y] << std::endl;
            break;
          }
          case (0xE): {
            /***
             * Store the value of register VY shifted left one bit in register VX
             * Set register VF to the most significant bit prior to the shift
             * VY is unchanged
             * *///0b00000000
            this->V_registers[0xF] = (this->V_registers[y] & 0x80) >> 7;
            this->V_registers[x] = V_registers[y] << 1;
            break;
          }
        }
        this->stepPC();
        break;
      }
      case (9): { //9xy0
        /***
         * Skip the following instruction if 
         * the value of register Vx is not equal to the value of register VY 
         * */
        if (this->V_registers[x] != this->V_registers[y]){
          this->stepPC();
        }
        this->stepPC();
        break;
      } 
      case (0xA):{ //Tested
        /***
         * Store memory address NNN in register I
         * */
        this->index_register = nnn;
        this->stepPC();
        break;
      }
      case (0xB):{ //Tested
        /***
         * Jump to location nnn + V0.
         * The program counter is set to 
         * nnn plus the value of V0.
         * */
        this->pc = this->V_registers[0] + nnn;
        break;
      }
      case (0xC):{ //Tested
        /***
         * Cxkk - RND Vx, byte
         * Set Vx = random byte AND kk.
         * The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx.
         * */
        this->gen.seed(this->rd());
        std::uniform_int_distribution<> distrib(0, 255);
        uint16_t rand_int = distrib(gen);
        this->V_registers[x] = rand_int & kk;
        this->stepPC();
        break;
      }
      case (0xD): { //0xDxyn
        //Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
        char n = (opcode & 0x000F);
        
        int Vx = (int)this->V_registers[x];
        int Vy = (int)this->V_registers[y];
        this->V_registers[0xf] = 0x0;
        std::cout << "--->>>Vx = " << (int)Vx << ",Vy = " << (int)Vy << std::endl;
        for (int i = 0; i<n; i++){
          uint8_t ByteinI = this->memory[this->index_register+i];
          for (int Horizontal_index=0; Horizontal_index<8; Horizontal_index++){
            uint8_t bit = ((ByteinI & (0x1 << (7-Horizontal_index))) >> (7-Horizontal_index));
            uint16_t Hl = ((Vy+i)%32) * 64 + ((Vx + Horizontal_index) % 64);
            uint8_t Before_=this->frame_buffer[Hl] & 0x1 ;
            this->frame_buffer[Hl] = this->frame_buffer[Hl] ^ bit;
            if (Before_ == 1 &&  this->frame_buffer[Hl] == 0){ //Collision Detection
              this->V_registers[0xf] |= 0x1;
            }
          }
        }

        this->stepPC();
        break;
      } //0xDxkn
      case (0xE):{
        switch (kk){
          case (0x9E):{
            /***
             * Skip the following instruction if the key corresponding to 
             * the hex value currently stored in register VX is pressed
             * */
            if (this->V_registers[x] == this->userInput){
              this->stepPC();
            }
            this->stepPC();
            break;
          }
          case (0xA1): {
            /***
             * Skip the following instruction if the key corresponding to
             *  the hex value currently stored in register VX is not pressed
             * */
            if (this->V_registers[x] != this->userInput){
              this->stepPC();
            }
            this->stepPC();
            break;
          }
          break;
        }
        break;
      } //0xE
      case (0xF):{
        switch (kk){
          case (0x07):{
            //Store the current value of the delay timer in register VX
            
            this->V_registers[x] = this->delayTime;
            //std::cout << "--->>>Vx = " << (int)this->V_registers[x] << std::endl;
            //std::cout << "--->>>Delay Timer = " << (int)this->delayTime << std::endl;
            this->stepPC();
            break;
          }
          case (0x0A):{
            //Wait for a keypress and store the result in register VX 
            if (this->userInput != 0xF0){
              this->V_registers[x] = this->userInput;
              this->stepPC();
            }
            break;
          }
          case (0x15):{
            //Set the delay timer to the value of register VX 
            this->delayTime = this->V_registers[x];
            this->stepPC();
            break;
          }
          case (0x18):{
            //Set the sound timer to the value of register VX 
            this->soundTime = this->V_registers[x];
            this->stepPC();
            break;
          }
          case (0x1E):{
            //Add the value stored in register VX to register I
            this->index_register += this->V_registers[x];
            this->stepPC();
            break;
          }
          case (0x29):{ //0xFx29 //Tested
            //Set I = location of sprite for digit Vx
            this->index_register = this->V_registers[x] * 5;
            this->stepPC();
            break;
          }
          case (0x33):{ //0xFx33 //Tested
            //Store BCD representation of Vx in memory 
            //locations I, I+1, and I+2.
            int BCD = (int)this->V_registers[x];
            this->memory [this->index_register + 0] = (int)(BCD/100);
            BCD = BCD %100;
            this->memory [this->index_register + 1] = (int)(BCD/10);
            BCD = BCD %10;
            this->memory [this->index_register + 2] = BCD;
            this->stepPC();
            break;
          }
          case (0x55):{ //0xFx55
            //Store the values of registers V0 to VX 
            //inclusive in memory starting at address I
            //I is set to I + X + 1 after operation
            for (int i=0; i<=x;i ++){
              this->memory[this->index_register + i] = this->V_registers[i];
            }
            this->index_register += x + 1;
            this->stepPC();
            break;
          }
          case (0x65):{
            /***
             * Fill registers V0 to VX inclusive with 
             * the values stored in memory starting at address I
             * I is set to I + X + 1 after operation
             * */
            for (int i=0; i<=x;i ++){
              this->V_registers[i] = this->memory[this->index_register + i];
            }
            this->index_register += x + 1;
            this->stepPC();
            break;
          }
        }
        break;
      }
    }
    this->updateUI = true;
    this->userInput = 0xF0;
    this->delayTime --;
    if (delayTime < 0){
      delayTime = 0;
    }
    this->soundTime --;
    if (soundTime < 0){
      soundTime < 0;
    }
  return 0;
}
