#include "CRenderer.h"

CRenderer::CRenderer()
{

}

CRenderer::~CRenderer()
{
    SDL_DestroyWindow(m_pWindow);
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyTexture(m_pRenderTexture);
}

void CRenderer::Initialize()
{
    SDL_Init(SDL_INIT_VIDEO);
    
    m_pWindow = SDL_CreateWindow("Nibbles", m_WindowPosition[0], m_WindowPosition[1], m_WindowSize[0], m_WindowSize[1], 0);

    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
    
    m_pRenderTexture = SDL_CreateTexture(m_pRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, m_WindowSize[0], m_WindowSize[1]);
}

void CRenderer::Draw(TMessage::ServerMessage* a_gameInfo, TMessage::EClientState* a_clientState, int a_playerID)
{
    if (a_gameInfo->State == TMessage::EGameState::Run)
    {
        //Clear
        SDL_SetRenderDrawColor(m_pRenderer, m_ColorWhite.r, m_ColorWhite.g, m_ColorWhite.b, 255);
        SDL_RenderClear(m_pRenderer);
        
        //Draw Game Floor
        SDL_SetRenderDrawColor(m_pRenderer, m_ColorBlack.r, m_ColorBlack.g, m_ColorBlack.b, 255);
        SDL_RenderFillRect(m_pRenderer, &m_GameFloor);

        //Draw Player Color
        if (a_playerID == 0)
        {
            SDL_SetRenderDrawColor(m_pRenderer, m_ColorRed.r, m_ColorRed.g, m_ColorRed.b, 255);
            SDL_RenderFillRect(m_pRenderer, &m_PlayerColor);
        }
        else
        {
            SDL_SetRenderDrawColor(m_pRenderer, m_ColorBlue.r, m_ColorBlue.g, m_ColorBlue.b, 255);
            SDL_RenderFillRect(m_pRenderer, &m_PlayerColor);
        }

        //Draw Player Host
        SDL_SetRenderDrawColor(m_pRenderer, m_ColorRed.r, m_ColorRed.g, m_ColorRed.b, 255);
        for (int i = 0; i < 168; i++)
        {
            if (a_gameInfo->PlayerHostPos[i][0] == 0 || a_gameInfo->PlayerHostPos[i][1] == 0)
                break;

            m_DrawObject.x = a_gameInfo->PlayerHostPos[i][0] * m_DrawObject.w;
            m_DrawObject.y = a_gameInfo->PlayerHostPos[i][1] * m_DrawObject.h;
            SDL_RenderFillRect(m_pRenderer, &m_DrawObject);
        }

        //Draw Player Client
        SDL_SetRenderDrawColor(m_pRenderer, m_ColorBlue.r, m_ColorBlue.g, m_ColorBlue.b, 255);
        for (int i = 0; i < 168; i++)
        {
            if (a_gameInfo->PlayerClientPos[i][0] == 0 || a_gameInfo->PlayerClientPos[i][1] == 0)
                break;

            m_DrawObject.x = a_gameInfo->PlayerClientPos[i][0] * m_DrawObject.w;
            m_DrawObject.y = a_gameInfo->PlayerClientPos[i][1] * m_DrawObject.h;
            SDL_RenderFillRect(m_pRenderer, &m_DrawObject);
        }

        //Draw Bonus
        SDL_SetRenderDrawColor(m_pRenderer, m_ColorGreen.r, m_ColorGreen.g, m_ColorGreen.b, 255);
        if (a_gameInfo->BonusPos[0] != 0 && a_gameInfo->BonusPos[1] != 0)
        {
            m_DrawObject.x = a_gameInfo->BonusPos[0] * m_DrawObject.w;
            m_DrawObject.y = a_gameInfo->BonusPos[1] * m_DrawObject.h;
            SDL_RenderFillRect(m_pRenderer, &m_DrawObject);
        }
    }
    else  if (a_gameInfo->State == TMessage::EGameState::VictoryClient)
    {
        SDL_SetRenderDrawColor(m_pRenderer, m_ColorBlue.r, m_ColorBlue.g, m_ColorBlue.b, 255);
        SDL_RenderClear(m_pRenderer);
    }
    else if (a_gameInfo->State == TMessage::EGameState::VictoryHost)
    {
        SDL_SetRenderDrawColor(m_pRenderer, m_ColorRed.r, m_ColorRed.g, m_ColorRed.b, 255);
        SDL_RenderClear(m_pRenderer);
    } 
    else if (a_gameInfo->State == TMessage::EGameState::VictoryDraw)
    {
        SDL_SetRenderDrawColor(m_pRenderer, m_ColorGreen.r, m_ColorGreen.g, m_ColorGreen.b, 255);
        SDL_RenderClear(m_pRenderer);
    }
    else if (a_gameInfo->State == TMessage::EGameState::PlayerInput)
    {
        SDL_SetRenderDrawColor(m_pRenderer, m_ColorBlack.r, m_ColorBlack.g, m_ColorBlack.b, 255);
        SDL_RenderClear(m_pRenderer);
    }

    SDL_RenderPresent(m_pRenderer);
}
