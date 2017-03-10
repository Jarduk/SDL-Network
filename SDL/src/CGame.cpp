#include "CGame.h"

CGame::CGame()
{
}

CGame::~CGame()
{
}

bool CGame::Initialize(TMessage::ServerMessage * a_pgameInfo)
{
    m_pGameInfo = a_pgameInfo;

    return false;
}

void CGame::InitializeGame()
{
    std::cout << "[Game Server] - 2.a Initialisierung des Spielfeldes" << std::endl;

    m_BonusPosition[0] = 0;
    m_BonusPosition[1] = 0;

    m_PlayerPoints[0] = 0;
    m_PlayerPoints[1] = 0;

    m_BlockedDirection[0] = TMessage::EMessage::Left;
    m_BlockedDirection[1] = TMessage::EMessage::Right;

    m_pGameInfo->State = TMessage::EGameState::PlayerInput;

    SetPlayerMovement(TMessage::EMessage::Right, 0);
    SetPlayerMovement(TMessage::EMessage::Left, 1);

    m_Collision[0] = ECollisionType::NONE;
    m_Collision[1] = ECollisionType::NONE;

    m_Tick = 1.0f;

    SSnakePosition snakePosition;

    m_PlayerSnake[0].clear();
    m_PlayerSnake[1].clear();

    for (int i = 0; i < 168; i++)
    {
        if (i < (m_PlayerPoints[0] + 1) * 8)
        {
            m_pGameInfo->PlayerHostPos[i][0] = 16;
            m_pGameInfo->PlayerHostPos[i][1] = 64;

            snakePosition.Position[0] = m_pGameInfo->PlayerHostPos[i][0];
            snakePosition.Position[1] = m_pGameInfo->PlayerHostPos[i][1];

            m_PlayerSnake[0].push_front(snakePosition);

            m_pGameInfo->PlayerClientPos[i][0] = 112;
            m_pGameInfo->PlayerClientPos[i][1] = 64;

            snakePosition.Position[0] = m_pGameInfo->PlayerClientPos[i][0];
            snakePosition.Position[1] = m_pGameInfo->PlayerClientPos[i][1];

            m_PlayerSnake[1].push_front(snakePosition);
        }
        else
        {
            m_pGameInfo->PlayerHostPos[i][0] = 0;
            m_pGameInfo->PlayerHostPos[i][1] = 0;

            m_pGameInfo->PlayerClientPos[i][0] = 0;
            m_pGameInfo->PlayerClientPos[i][1] = 0;
        }
    }

    GenerateNewBonus();

    std::cout << "[Game Server] - 2.b Den Spieler Auffordern eine Taste zu druecken" << std::endl;
}

void CGame::CalculateTick()
{
    CalculatePlayerMovement();

    CheckCollision();

    SetGameInfo();
}

void CGame::CalculatePlayerMovement()
{
    SSnakePosition newPos;

    newPos.Position[0] = m_PlayerSnake[0].front().Position[0] + m_PlayerMoveDir[0][0];
    newPos.Position[1] = m_PlayerSnake[0].front().Position[1] + m_PlayerMoveDir[0][1];

    m_PlayerSnake[0].push_front(newPos);
    m_PlayerSnake[0].pop_back();    

    newPos.Position[0] = m_PlayerSnake[1].front().Position[0] + m_PlayerMoveDir[1][0];
    newPos.Position[1] = m_PlayerSnake[1].front().Position[1] + m_PlayerMoveDir[1][1];

    m_PlayerSnake[1].push_front(newPos);
    m_PlayerSnake[1].pop_back();
}

