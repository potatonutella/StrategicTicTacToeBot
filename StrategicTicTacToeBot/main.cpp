#include <SFML/Graphics.hpp>
#include "State.h"
#include "Timer.h"
#include "Settings.h"
#include <iostream>


#ifdef DISABLE_CONSOLE
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif


enum class UIState
{
    Menu,
    PlayerTurn,
    BotTurn
};


int main()
{
    SmallBoard::LoadOutcomes();

    sf::RenderWindow window(sf::VideoMode(900, 900), "SFML works!");

    UIState uiState = UIState::Menu;
    bool isXTurn = true;
    bool alternateTurnMode = false;

    sf::Texture menuTex; menuTex.loadFromFile("Assets\\menu.png");
    sf::Sprite menuSprite(menuTex);
    sf::RectangleShape rect{ {267, 267} };

    sf::RectangleShape shape({ 100.f, 100.f });
    sf::Texture tex; tex.loadFromFile("Assets\\Pieces.png");
    sf::Sprite piece(tex);

    Board currentBoard = { 0, 0, true, -1 };
    int previousMove = -1;

    while (window.isOpen())
    {
        if (uiState == UIState::BotTurn)
        {
            Minimax<WPrSum>(DEPTH, currentBoard);
            int bestMove = GetBestMove(currentBoard);
            if (bestMove != -1)
            {
                currentBoard = currentBoard.MakeMove(bestMove);
                if (alternateTurnMode) uiState = UIState::PlayerTurn;
                if (MOVE_EVAL_SHOWN)
                    Minimax<WPrSum>(DEPTH, currentBoard);

                if (alternateTurnMode) uiState = UIState::PlayerTurn;
            }
        }

        sf::Event event;
        if (uiState == UIState::BotTurn ? window.pollEvent(event) : window.waitEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (uiState == UIState::Menu)
            {
                if (event.type == sf::Event::MouseMoved)
                {
                    rect.setFillColor(sf::Color{ 100,100,100 });

                    if (sf::IntRect{ 180, 180, 267, 267 }.contains(event.mouseMove.x, event.mouseMove.y))
                        rect.setPosition({ 180, 180 });
                    else if (sf::IntRect{ 453, 180, 267, 267 }.contains(event.mouseMove.x, event.mouseMove.y))
                        rect.setPosition({ 453, 180 });
                    else if (sf::IntRect{ 180, 453, 267, 267 }.contains(event.mouseMove.x, event.mouseMove.y))
                        rect.setPosition({ 180, 453 });
                    else if (sf::IntRect{ 453, 453, 267, 267 }.contains(event.mouseMove.x, event.mouseMove.y))
                        rect.setPosition({ 453, 453 });
                    else
                        rect.setFillColor(sf::Color{ 0,0,0 });
                }
                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (sf::IntRect{ 180, 180, 267, 267 }.contains(sf::Mouse::getPosition(window)))
                    {
                        uiState = UIState::PlayerTurn;
                        alternateTurnMode = false;
                    }
                    else if (sf::IntRect{ 453, 180, 267, 267 }.contains(sf::Mouse::getPosition(window)))
                    {
                        uiState = UIState::PlayerTurn;
                        alternateTurnMode = true;
                    }
                    else if (sf::IntRect{ 180, 453, 267, 267 }.contains(sf::Mouse::getPosition(window)))
                    {
                        uiState = UIState::BotTurn;
                        alternateTurnMode = true;
                    }
                    else if (sf::IntRect{ 453, 453, 267, 267 }.contains(sf::Mouse::getPosition(window)))
                    {
                        uiState = UIState::BotTurn;
                        alternateTurnMode = false;
                    }

                }
            }
            else if (uiState == UIState::PlayerTurn)
            {
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    // get input square
                    sf::Vector2i pos = sf::Mouse::getPosition(window) / 100;
                    int square = 3 * (pos.y % 3) + pos.x % 3 + 9 * ((pos.x / 3) % 3) + 27 * (pos.y / 3);

                    // test if square is good
                    if ((currentBoard.m_NextMove == -1 || currentBoard.m_NextMove == square / 9)
                        && currentBoard.IsAvailable(currentBoard.m_NextMove == -1
                            ? square : square % 9) && currentBoard.GetWinState() == 0)
                    {
                        // store previous move
                        previousMove = square;


                        // make move
                        currentBoard = currentBoard.MakeMove(currentBoard.m_NextMove == -1
                            ? square : square % 9); // do something about the lack of shownness

                        if (alternateTurnMode) uiState = UIState::BotTurn;
                    }

                }
                
            }
            else
            {

            }
            
            if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::R)
                    {
                        currentBoard = { 0, 0, true, -1 };
                        uiState = UIState::Menu;
                    }
                    if (event.key.code == sf::Keyboard::E)
                    {
                        MOVE_EVAL_SHOWN = !MOVE_EVAL_SHOWN;

                        Minimax<WPrSum>(DEPTH, currentBoard);
                    }
                    if (event.key.code == sf::Keyboard::P)
                    {
                        alternateTurnMode = !alternateTurnMode;
                    }
                    if (26 < event.key.code && event.key.code < 36)
                    {
                        DEPTH = event.key.code - 26;

                        Minimax<WPrSum>(DEPTH, currentBoard);
                    }
                }
        }


