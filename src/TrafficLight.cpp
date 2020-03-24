#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lock(_mtx);
    _cv.wait(lock, [this]() {return !_queue.empty();});

    T msg = std::move(_queue.front());
    _queue.pop_front();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mtx);
    _queue.emplace_back(std::move(msg));
    _cv.notify_one();
}


/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{

    while(msgQueue.receive() != TrafficLightPhase::green);
    return;
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    using time_type = std::chrono::time_point<std::chrono::steady_clock>;
    const int interval = 4 + rand() % 3;

    time_type last_toggle_time = std::chrono::steady_clock::now();

    while(true)
    {
        time_type now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_toggle_time).count() >= interval)
        {
            toggleState();
            last_toggle_time = now;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Cleaner solution IMHO, doesn't use active polling
        // std::this_thread::sleep_for(std::chrono::seconds(interval));
        // toggleState();
    }
}


void TrafficLight::toggleState()
{
    if (_currentPhase == TrafficLightPhase::green) _currentPhase = TrafficLightPhase::red;
    else _currentPhase = TrafficLightPhase::green;
    msgQueue.send(std::move(_currentPhase));
}