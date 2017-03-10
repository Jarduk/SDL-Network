#pragma once

#include <ctime>

class CTimer
{
#pragma region Variables
public:
    std::clock_t m_Start;
    std::clock_t m_End;

    float m_DeltaTime;

private:

#pragma endregion
#pragma region Methods
public:
    CTimer();
    ~CTimer();

    void Update();

    float GetDeltaTime();
    float GetTime();

private:

#pragma endregion
};
