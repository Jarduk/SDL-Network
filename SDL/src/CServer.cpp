#include "CServer.h"

CServer::CServer()
{
}

CServer::~CServer()
{
    SDLNet_TCP_Close(m_AcceptSocket);
    SDLNet_TCP_Close(m_ConnectionSocketHost);
    SDLNet_TCP_Close(m_ConnectionSocketClient);
    SDLNet_FreeSocketSet(m_SocketSet);
    SDLNet_Quit();
}

bool CServer::Initialize(std::string a_port)
{
    if (SDLNet_Init() == -1)
    {
        std::cout << "[ERROR CServer.cpp] - SDL Init error: " << SDLNet_GetError() << std::endl;
    }
    else
    {
        IPaddress ipAddress;

        if (SDLNet_ResolveHost(&ipAddress, NULL, std::stoi(a_port)) == -1)
        {
            std::cout << "[ERROR CServer.cpp] - SDL ResolveHost error: " << SDLNet_GetError() << std::endl;
        }
        else
        {
            m_AcceptSocket = SDLNet_TCP_Open(&ipAddress);
            if (!m_AcceptSocket)
            {
                std::cout << "[ERROR CServer.cpp] - SDL TCP_Open error: " << SDLNet_GetError() << std::endl;
            }
            else
            {
                m_SocketSet = SDLNet_AllocSocketSet(2);

                if (!m_SocketSet)
                {
                    std::cout << "[ERROR CServer.cpp] - SDL SocketSet error: " << SDLNet_GetError() << std::endl;
                }
                else
                {
                    m_State = TMessage::EServerState::AcceptingClients;
                    std::cout << "[Game Server] - 1. Warten auf Verbindung" << std::endl;

                    return true;
                }
            }
        }
    }   

    m_State = TMessage::EServerState::NONE;
    return false;
}

void CServer::Update()
{
    while (!m_Quit)
    {
        m_Timer.Update();

        switch (m_State)
        {
        case TMessage::EServerState::AcceptingClients:
            AcceptingClients();
            break;
        case TMessage::EServerState::Lobby:
            CheckTimeout();
            Lobby();
            break;
        case TMessage::EServerState::Run:
            CheckTimeout();
            Run();
            break;
        case TMessage::EServerState::Victory:
            ResetGame();
            break;
        }
    }
}

void CServer::SetQuit(bool a_quit)
{
    m_Quit = a_quit;
}

bool CServer::GetReset()
{
    return m_Reset;
}

void CServer::AcceptingClients()
{
    if (!m_ConnectionSocketHost)
    {
        m_ConnectionSocketHost = SDLNet_TCP_Accept(m_AcceptSocket);

        if (m_ConnectionSocketHost)
        {
            SDLNet_TCP_AddSocket(m_SocketSet, m_ConnectionSocketHost);
            std::cout << "[Game Server] - 2. Client Connected" << std::endl;
        }
    }

    else if (!m_ConnectionSocketClient)
    {
        m_ConnectionSocketClient = SDLNet_TCP_Accept(m_AcceptSocket);

        if (m_ConnectionSocketClient)
        {
            SDLNet_TCP_AddSocket(m_SocketSet, m_ConnectionSocketClient);
            std::cout << "[Game Server] - 2. Client Connected" << std::endl;

            m_Game.Initialize(&m_GameInfo);

            m_Game.InitializeGame();

            m_State = TMessage::EServerState::Lobby;
        }
    }
}

void CServer::Lobby()
{
    Recieve();
    Send();
}

void CServer::Run()
{
    Recieve();

    if (m_CurTick <= 0)
    {
        m_Game.CalculateTick();

        m_CurTick += m_Game.GetTick();

        Send();
    }
    else
    {
        m_CurTick -= m_Timer.GetDeltaTime();
    }

    if (m_GameInfo.State == TMessage::EGameState::VictoryClient || m_GameInfo.State == TMessage::EGameState::VictoryHost || m_GameInfo.State == TMessage::EGameState::VictoryDraw)
    {
        m_State = TMessage::EServerState::Victory;
    }
}

