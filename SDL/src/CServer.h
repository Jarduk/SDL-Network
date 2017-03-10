#pragma once

#include <string>
#include <iostream>
#include <Windows.h>

#include "SDL_net.h"
#undef main

#include "TMessage.h"
#include "CGame.h"
#include "CTimer.h"

class CServer
{
#pragma region Variables
public:

private:

    bool m_Quit = false;
    bool m_Reset = false;

    TMessage::EServerState m_State = TMessage::EServerState::NONE;

    SDLNet_SocketSet m_SocketSet;
    TCPsocket m_AcceptSocket = 0;
    TCPsocket m_ConnectionSocketHost = 0;
    TCPsocket m_ConnectionSocketClient = 0;

    TMessage::ServerMessage m_GameInfo;

    int m_Result = 0;
    bool m_PlayerReady[2] = { false };

    CGame m_Game;
    bool m_GameInit = false;

    CTimer m_Timer;
    float m_ClientTimeout[2] = { 0 };
    float m_CurTick = 0.0f;

#pragma endregion
#pragma region Methods
public:
    CServer();
    ~CServer();

    bool Initialize(std::string a_port);
    void Update();

    void SetQuit(bool a_quit);
    bool GetReset();

private:

    void AcceptingClients();
    void Lobby();
    void Run();
    void ResetGame();

    void CheckTimeout();

    void Recieve();
    void Send();

    void HandleMessage(TMessage::ClientMessage a_message, int a_playerID);
#pragma endregion
};