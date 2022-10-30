#include <iostream>
#include "Submem.h"

int main() {
    Submem game;

    // Optimal for standard monitors:
    //game.create(720, 480, SubmemConsts::PROJECT_NAME, true);
    //game.create(1920, 1080, SubmemConsts::PROJECT_NAME, true);
    game.create(1920, 1080, SubmemConsts::PROJECT_NAME, true, Consts::BACKGROUND_COLOR, sf::Style::Fullscreen);

    // Optimal for MacBook Pro 16 display:
    //game.create(2048, 1152, SubmemConsts::PROJECT_NAME, true, Consts::BACKGROUND_COLOR);
    //game.create(3072, 1920, SubmemConsts::PROJECT_NAME, true, Consts::BACKGROUND_COLOR, sf::Style::Fullscreen);

    return 0;
}
