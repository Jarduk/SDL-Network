#include "CTimer.h"

CTimer::CTimer()
{
    m_Start = clock();
}

CTimer::~CTimer()
{

}

void CTimer::Update()
{
    m_End = clock();

    m_DeltaTime = (m_End - m_Start) / (float)CLOCKS_PER_SEC;

    m_Start = clock();
}

float CTimer::GetDeltaTime()
{
    return m_DeltaTime;
}

float CTimer::GetTime()
{
    return 0;
}