void CGame::CheckCollision()
{
    for (int i = 0; i < (int)m_PlayerSnake[0].size() ; i++)
    {
        if (m_PlayerSnake[1].front().Position[0] == m_PlayerSnake[0].at(i).Position[0]
            && m_PlayerSnake[1].front().Position[1] == m_PlayerSnake[0].at(i).Position[1])
            HandleCollision(ECollisionType::Snake, 1);

        if (i > 0)
        {
            if (m_PlayerSnake[0].front().Position[0] == m_PlayerSnake[0].at(i).Position[0]
                && m_PlayerSnake[0].front().Position[1] == m_PlayerSnake[0].at(i).Position[1])
                HandleCollision(ECollisionType::Snake, 0);
        }
    }

    for (int i = 0; i < (int)m_PlayerSnake[1].size(); i++)
    {
        if (m_PlayerSnake[0].front().Position[0] == m_PlayerSnake[1].at(i).Position[0] 
            && m_PlayerSnake[0].front().Position[1] == m_PlayerSnake[1].at(i).Position[1])
            HandleCollision(ECollisionType::Snake, 0);

        if (i > 0)
        {
            if (m_PlayerSnake[1].front().Position[0] == m_PlayerSnake[1].at(i).Position[0]
                && m_PlayerSnake[1].front().Position[1] == m_PlayerSnake[1].at(i).Position[1])
                HandleCollision(ECollisionType::Snake, 1);
        }
    }

    if (m_PlayerSnake[0].front().Position[0] == 127 || m_PlayerSnake[0].front().Position[0] == 0 ||
        m_PlayerSnake[0].front().Position[1] == 127 || m_PlayerSnake[0].front().Position[1] == 0)
    {
        HandleCollision(ECollisionType::Border, 0);
    }

    if (m_PlayerSnake[1].front().Position[0] == 127 || m_PlayerSnake[1].front().Position[0] == 0 ||
        m_PlayerSnake[1].front().Position[1] == 127 || m_PlayerSnake[1].front().Position[1] == 0)
    {
        HandleCollision(ECollisionType::Border, 1);
    }

    if (m_Collision[0] != ECollisionType::NONE && m_Collision[1] != ECollisionType::NONE)
    {
        if (m_PlayerPoints[0] > m_PlayerPoints[1])
        {
            m_pGameInfo->State = TMessage::EGameState::VictoryHost;
            std::cout << "[Game Server] - 2.d Ein Spieler Gewinnt: Beide Spieler sind kollidiert, Client 1 hat mehr Punkte" << std::endl;
        }

        else if (m_PlayerPoints[0] < m_PlayerPoints[1])
        {
            m_pGameInfo->State = TMessage::EGameState::VictoryClient;
            std::cout << "[Game Server] - 2.d Ein Spieler Gewinnt: Beide Spieler sind kollidiert, Client 2 hat mehr Punkte" << std::endl;
        }

        else
        {
            m_pGameInfo->State = TMessage::EGameState::VictoryDraw;
            std::cout << "[Game Server] - 2.d Ein Spieler Gewinnt: Beide Spieler sind kollidiert, unentschieden" << std::endl;
        }
    }
    else if (m_Collision[0] != ECollisionType::NONE)
    {
        m_pGameInfo->State = TMessage::EGameState::VictoryClient;
        std::cout << "[Game Server] - 2.d Ein Spieler Gewinnt: Client 2 ist kollidiert" << std::endl;
    }
    else if (m_Collision[1] != ECollisionType::NONE)
    {
        m_pGameInfo->State = TMessage::EGameState::VictoryHost;
        std::cout << "[Game Server] - 2.d Ein Spieler Gewinnt: Client 1 ist kollidiert" << std::endl;
    }

    if (m_Collision[0] == ECollisionType::NONE && m_Collision[1] == ECollisionType::NONE)
    {
        if (m_PlayerSnake[0].front().Position[0] == m_BonusPosition[0]
            && m_PlayerSnake[0].front().Position[1] == m_BonusPosition[1])
        {
            HandleCollision(ECollisionType::Bonus, 0);
        }
        else if (m_PlayerSnake[1].front().Position[0] == m_BonusPosition[0]
            && m_PlayerSnake[1].front().Position[1] == m_BonusPosition[1])
        {
            HandleCollision(ECollisionType::Bonus, 1);
        }
    }
}

void CGame::HandleCollision(ECollisionType a_collisionType, int a_PlayerID)
{
    switch (a_collisionType)
    {
        case ECollisionType::Bonus:
        {
            if (m_Tick > 0.5)
                m_Tick -= 0.01f;

            m_PlayerPoints[a_PlayerID]++;

            if (a_PlayerID == 0) 
                m_pGameInfo->PlayerHostPoints = m_PlayerPoints[a_PlayerID];

            else 
                m_pGameInfo->PlayerClientPoints = m_PlayerPoints[a_PlayerID];

            if (m_PlayerPoints[a_PlayerID] >= 20)
            {
                if (a_PlayerID == 0)
                {
                    m_pGameInfo->State = TMessage::EGameState::VictoryHost;
                    
                    std::cout << "[Game Server] - 2.d Ein Spieler Gewinnt: Client 1 hat 20 Punkte" << std::endl;
                }

                else if (a_PlayerID == 1)
                {
                    m_pGameInfo->State = TMessage::EGameState::VictoryClient;

                    std::cout << "[Game Server] - 2.d Ein Spieler Gewinnt: Client 2 hat 20 Punkte" << std::endl;
                }
            }

            ExpandSnake(a_PlayerID);
            GenerateNewBonus();
            break;
        }

        case ECollisionType::Snake:
        {

            m_Collision[a_PlayerID] = ECollisionType::Snake;
            break;
        }

        case ECollisionType::Border:
        {

            m_Collision[a_PlayerID] = ECollisionType::Border;
            break;
        }
    }
}