#pragma region RENDERING

        window.clear();

        if (uiState == UIState::Menu)
        {
            window.draw(rect);
            window.draw(menuSprite);
        }
        else
        {

            float bestEval = currentBoard.m_Turn ? -INFINITY : INFINITY;
            std::vector<int> bests = {};

            for (int j = 0; j < 81; j++)
            {
                if (j == previousMove)
                {
                    shape.setFillColor({ 127, 127, 255 });

                    shape.setPosition({ 100 * float(j % 3 + 3 * ((j / 9) % 3)),
                        100 * float((j / 3) % 3 + 3 * (j / 27)) });

                    window.draw(shape);
                }

                if (currentBoard.x[j])
                {
                    piece.setTextureRect({ 0, 0, 100, 100 });
                    piece.setPosition({ 100 * float(j % 3 + 3 * ((j / 9) % 3)),
                        100 * float((j / 3) % 3 + 3 * (j / 27)) });
                    window.draw(piece);
                }
                else if (currentBoard.o[j])
                {
                    piece.setTextureRect({ 100, 0, 100, 100 });
                    piece.setPosition({ 100 * float(j % 3 + 3 * ((j / 9) % 3)),
                        100 * float((j / 3) % 3 + 3 * (j / 27)) });
                    window.draw(piece);
                }
                else
                {

                    if (currentBoard.GetWinState() != 0 ||
                        (currentBoard.m_NextMove != -1 && currentBoard.m_NextMove != j / 9)
                        || (currentBoard.m_NextMove == -1 &&
                            (currentBoard.bx | currentBoard.bo)[j / 9]))
                        continue;

                    if (MOVE_EVAL_SHOWN)
                    {
                        float val = EVALUATIONS_ARRAY[currentBoard.m_NextMove == -1 ? j : j % 9];

                        if (val == bestEval)
                            bests.push_back(j);
                        else if (currentBoard.m_Turn ? val > bestEval : val < bestEval)
                        {
                            bestEval = val;
                            bests = { j };
                        }

                        val *= (currentBoard.m_Turn ? 1 : -1);

                        shape.setFillColor({ sf::Uint8(std::max(0.f, std::min(255.f, -255 * (val - 1.f) / 2.f))),
                                             sf::Uint8(std::max(0.f, std::min(255.f,  255 * (val + 1.f) / 2.f))),
                                             sf::Uint8(std::max(0.f, std::min(255.f, 127 * (1.f - std::abs(val))))) });
                    }
                    else shape.setFillColor({ 127, 127, 127 });

                    shape.setPosition({ 100 * float(j % 3 + 3 * ((j / 9) % 3)),
                        100 * float((j / 3) % 3 + 3 * (j / 27)) });
                    window.draw(shape);
                }
            }

            for (int j : bests)
            {
                piece.setTextureRect({ 200, 0, 100, 100 });
                piece.setPosition({ 100 * float(j % 3 + 3 * ((j / 9) % 3)),
                    100 * float((j / 3) % 3 + 3 * (j / 27)) });
                window.draw(piece);
            }

            piece.setScale(3, 3);
            for (int j = 0; j < 9; j++)
            {
                if (currentBoard.bx[j])
                {
                    piece.setTextureRect({ 0, 0, 100, 100 });
                    piece.setPosition({ 300 * float(j % 3), 300 * float((j / 3)) });
                    window.draw(piece);
                }
                else if (currentBoard.bo[j])
                {
                    piece.setTextureRect({ 100, 0, 100, 100 });
                    piece.setPosition({ 300 * float(j % 3), 300 * float((j / 3)) });
                    window.draw(piece);
                }
            }
            piece.setScale(1, 1);

        }
        window.display();

#pragma endregion

    }

    return 0;
}
