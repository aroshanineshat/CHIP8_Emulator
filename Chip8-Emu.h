/*
* Project: CHIP8 Emulator
* Author: Arash Roshanineshat
* Date  : July 18th, 2021
*/
#pragma once
#include <iostream>
#include <cmath>
#include <string.h>
#include <array>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <random>


#define CHIP_MEMORY_SIZE 4096
#define PC_INITIAL 0x0200
#define FONT_ADDRESS 0x0000
#define PC_STEP 2

class CHIP8 {

private:

  /***
   * Font memory template
   * */

  uint8_t fontMem[80] = 
  {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
  };

  /***
   * ROM variables
   ***/
  std::string romPath;
  int romSize;

  /***
   * Chip8 memory variables
   ***/
  uint16_t chipMemorySize;
  uint8_t *memory;
  
  std::array<uint8_t, 16> V_registers;
 
  //Address register
  //although it's 16bit wide, only 12 bit of it will be used
  //because of the size of the CHIP8 memory 
  uint16_t index_register;

  //Program Counter
  uint16_t pc;

  
  //Stack pointer register
  //I'm using vector so that it can support *almost* unlimited stacks
  //according the documetation, it should at least support 12 stacks
  std::vector <uint16_t> stack_ptr;
 


  //Random value generator
  std::random_device rd;
  std::mt19937 gen;

  int delayTime = 0;

  void stepPC();


//private routines
private:
  void clear_display(); //Clear the display
  void printInit(); //Print the status
  void initial_display();

public:
  int soundTime = 0;
  //FrameBuffer
  //Original CHIP8 used 64x32 frame buffer
  uint8_t frame_buffer[64 * 32];

  //Keyboard variables
  uint8_t userInput = 0xF0;

  //Should update the display
  uint8_t updateUI = false;

  std::vector <uint16_t> Opcode_list;

  int loadROM(std::string romPath);
  CHIP8();
  ~CHIP8();
  int run();
   
};
