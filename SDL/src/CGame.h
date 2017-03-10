#pragma once
#include<Windows.h>
#include<deque>
#include <iostream>
#include <random>

#include "TMessage.h"

class CGame
{
    enum class ECollisionType
    {
        NONE,
        Snake,
        Bonus,
        Border,
    };

    struct SSnakePosition
    {
        uint8_t Position[2] = { 0 };
    };

#pragma region Variables
public:

private:

    TMessage::ServerMessage* m_pGameInfo = nullptr;

    float m_Tick = 0;

    uint8_t m_PlayerMoveDir[2][2] = { 0 };
    TMessage::EMessage m_BlockedDirection[2] = { TMessage::EMessage::Left, TMessage::EMessage::Right };

    uint8_t m_BonusPosition[2] = { 0 };

    int m_PlayerPoints[2] = { 0 };

    std::deque<SSnakePosition> m_PlayerSnake[2];

    ECollisionType m_Collision[2] = { ECollisionType::NONE };

#pragma endregion
#pragma region Methods
public:
    CGame();
    ~CGame();

    bool Initialize(TMessage::ServerMessage* a_pgameInfo);
    void InitializeGame();

    void CalculateTick();


    void SetPlayerMovement(TMessage::EMessage a_movementDir, int a_playerID);

    float GetTick();

private:

    void CalculatePlayerMovement();

    void CheckCollision();
    void HandleCollision(ECollisionType a_collisionType, int a_PlayerID);
    
    void GenerateNewBonus();

    void ExpandSnake(int a_playerID);

    void SetGameInfo();

#pragma endregion
};