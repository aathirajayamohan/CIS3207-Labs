//
// Created by Matthew Cohen on 9/9/19.
//

#ifndef DISK_H
#define DISK_H

#include <queue>

#include "globals.h"
#include <fstream>

namespace mc
{
    enum DiskType {
        DISK_1,
        DISK_2
    };

    class Disk {
    protected:
        std::queue<mc::Globals::Event> m_queue;
        std::priority_queue<mc::Globals::Event, std::vector<mc::Globals::Event>, mc::Globals::CompareEvents>* m_eventsPQ;

        // reference to outputFile in main.cpp
        std::ofstream &m_outputFile;

        // is used to tell which disk is which when there are multiple
        DiskType m_diskType;

        bool m_occupied;

        // vars for STAT file
        int m_totalQueueSize;
        int m_totalResponseTime;
        int m_totalEventsHandled;
        int m_totalNumberOfUpdates;
        int m_timeInUse;

        void beginProcess(mc::Globals::Event e);

    public:
        // main constructor
        Disk(std::priority_queue<mc::Globals::Event, std::vector<mc::Globals::Event>, mc::Globals::CompareEvents>* eventsPQ, DiskType disk, std::ofstream &outputFile);
        void handleArrival(mc::Globals::Event e);
        void handleExit(mc::Globals::Event e);
        bool isOccupied();

        std::queue<mc::Globals::Event>* getQueue();
        void end();

        void update();

        // vars for STAT file
        float m_avgQueueSize;
        float m_avgResponseTime;
        int m_maxResponseTime;
        int m_maxQueueSize;
        float m_util;
        float m_throughput;

    };
}



#endif //DISK_H
