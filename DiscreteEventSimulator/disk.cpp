//
// Created by Matthew Cohen on 9/9/19.
//

#include "disk.h"

namespace mc
{
    Disk::Disk(std::priority_queue<mc::Globals::Event, std::vector<mc::Globals::Event>, mc::Globals::CompareEvents>* eventsPQ, DiskType disk, std::ofstream &outputFile) : m_outputFile(outputFile)
    {
        this->m_eventsPQ = eventsPQ;
        this->m_diskType = disk;
        m_maxQueueSize = 0;
        m_totalResponseTime = 0;
        m_maxResponseTime = 0;
        m_totalEventsHandled = 0;
        m_avgQueueSize = 0.0f;
        m_totalNumberOfUpdates = 0;
        m_totalQueueSize = 0;
        m_timeInUse = 0;
    }

    void Disk::handleArrival(mc::Globals::Event e)
    {
        if (CREATE_LOG_FILE)
            m_outputFile << e.time << "," << e.pid << ",Begin Disk " << m_diskType << " I/O\n";
        if (DEBUG_MESSAGES)
            printf("At time %d, process %d begins I/O on Disk %d\n", e.time, e.pid, m_diskType);

        // keep track of the max size the queue reaches during the simulation
        if (m_maxQueueSize < m_queue.size())
            m_maxQueueSize = m_queue.size();

        // currently working on a event or events are waiting to be processed
        // add event to queue
        if (!m_occupied && m_queue.empty())
        {
            beginProcess(e);
        }
        else
        {
            m_queue.push(e);
        }
    }

    void Disk::beginProcess(mc::Globals::Event e)
    {
        m_occupied = 1;
        mc::Globals::Event diskFinish;
        diskFinish.pid = e.pid;
        diskFinish.received = mc::Globals::currentTime;

        // find the disk the event belongs to then
        // determine how long the disk needs to run
        switch (e.type)
        {
            case mc::Globals::PROCESS_ARRIVE_DISK1:
                diskFinish.type = mc::Globals::PROCESS_FINISH_DISK1;
                diskFinish.time = mc::Globals::currentTime + mc::Globals::randomInt(mc::Globals::DISK1_MIN, mc::Globals::DISK1_MAX);
                break;
            case mc::Globals::PROCESS_ARRIVE_DISK2:
                diskFinish.type = mc::Globals::PROCESS_FINISH_DISK2;
                diskFinish.time = mc::Globals::currentTime + mc::Globals::randomInt(mc::Globals::DISK2_MIN, mc::Globals::DISK2_MAX);
                break;
            default:
                break;
        }

        m_eventsPQ->push(diskFinish);
    }

    void Disk::handleExit(mc::Globals::Event e)
    {
        // TODO: determine the quit probability or send back to cpu
        if (CREATE_LOG_FILE)
            m_outputFile << e.time << "," << e.pid << ",Finish Disk " << m_diskType << " I/O\n";
        if (DEBUG_MESSAGES)
            printf("At time %d, process %d finishes I/O on Disk %d\n", e.time, e.pid, m_diskType);

        m_totalEventsHandled++;
        m_totalResponseTime += (e.time - e.received);
        if ((e.time - e.received) > m_maxResponseTime)
            m_maxResponseTime = (e.time - e.received);

        mc::Globals::Event newEvent ;
        newEvent.type = mc::Globals::PROCESS_ARRIVE_CPU;
        newEvent.pid = e.pid;
        newEvent.time = mc::Globals::currentTime;

        m_timeInUse += mc::Globals::currentTime - e.received;

        m_eventsPQ->push(newEvent);

        // check for waiting events in the queue
        if (!m_queue.empty())
        {
            mc::Globals::Event temp = m_queue.front();
            m_queue.pop();
            beginProcess(temp);
        }
        else
        {
            m_occupied = 0;
        }
    }

    bool Disk::isOccupied()
    {
        return m_occupied;
    }

    std::queue<mc::Globals::Event>* Disk::getQueue()
    {
        return &m_queue;
    }

    void Disk::update()
    {
        m_totalNumberOfUpdates++;
        m_totalQueueSize += m_queue.size();
    }

    void Disk::end()
    {
        m_avgQueueSize = (double)m_totalQueueSize / m_totalNumberOfUpdates;
        m_util = (double)m_timeInUse/mc::Globals::FIN_TIME;
        m_throughput = (double)m_totalEventsHandled/mc::Globals::FIN_TIME;
        m_avgResponseTime = (double)m_totalResponseTime/(m_totalEventsHandled+1);
    }
}