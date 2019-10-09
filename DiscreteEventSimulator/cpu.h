//
// Created by Matthew Cohen on 9/8/19.
//

#ifndef CPU_H
#define CPU_H

#include <iostream>
#include <queue>
#include <fstream>

#include "globals.h"
#include "disk.h"

namespace mc
{
    class Cpu {
    private:
        std::queue<mc::Globals::Event> m_queue;
        std::priority_queue<mc::Globals::Event, std::vector<mc::Globals::Event>, mc::Globals::CompareEvents>* m_eventsPQ;

        // reference to outputFile in main.cpp
        std::ofstream &m_outputFile;

        int m_occupied;

        int m_totalQueueSize;
        int m_totalResponseTime;

        int m_totalEventsHandled;
        int m_totalNumberOfUpdates;
        int m_timeInUse;

        // begins working on a Event when the cpu is free
        void beginProcess(mc::Globals::Event e);
    public:
        Cpu(std::priority_queue<mc::Globals::Event, std::vector<mc::Globals::Event>, mc::Globals::CompareEvents>* eventsPQ, std::ofstream &outputFile);
        void handleArrival(mc::Globals::Event e);
        void handleExit(mc::Globals::Event e, mc::Disk* disk1, mc::Disk* disk2);

        void update();
        // saves the values for the STAT file once simulation is completed
        void end();

        float m_avgQueueSize;
        float m_avgResponseTime;
        int m_maxResponseTime;
        int m_maxQueueSize;
        float m_util;
        float m_throughput;
    };
}

#endif //CPU_H
