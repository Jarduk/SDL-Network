#pragma once

#include <iostream>

#include "SDL.h"

#include "TMessage.h"

class CRenderer
{
#pragma region Variables
public:

private:
    
    SDL_Window* m_pWindow = nullptr;
    SDL_Renderer* m_pRenderer = nullptr;
    SDL_Texture* m_pRenderTexture = nullptr;

    int m_WindowPosition[2] = { 100, 100 };
    int m_WindowSize[2] = { 640, 700 };

    SDL_Rect m_GameFloor = { 5, 5, 630, 630 };
    SDL_Rect m_GameBorder = { 0, 0, 640, 640 };

    SDL_Rect m_PlayerColor = { 0, 640, 640, 60 };

    SDL_Rect m_DrawObject = { 0, 0, 5, 5 };

    
    SDL_Color m_ColorWhite = { 255, 255, 255 };
    SDL_Color m_ColorBlack = { 0, 0, 0 };
    SDL_Color m_ColorBlue = { 0, 0, 255 };
    SDL_Color m_ColorRed = { 255, 0, 0 };
    SDL_Color m_ColorGreen = { 0, 255, 0 };

#pragma endregion
#pragma region Methods
public:
    CRenderer();
    ~CRenderer();

    void Initialize();
    
    void Draw(TMessage::ServerMessage* a_gameInfo, TMessage::EClientState* a_clientState, int a_playerID);
    
private:

#pragma endregion
};