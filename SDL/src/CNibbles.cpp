#include "CNibbles.h"

CNibbles::CNibbles()
{
    Initialize();
}

CNibbles::~CNibbles()
{
    if (m_pClient != nullptr)
    {
        delete(m_pClient);
        m_pClient = nullptr;
    }
    if (m_pServer != nullptr)
    {
        m_pServer->SetQuit(true);
        m_ServerThread.join();
        delete(m_pServer);
        m_pServer = nullptr;
    }
}

void CNibbles::Initialize()
{
    std::string input = "";

    bool correctInput = false;

    while (!correctInput && !m_Quit)
    {
        if (_kbhit())
        {
            if (GetKeyState(VK_ESCAPE) & 0x8000)
            {
                m_Quit = true;
            }
            else
            {
                std::getline(std::cin, input);

                //Debug
                if (input == "1")
                {
                    input = "nibbles server 1234";
                }
                else if (input == "2")
                {
                    input = "nibbles client 10.1.4.33 1234";
                }

                if (!(correctInput = CheckInput(input)))
                {
                    std::cout << "Input was not correct. Please try again." << std::endl;
                }
            }
        }
    }
}

void CNibbles::Update()
{    
    if (m_pServer != nullptr)
    {
        if (m_pServer->GetReset())
        {
            ResetServer();
        }
    }

    if (m_pClient != nullptr)
    {
        if(!m_Quit) 
            m_Quit = m_pClient->GetQuit();

        m_pClient->Update();
    }
}

bool CNibbles::GetQuit()
{
    return m_Quit;
}

bool CNibbles::CheckInput(std::string a_input)
{
    std::string gameName = "";
    std::string startType = "";
    std::string ip = "";

    int itGameName = 0;
    int itStartType = 0;
    int itIp = 0;
    int itPort = 0;

    itGameName = a_input.find(" ", 0);
    itStartType = a_input.find(" ", itGameName + 1);

    gameName = a_input.substr(0, itGameName);
    startType = a_input.substr(itGameName + 1, itStartType - (itGameName + 1));

    //Check Input
    if (gameName == "nibbles")
    {
        if (startType == "client")
        {
            itIp = a_input.find(" ", itStartType + 1);
            itPort = a_input.find(" ", itIp + 1);
            ip = a_input.substr(itStartType + 1, itIp - (itStartType + 1));
            m_Port = a_input.substr(itIp + 1, itPort - itIp);

            m_pClient = new CClient();
            if (m_pClient->Initialize(ip, m_Port))
            {
                return true;
            }
            else
            {
                std::cout << "[ERROR CNibbles.cpp] - Client Initialize() failed" << std::endl;

                std::cout << std::endl << "Could not connect to Server. Please try again." << std::endl;
            }

        }
        else if (startType == "server")
        {
            itPort = a_input.find(" ", itStartType + 1);
            m_Port = a_input.substr(itStartType + 1, itPort - itStartType);

            m_pServer = new CServer();
            if (m_pServer->Initialize(m_Port))
            {
                m_ServerThread = std::thread(&CServer::Update, m_pServer);
                m_pClient = new CClient();
                if (m_pClient->Initialize(m_Port))
                {
                    return true;
                }
                else
                {
                    std::cout << "[ERROR CNibbles.cpp] - ServerClient Initialize() failed" << std::endl;

                    std::cout << std::endl << "Could not connect to Server. Please try again." << std::endl;
                }
            }
            else
            {
                std::cout << "[ERROR CNibbles.cpp] - Server Initialize() failed" << std::endl;
            }
        }
    }
    return false;
}

void CNibbles::ResetServer()
{
    m_pServer->SetQuit(true);
    m_ServerThread.join();
    delete(m_pServer);

    delete m_pClient;  

    m_pServer = new CServer();
    m_pServer->Initialize(m_Port);
    m_ServerThread = std::thread(&CServer::Update, m_pServer);

    m_pClient = new CClient();
    m_pClient->Initialize(m_Port);
}
