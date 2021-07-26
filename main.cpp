#include "Chip8-Emu.h"
#include "UI.h"
#include <iostream>
#include <string>



/***
* Chip8 emulator requires the size and path of the rom file.
***/
int main(int argc, char* argv[]){
  if (argc < 2) {
    std::cout << "Input Chip8 ROM" << std::endl;
    return -1;
  }

  std::string romPath = argv[1];

  //Create the CHIP8 Emulator
  CHIP8 emuInstant;

  //load the rom
  int status = emuInstant.loadROM(romPath);
  if (status!=0){
    std::cout << "There was an error loading the rom file, Aborting..." << std::endl;
  }

  CHIP8_UI CHIPUI(&emuInstant);

  //Run the emulator
  CHIPUI.run();
  //Will continue running until the user exits the program
}
