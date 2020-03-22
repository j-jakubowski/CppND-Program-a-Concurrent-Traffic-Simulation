#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include <deque>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum TrafficLightPhase {red, green};

template <class T>
class MessageQueue
{
public:
    void send(T &&msg);
    T receive(void);

private:
    std::deque<T> _queue;
    std::mutex _mtx;;
    std::condition_variable _cv;
};

class TrafficLight : public TrafficObject
{
public:

    // constructor / destructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();
    
    // typical behaviour methods
    void waitForGreen();
    void simulate();

    
private:
    // typical behaviour methods

    void cycleThroughPhases();
    
    MessageQueue<TrafficLightPhase> msgQueue;
    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif