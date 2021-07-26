#include "UI.h"

CHIP8_UI::CHIP8_UI(CHIP8* Inst){
    std::cout << "Initializing the User Interface ..." << std::endl;
    this->Inst = Inst;   
    this->pixelData = new sf::Uint8 [X_SIZE * Y_SIZE * 4];
    texture.create (X_SIZE, Y_SIZE);
    std::cout << "User Interface initialized!" << std::endl;
    graphic_sprite.scale(TEXTURE_SCALE,TEXTURE_SCALE);

    //load Sound Buffer
    if (!this->buffer.loadFromFile("beep.wav")){
        std::cout << "Sound File Not Loaded!" << std::endl;
    }else{
        std::cout << "Sound File Loaded!" << std::endl;
        this->sound.setBuffer(this->buffer);
    }

    for (int i = 0; i< 63; i++){
        this->line[i*2] = sf::Vertex(sf::Vector2f((i+1) * TEXTURE_SCALE, 0), gridColor);
        this->line[i*2 + 1] = sf::Vertex(sf::Vector2f((i+1) * TEXTURE_SCALE, Y_SIZE * TEXTURE_SCALE),gridColor);
    }
    for (int i = 0; i< 32; i++){
        this->line[i*2 + 63*2] = sf::Vertex(sf::Vector2f(0, (i+1) * TEXTURE_SCALE), gridColor);
        this->line[i*2 + 1 + 63*2] = sf::Vertex(sf::Vector2f(X_SIZE * TEXTURE_SCALE, (i+1) * TEXTURE_SCALE), gridColor);
    }

}

void CHIP8_UI::update_pixelData(){
    for (int y = 0; y< Y_SIZE; y++){
        for (int x=0; x< X_SIZE; x++){
            int index = y * X_SIZE + x;

            int R, G, B, A = 255;
            if (this->Inst->frame_buffer[index] == 1){
                R = this->foreground_c.r;
                G = this->foreground_c.g;
                B = this->foreground_c.b;
            }
            else if (this->Inst->frame_buffer[index] == 0){
                R = this->background_c.r;
                G = this->background_c.g;
                B = this->background_c.b;
            }
            else {
                std::cout << "error" << std::endl;
            }

            this->pixelData[index*4 + 0] = R;
            this->pixelData[index*4 + 1] = G;
            this->pixelData[index*4 + 2] = B;
            this->pixelData[index*4 + 3] = A;

        }
    }
}

CHIP8_UI::~CHIP8_UI(){
    std::cout << "---------------------" << std::endl;
    for (auto &i: this->Inst->Opcode_list){
        std::cout << std::hex << (int)i<< std::endl;
    }
    delete [] this->pixelData;
}

void CHIP8_UI::run(){
    sf::RenderWindow window (sf::VideoMode(X_SIZE * TEXTURE_SCALE, Y_SIZE * TEXTURE_SCALE), "CHIP 8");
    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            this->sleeping_time = 500;
        }
        
        /***
         * Polling Pressed Key
         * */
        for (int i = 0;i < 16; i++){
            if (sf::Keyboard::isKeyPressed(this->Keymap[i])){
                    this->Inst->userInput = this->Keycodes[i];
            }
        }
        this->Inst->run();
        if (this->Inst->updateUI == true){
            window.clear();
            this->update_pixelData();
            texture.update(this->pixelData);
            graphic_sprite.setTexture(this->texture);
            window.draw(graphic_sprite);
            window.draw(this->line,188, sf::Lines);
            window.display();
            this->Inst->updateUI = false;
        }
        if (this->Inst->soundTime == 0){
            this->sound.play();
        }
        sf::sleep(sf::milliseconds(this->sleeping_time));
    }
}