#pragma once

#include <string>
#include <iostream>
#include <Windows.h>
#include <conio.h>

#include "SDL_net.h"
#undef main

#include "TMessage.h"
#include "CTimer.h"
#include "CRenderer.h"

class CClient
{
#pragma region Variables
public:

private:

    bool m_Quit = false;

    TMessage::EClientState m_State = TMessage::EClientState::NONE;

    SDLNet_SocketSet m_SocketSet;
    TCPsocket m_ConnectionSocket;

    TMessage::ServerMessage m_GameInfo;
    TMessage::ClientMessage m_Message;

    bool m_ReadyMessage = false;

    int m_PlayerID = 0;

    int m_Result = 0; 

    CTimer m_Timer;
    float m_Timeout = 0;

    CRenderer m_Renderer;

#pragma endregion
#pragma region Methods
public:
    CClient();
    ~CClient();

    bool Initialize(std::string a_ipAddress, std::string a_port);
    bool Initialize(std::string a_port);

    void Update();

    bool GetQuit();

private:
    void CheckTimeout();

    void Recieve();
    void Send();

    void HandleMessage(TMessage::ServerMessage a_message);
    void HandleEvents();
#pragma endregion
};