#include <SFML/Graphics.hpp>
#include "State.h"
#include "Timer.h"
#include <iostream>

int main()
{

    Timer* t = new Timer("Load");

    State superState;
    State* currentState = &superState;
    
    delete t;
    
    sf::RenderWindow window(sf::VideoMode(300, 300), "SFML works!");
    sf::RectangleShape shape({ 100.f, 100.f });
    sf::Texture tex; tex.loadFromFile("Assets\\Pieces.png");
    sf::Sprite piece(tex);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2i pos = sf::Mouse::getPosition(window) / 100;
                int square = 3 * pos.y + pos.x;
                std::cout << square;
                if (currentState->m_Board.IsAvailable(square) && currentState->m_Board.LoadEval() == 0)
                {
                    currentState = &State::boards.at(currentState->m_Board.MakeMove(square));
                }
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::R)
                    currentState = &superState;
            }
        }

        window.clear();
        for (int j = 0; j < 9; j++)
        {
            if (currentState->m_Board.x[j])
            {
                piece.setTextureRect({ 0, 0, 100, 100 });
                piece.setPosition({ 100 * float(j % 3), 100 * float(j / 3) });
                window.draw(piece);
            }
            else if (currentState->m_Board.o[j])
            {
                piece.setTextureRect({ 100, 0, 100, 100 });
                piece.setPosition({ 100 * float(j % 3), 100 * float(j / 3) });
                window.draw(piece);
            }
            else
            {

                if (currentState->m_Board.LoadEval() != 0 || !State::boards.count(currentState->m_Board.MakeMove(j))) continue;

                int val = State::boards.at(currentState->m_Board.MakeMove(j)).m_Value;

                if (val == (currentState->m_Board.m_Turn ? 1 : -1))
                    shape.setFillColor({ 0, 255, 0 });
                else if (val == (currentState->m_Board.m_Turn ? -1 : 1))
                    shape.setFillColor({ 255, 0, 0 });
                else shape.setFillColor({ 127, 127, 127 });
                
                shape.setPosition({ 100 * float(j % 3), 100 * float(j / 3) });
                window.draw(shape);
            }

        }
        window.display();
    }

    return 0;
}