void CGame::GenerateNewBonus()
{
    bool bonusCollision = true;

    std::random_device randomdevice;
    std::mt19937 rng(randomdevice());
    std::uniform_int_distribution<int> uni(1, 126);

    while (bonusCollision)
    {
        bonusCollision = false;

        m_BonusPosition[0] = uni(rng);
        m_BonusPosition[1] = uni(rng);

        for (int i = 0; i < (int)m_PlayerSnake[0].size(); i++)
        {
            if (m_BonusPosition == m_PlayerSnake[0].at(i).Position)
            {
                bonusCollision = true;
            }
        }

        for (int i = 0; i < (int)m_PlayerSnake[1].size(); i++)
        {
            if (m_BonusPosition == m_PlayerSnake[1].at(i).Position)
            {
                bonusCollision = true;
            }
        }
    }

    m_pGameInfo->BonusPos[0] = m_BonusPosition[0];
    m_pGameInfo->BonusPos[1] = m_BonusPosition[1];
}

void CGame::ExpandSnake(int a_playerID)
{
    for (int i = 0; i < 8; i++)
    {
        m_PlayerSnake[a_playerID].push_back(m_PlayerSnake[a_playerID].back());
    }
}

void CGame::SetGameInfo()
{
    for (int i = 0; i < (m_PlayerPoints[0] + 1) * 8; i++)
    {
        m_pGameInfo->PlayerHostPos[i][0] = m_PlayerSnake[0][i].Position[0];
        m_pGameInfo->PlayerHostPos[i][1] = m_PlayerSnake[0][i].Position[1];
    }

    for (int i = 0; i < (m_PlayerPoints[1] + 1) * 8; i++)
    {
        m_pGameInfo->PlayerClientPos[i][0] = m_PlayerSnake[1][i].Position[0];
        m_pGameInfo->PlayerClientPos[i][1] = m_PlayerSnake[1][i].Position[1];
    }

    m_pGameInfo->BlockedDirection[0] = m_BlockedDirection[0];
    m_pGameInfo->BlockedDirection[1] = m_BlockedDirection[1];
}

void CGame::SetPlayerMovement(TMessage::EMessage a_movementDir, int a_playerID)
{
    switch (a_movementDir)
    {
    case TMessage::EMessage::Up:
        if (m_BlockedDirection[a_playerID] != TMessage::EMessage::Up)
        {
            m_PlayerMoveDir[a_playerID][0] = 0;
            m_PlayerMoveDir[a_playerID][1] = -1;
            m_BlockedDirection[a_playerID] = TMessage::EMessage::Down;
        }
        break;
    case TMessage::EMessage::Down:
        if (m_BlockedDirection[a_playerID] != TMessage::EMessage::Down)
        {
            m_PlayerMoveDir[a_playerID][0] = 0;
            m_PlayerMoveDir[a_playerID][1] = 1;
            m_BlockedDirection[a_playerID] = TMessage::EMessage::Up;
        }
        break;
    case TMessage::EMessage::Left:
        if (m_BlockedDirection[a_playerID] != TMessage::EMessage::Left)
        {
            m_PlayerMoveDir[a_playerID][0] = -1;
            m_PlayerMoveDir[a_playerID][1] = 0;
            m_BlockedDirection[a_playerID] = TMessage::EMessage::Right;
        }
        break;
    case TMessage::EMessage::Right:
        if (m_BlockedDirection[a_playerID] != TMessage::EMessage::Right)
        {
            m_PlayerMoveDir[a_playerID][0] = 1;
            m_PlayerMoveDir[a_playerID][1] = 0;
            m_BlockedDirection[a_playerID] = TMessage::EMessage::Left;
        }
        break;
    }
}

float CGame::GetTick()
{
    return m_Tick;
}
