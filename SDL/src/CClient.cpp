#include "CClient.h"

CClient::CClient()
{

}

CClient::~CClient()
{
    SDLNet_TCP_Close(m_ConnectionSocket);
    SDLNet_FreeSocketSet(m_SocketSet);
    SDLNet_Quit();
}

bool CClient::Initialize(std::string a_ipAddress, std::string a_port)
{
    m_PlayerID = 1;

    if (SDLNet_Init() == -1)
    {
        std::cout << "[ERROR CClient.cpp] - SDL Init error: " << SDLNet_GetError() << std::endl;
    }
    else
    {
        IPaddress ipAddress;

        if (SDLNet_ResolveHost(&ipAddress, a_ipAddress.c_str(), std::stoi(a_port)) == -1)
        {
            std::cout << "[ERROR CClient.cpp] - SDL ResolveHost error: " << SDLNet_GetError() << std::endl;
        }
        else
        {
            m_ConnectionSocket = SDLNet_TCP_Open(&ipAddress);
            if (!m_ConnectionSocket)
            {
                std::cout << "[ERROR CClient.cpp] - SDL TCP_Open error: " << SDLNet_GetError() << std::endl;

                std::cout << "[Game Client] - 1. Verbindung nicht möglich" << SDLNet_GetError() << std::endl;

            }
            else
            {
                m_SocketSet = SDLNet_AllocSocketSet(1);

                if (!m_SocketSet)
                {
                    std::cout << "[ERROR CClient.cpp] - SDL SocketSet error: " << SDLNet_GetError() << std::endl;
                }
                else
                {
                    SDLNet_TCP_AddSocket(m_SocketSet, m_ConnectionSocket);

                    std::cout << "[Game Client] - 1. Verbindung erfolgreich aufgebaut" << std::endl;
                    m_State = TMessage::EClientState::Connected;

                    m_Renderer.Initialize();
                    return true;
                }
            }
        }
    }

    return false;
}

//Local Client Initialize Funktion
bool CClient::Initialize(std::string a_port)
{
    IPaddress ipAddress;

    m_PlayerID = 0;

    if (SDLNet_ResolveHost(&ipAddress, "127.0.0.1", std::stoi(a_port)) == -1)
    {
        std::cout << "[ERROR CClient.cpp] - SDL ResolveHost error: " << SDLNet_GetError() << std::endl;
    }
    else
    {
        m_ConnectionSocket = SDLNet_TCP_Open(&ipAddress);
        if (!m_ConnectionSocket)
        {
            std::cout << "[ERROR CClient.cpp] - SDL TCP_Open error: " << SDLNet_GetError() << std::endl;
        }
        else
        {
            m_SocketSet = SDLNet_AllocSocketSet(1);

            if (!m_SocketSet)
            {
                std::cout << "[ERROR CClient.cpp] - SDL SocketSet error: " << SDLNet_GetError() << std::endl;
            }
            else
            {
                SDLNet_TCP_AddSocket(m_SocketSet, m_ConnectionSocket);

                std::cout << "[Game Client] - 1. Verbindung erfolgreich aufgebaut" << std::endl;
                m_State = TMessage::EClientState::Connected;

                m_Renderer.Initialize();
                return true;
            }
        }
    }

    return false;
}

void CClient::Update()
{
    m_Timer.Update();

    if(m_GameInfo.State != TMessage::EGameState::NONE)
        CheckTimeout();

    Recieve();
    HandleEvents();
    
    if (m_State == TMessage::EClientState::Connected && m_GameInfo.State == TMessage::EGameState::PlayerInput && _kbhit())
    {
        m_Message.Message = TMessage::EMessage::Ready;
        m_State = TMessage::EClientState::Ready;

        Send();
    }

    m_Renderer.Draw(&m_GameInfo, &m_State, m_PlayerID);
}

bool CClient::GetQuit()
{
    return m_Quit;
}

void CClient::CheckTimeout()
{    
    m_Timeout += m_Timer.GetDeltaTime();

    if (m_Timeout > 3.0f)
    {
        std::cout << "[Game Client] - 2.g  Server verliert (oder beendet) die Verbindung " << std::endl;
        m_Quit = true;
    }
}

