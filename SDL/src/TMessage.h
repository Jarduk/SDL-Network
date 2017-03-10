#pragma once
#include <cstdint>

class TMessage
{
public:

    enum class EFieldType : uint8_t
    {
        NONE, 
        Gridboarder,
        PlayerHostHead,
        PlayerClientHead,
        PlayerHostTail,
        PlayerClientTail,
        Bonus,
    };  

    enum class EGameState : uint8_t
    {
        NONE,
        PlayerInput,
        Run,
        VictoryHost,
        VictoryClient,
        VictoryDraw,
    };    

    enum class EMessage : uint8_t
    {
        NONE,
        Up,
        Down,
        Left,
        Right,
        Ping,
        Ready,
        Leave,
    };

    enum class EServerState : uint8_t
    {
        NONE,
        AcceptingClients,
        Lobby,
        Run,
        Victory,
    };

    enum class EClientState : uint8_t
    {
        NONE,
        Connected,
        Ready,
        Run,
    };

    struct ServerMessage
    {
        uint8_t PlayerHostPos[168][2] = { 0 };
        uint8_t PlayerClientPos[168][2] = { 0 };
        uint8_t BonusPos[2] = { 0 };

        EMessage BlockedDirection[2] = { EMessage::Left, EMessage::Right };
        uint8_t PlayerHostPoints = 0;
        uint8_t PlayerClientPoints = 0;
        EGameState State = EGameState::NONE;
    };

    struct ClientMessage
    {
        EMessage Message = EMessage::NONE;
    };
};