void CServer::ResetGame()
{
    if (m_CurTick <= 0)
    {
        m_Game.InitializeGame();
        m_State = TMessage::EServerState::Lobby;

        m_PlayerReady[0] = false;
        m_PlayerReady[1] = false;
    }
    else
    {
        m_CurTick -= m_Timer.GetDeltaTime();
    }
}

void CServer::CheckTimeout()
{
    m_ClientTimeout[0] += m_Timer.GetDeltaTime();
    m_ClientTimeout[1] += m_Timer.GetDeltaTime();

    if (m_ClientTimeout[0] > 10.0f || m_ClientTimeout[1] > 10.0f)
    {
        std::cout << "[Game Server] - 2.e Client unterbricht die Verbindung" << std::endl;
        m_Reset = true;
    }
}

void CServer::Recieve()
{
    TMessage::ClientMessage Message;

    m_Result = SDLNet_CheckSockets(m_SocketSet, 50);

    if (m_Result == -1)
    {
        std::cout << "[ERROR CServer.cpp] - CheckSockets(): " << SDLNet_GetError() << std::endl;
        m_Reset = true;
    }
    else if (m_Result > 0)
    {
        m_Result = 0;

        if (SDLNet_SocketReady(m_ConnectionSocketHost))
            m_Result = SDLNet_TCP_Recv(m_ConnectionSocketHost, &Message, sizeof(Message));

        if (m_Result > 0)
        {
            HandleMessage(Message, 0);
        }
        else if (m_Result < 0)
        {
            std::cout << "[ERROR CServer.cpp] - RecieveHost(): " << SDLNet_GetError() << std::endl;
            m_Reset = true;
        }

        m_Result = 0;

        if (SDLNet_SocketReady(m_ConnectionSocketClient))
            m_Result = SDLNet_TCP_Recv(m_ConnectionSocketClient, &Message, sizeof(Message));

        if (m_Result > 0)
        {
            HandleMessage(Message, 1);
        }
        else if (m_Result < 0)
        {
            std::cout << "[ERROR CServer.cpp] - RecieveClient(): " << SDLNet_GetError() << std::endl;
            m_Reset = true;
        }
    }
}

void CServer::Send()
{
    m_Result = SDLNet_TCP_Send(m_ConnectionSocketHost, &m_GameInfo, sizeof(m_GameInfo));
    
    if (m_Result < sizeof(m_GameInfo))
    {
        std::cout << "[ERROR CServer.cpp] - SendHost(): " << SDLNet_GetError() << std::endl;
        m_Reset = true;
    }

   m_Result = SDLNet_TCP_Send(m_ConnectionSocketClient, &m_GameInfo, sizeof(m_GameInfo));

    if (m_Result < sizeof(m_GameInfo))
    {
        std::cout << "[ERROR CServer.cpp] - SendClient(): " << SDLNet_GetError() << std::endl;
        m_Reset = true;
    }
}

void CServer::HandleMessage(TMessage::ClientMessage a_message, int a_playerID)
{
    m_ClientTimeout[a_playerID] = 0;
    switch (a_message.Message)
    {
    case TMessage::EMessage::Up:
        m_Game.SetPlayerMovement(TMessage::EMessage::Up, a_playerID);
        break;
    case TMessage::EMessage::Down:
        m_Game.SetPlayerMovement(TMessage::EMessage::Down, a_playerID);
        break;
    case TMessage::EMessage::Left:
        m_Game.SetPlayerMovement(TMessage::EMessage::Left, a_playerID);
        break;
    case TMessage::EMessage::Right:
        m_Game.SetPlayerMovement(TMessage::EMessage::Right, a_playerID);
        break;
    case TMessage::EMessage::Ready:
        m_PlayerReady[a_playerID] = true;

        if (m_PlayerReady[0] && m_PlayerReady[1])
        {
            std::cout << "[Game Server] - 2.c Spiel beginnt" << std::endl;
            m_State = TMessage::EServerState::Run;
            m_GameInfo.State = TMessage::EGameState::Run;
            m_CurTick = m_Game.GetTick();
        }
        break;
    case TMessage::EMessage::Leave:
        if (a_playerID == 1)
        {
            std::cout << "[Game Server] - 2.e Client unterbricht die Verbindung" << std::endl;
            m_Reset = true;
        }
        break;
    }
}