/**
 * @file simple_timer.h
 * @brief Some functionality to implement a basic profiler
 * 
 * @author Daniel Calderón
 * @license MIT
*/

#include <chrono>
#include <iostream>
#include <string>
#include <iomanip>

namespace Grafica
{

template <typename LogProfileFunction>
class Timer
{
public:
    Timer(const char* name, LogProfileFunction&& logProfileFunction)
        : _name(name), _logProfileFunction(logProfileFunction), _stopped(false)
    {
        _startTime = std::chrono::steady_clock::now();
    }

    ~Timer()
    {
        if (!_stopped)
        {
            stop();
        }
    }

    void stop()
    {
        const std::chrono::time_point<std::chrono::steady_clock> endTime = std::chrono::steady_clock::now();
        const std::chrono::duration<float, std::milli> duration = endTime - _startTime;
        const float durationAsFloat = duration.count();

        _stopped = true;

        //std::cout << _name << ": "<< durationAsFloat << "ms" << std::endl;
        _logProfileFunction({ _name , durationAsFloat });
    }

private:
    const char* _name;
    LogProfileFunction _logProfileFunction;
    bool _stopped;
    std::chrono::time_point<std::chrono::steady_clock> _startTime;
};

struct ProfileResult
{
    std::string timerName;
    float duration;
};

ProfileResult res("h", 2.0f);



std::ostream& operator<<(std::ostream& os, const ProfileResult& result)
{
    return os << std::fixed << std::setprecision(3) << std::setw(2) << result.duration << "ms " << result.timerName;
}

} // Grafica

#define PROFILE_SCOPE(name, stats) Grafica::Timer timer##__LINE__(name, [&stats](const Grafica::ProfileResult& profileResult) { stats.push_back(profileResult); })
