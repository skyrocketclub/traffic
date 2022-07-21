#include <iostream>
#include <random>
#include "TrafficLight.h"
#include <time.h> // for the time between cycles


/* Implementation of class "MessageQueue" */



template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 

    std::unique_lock<std::mutex> lock(_mt);
    _cd.wait(lock);
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <class T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mt);
    _queue.emplace_back(std::move(msg));
    _cd.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

TrafficLight::~TrafficLight(){
    
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
   
    while(true){
        TrafficLightPhase current = mq.receive();
        if(current == TrafficLightPhase::green){
            return;
        }
    }
    return;
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    //Use the thread queue to start the method cycleThroughPhases
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases,this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
        std::random_device rand;
        std::uniform_int_distribution<> dis(4000,6000);
        std::mt19937 gen(rand());
        std::chrono::time_point<std::chrono::system_clock> startWatch = std::chrono::system_clock::now();
        long cycleTime = dis(gen);

        while(true){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::chrono::time_point<std::chrono::system_clock> timeSpent = std::chrono::system_clock::now();
            if(std::chrono::duration_cast<std::chrono::milliseconds>(timeSpent - startWatch).count()>=cycleTime){
                     if(_currentPhase ==red){
                    _currentPhase == green;
                }
                else{
                    _currentPhase == red;
                }
                //send update using move semantics
                mq.send(std::move(_currentPhase));
                long cycleTime = rand()%2000 + 4000;
                startWatch = std::chrono::system_clock::now();
            
            }   
        }        
}

