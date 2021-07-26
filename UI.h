/*
* Project: CHIP8 Emulator's UI
* Author: Arash Roshanineshat
* Date  : July 22th, 2021
*/
#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include "Chip8-Emu.h"
#include <iostream>


#define TEXTURE_SCALE 20
#define X_SIZE 64
#define Y_SIZE 32

class CHIP8_UI{

private:

    sf::Keyboard::Key Keymap[16] = {
        sf::Keyboard::Num1, sf::Keyboard::Num2, sf::Keyboard::Num3, sf::Keyboard::Num4,
        sf::Keyboard::Q, sf::Keyboard::W, sf::Keyboard::E, sf::Keyboard::R,
        sf::Keyboard::A, sf::Keyboard::S, sf::Keyboard::D, sf::Keyboard::F,
        sf::Keyboard::Z, sf::Keyboard::X, sf::Keyboard::C, sf::Keyboard::V
    }; 
    
    uint8_t Keycodes[16] = {
        0x1, 0x2, 0x3, 0xC,
        0x4, 0x5, 0x6, 0xD,
        0x7, 0x8, 0x9, 0xE,
        0xA, 0x0, 0xB, 0xF,
    };
    
    sf::Uint8* pixelData;
    void update_pixelData();

    sf::Color background_c = sf::Color(22, 22, 22,255);
    sf::Color foreground_c = sf::Color(244, 204, 164, 255);
    sf::Color gridColor = sf::Color(255,255,255, 25);

    sf::Texture texture;
            sf::Sprite graphic_sprite;

    CHIP8 *Inst;

    sf::SoundBuffer buffer;
    sf::Sound sound;

    sf::Vertex line[188];

    int sleeping_time = 16;

public:
    void run();
    CHIP8_UI(CHIP8*);
    ~CHIP8_UI();

};


