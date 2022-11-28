#pragma once
class Time
{
public:
    void update();

    float time();
    float deltaTime();

private:
    float m_lastFrame;
    float m_deltaTime;
    float m_time;
};