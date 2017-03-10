#pragma once
#include <iostream>
#include <string>
#include <conio.h>
#include <Windows.h>
#include <thread>

#include "CClient.h"
#include "CServer.h"

class CNibbles
{
#pragma region Variables
public:

private:

    bool m_Quit = false;

    std::string m_Port = "";

    CClient* m_pClient = nullptr;
    CServer* m_pServer = nullptr;

    std::thread m_ServerThread;

#pragma endregion
#pragma region Methods
public:
    CNibbles();
    ~CNibbles();

    void Update();

    bool GetQuit();

private:
    void Initialize();

    bool CheckInput(std::string a_input);

    void ResetServer();

#pragma endregion
};