void CClient::Recieve()
{
    m_Result = SDLNet_CheckSockets(m_SocketSet, 50);

    if (m_Result == -1)
    {
        std::cout << "[ERROR CClient.cpp] - CheckSockets(): " << SDLNet_GetError() << std::endl;
        m_Quit = true;
    }
    else if (m_Result > 0)
    {

        if (SDLNet_SocketReady(m_ConnectionSocket))
            m_Result = SDLNet_TCP_Recv(m_ConnectionSocket, &m_GameInfo, sizeof(m_GameInfo));

        else
            m_Result = 0;
        
        if (m_Result > 0)
        {
            HandleMessage(m_GameInfo);

            m_Timeout = 0;
            m_Message.Message = TMessage::EMessage::Ping;
            Send();
        }
        else if (m_Result < 0)
        {
            std::cout << "[ERROR CClient.cpp] - Recieve(): " << SDLNet_GetError() << std::endl;
            m_Quit = true;
        }
    }
}

void CClient::Send()
{
    m_Result = SDLNet_TCP_Send(m_ConnectionSocket, &m_Message, sizeof(m_Message));

    if (m_Result < sizeof(m_Message))
    {
        std::cout << "[ERROR CClient.cpp] - Send(): " << SDLNet_GetError() << std::endl;
    }

    m_Message.Message = TMessage::EMessage::NONE;
}

void CClient::HandleMessage(TMessage::ServerMessage a_message)
{
    switch (a_message.State)
    {
    case TMessage::EGameState::PlayerInput:
        if (!m_ReadyMessage)
        {
            std::cout << "[Game Client] - 2.a Initialisierung des Spielfeldes" << std::endl;
            std::cout << "[Game Client] - 2.b Bitte eine Taste druecken" << std::endl;
            m_ReadyMessage = true;
        }
        break;
    case TMessage::EGameState::Run:
        if (m_State != TMessage::EClientState::Run)
        {
            std::cout << "[Game Client] - 2.e Spiel beginnen" << std::endl;
            m_State = TMessage::EClientState::Run;
        }
        break;
    case TMessage::EGameState::VictoryHost:
        if (m_State != TMessage::EClientState::Connected)
        {
            std::cout << "[Game Client] - 2.f Client 1 Gewinnt" << std::endl;
        m_State = TMessage::EClientState::Connected;
        }
        m_ReadyMessage = false;
        break;
    case TMessage::EGameState::VictoryClient:
        if (m_State != TMessage::EClientState::Connected)
        {
            std::cout << "[Game Client] - 2.f Client 2 Gewinnt" << std::endl;
        m_State = TMessage::EClientState::Connected;
        }
        m_ReadyMessage = false;
        break;
    case TMessage::EGameState::VictoryDraw:
        if (m_State != TMessage::EClientState::Connected)
        {
            std::cout << "[Game Client] - 2.f Untentschieden" << std::endl;
            m_State = TMessage::EClientState::Connected;
        }
        m_ReadyMessage = false;
        break;
    }
}

void CClient::HandleEvents()
{
    SDL_Event Event;
    if (SDL_PollEvent(&Event))
    {
        if (m_GameInfo.State == TMessage::EGameState::Run)
        {
            if (Event.key.keysym.scancode == SDL_SCANCODE_UP && m_GameInfo.BlockedDirection[m_PlayerID] != TMessage::EMessage::Up)
            {
                m_Message.Message = TMessage::EMessage::Up;
                Send();
            }
            else if (Event.key.keysym.scancode == SDL_SCANCODE_DOWN && m_GameInfo.BlockedDirection[m_PlayerID] != TMessage::EMessage::Down)
            {
                m_Message.Message = TMessage::EMessage::Down;
                Send();
            }
            if (Event.key.keysym.scancode == SDL_SCANCODE_LEFT && m_GameInfo.BlockedDirection[m_PlayerID] != TMessage::EMessage::Left)
            {
                m_Message.Message = TMessage::EMessage::Left;
                Send();
            }
            else if (Event.key.keysym.scancode == SDL_SCANCODE_RIGHT && m_GameInfo.BlockedDirection[m_PlayerID] != TMessage::EMessage::Right)
            {
                m_Message.Message = TMessage::EMessage::Right;
                Send();
            }
        }       
        else if (m_GameInfo.State == TMessage::EGameState::PlayerInput)
        {
            if (Event.type == SDL_KEYDOWN)
            {
                std::cout << "[Game Client] - 2.c Client Taste gedrueckt" << std::endl;
                std::cout << "[Game Client] - 2.d Warten auf Antwort des Server" << std::endl;

                m_Message.Message = TMessage::EMessage::Ready;
                Send();
            }
        }

        if (Event.type == SDL_QUIT || Event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            m_Message.Message = TMessage::EMessage::Leave;
            Send();

            m_Quit = true;
        }
        else if (Event.type == SDL_WINDOWEVENT_MOVED)
        {
            m_Timeout = 0;
        }
    }
}