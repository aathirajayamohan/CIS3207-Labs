//
// Created by Matthew Cohen on 9/8/19.
//

#include "cpu.h"

namespace mc
{
    Cpu::Cpu(std::priority_queue<mc::Globals::Event, std::vector<mc::Globals::Event>, mc::Globals::CompareEvents>* eventsPQ, std::ofstream &outputFile) : m_outputFile(outputFile)
    {
        this->m_eventsPQ = eventsPQ;
        m_occupied = 0;
    }

    void Cpu::handleArrival(mc::Globals::Event e)
    {
        // keep track of the max size the queue reaches during the simulation
        if (m_maxQueueSize < m_queue.size())
            m_maxQueueSize = m_queue.size();

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

// TODO: redo function
    void Cpu::handleExit(mc::Globals::Event e, Disk* disk1, Disk* disk2)
    {
        if (CREATE_LOG_FILE)
            m_outputFile << e.time << "," << e.pid << ",Exit CPU\n";
        if (DEBUG_MESSAGES)
            printf("At time %d, process %d exits the CPU\n", e.time, e.pid);

        // begin setup for temp event
        mc::Globals::Event tempEvent;
        tempEvent.pid = e.pid;

        m_totalEventsHandled++;
        m_totalResponseTime += (e.time - e.received);
        if ((e.time - e.received) > m_maxResponseTime)
            m_maxResponseTime = (e.time - e.received);

        m_timeInUse += mc::Globals::currentTime - e.received;

        double randQuitProb = (double)rand() / (double)RAND_MAX;
        if (randQuitProb < mc::Globals::QUIT_PROB)
        {
            // exit event
            tempEvent.type = mc::Globals::PROCESS_EXIT_SYSTEM;
            tempEvent.time = mc::Globals::currentTime;
        }
        else
        {
            // use a disk
            if ((!disk1->isOccupied() && disk1->getQueue()->empty()) || (!disk2->isOccupied() && disk2->getQueue()->empty()))
            {
                // 50/50 chance to choose disk 1 or disk 2 if both are empty and not occupied
                if ( ((double)rand() / RAND_MAX) < 0.5)
                {
                    // choose disk 1
                    tempEvent.type = mc::Globals::PROCESS_ARRIVE_DISK1;
                    tempEvent.time = e.time;
                }
                else
                {
                    // choose disk 2
                    tempEvent.type = mc::Globals::PROCESS_ARRIVE_DISK2;
                    tempEvent.time = e.time;
                }

            }
            // disk 1 queues is shorter
            else if (disk1->getQueue()->size() < disk2->getQueue()->size())
            {
                tempEvent.type = mc::Globals::PROCESS_ARRIVE_DISK1;
                tempEvent.time = e.time;
            }
            // disk 2 queues is shorter
            else
            {
                tempEvent.type = mc::Globals::PROCESS_ARRIVE_DISK2;
                tempEvent.time = e.time;
            }
        }

        m_eventsPQ->push(tempEvent);

        // there are events in the CPU queue that need to be processed
        if (!m_queue.empty())
        {
            m_occupied = 1;
            // remove event from cpu queue
            mc::Globals::Event temp = m_queue.front();
            m_queue.pop();
            beginProcess(temp);
        }
        else
        {
            m_occupied = 0;
        }

    }

    void Cpu::beginProcess(mc::Globals::Event e)
    {
        if (CREATE_LOG_FILE)
            m_outputFile << e.time << "," << e.pid << ",Enter CPU\n";
        if (DEBUG_MESSAGES)
            printf("At time %d, process %d enters CPU\n", e.time, e.pid);
        m_occupied = 1;

        mc::Globals::Event newEvent;
        newEvent.type = mc::Globals::PROCESS_FINISH_CPU;
        newEvent.time = mc::Globals::randomInt(mc::Globals::CPU_MIN, mc::Globals::CPU_MAX) + mc::Globals::currentTime;
        newEvent.received = mc::Globals::currentTime;
        newEvent.pid = e.pid;

        m_eventsPQ->push(newEvent);
    }

    void Cpu::update()
    {
        m_totalNumberOfUpdates++;
        m_totalQueueSize += m_queue.size();
    }

    void Cpu::end()
    {
        m_avgQueueSize = (double)m_totalQueueSize / m_totalNumberOfUpdates;
        m_util = (double)m_timeInUse/mc::Globals::FIN_TIME;
        m_throughput = (double)m_totalEventsHandled/mc::Globals::FIN_TIME;
        m_avgResponseTime = (double)m_totalResponseTime/(m_totalEventsHandled+1);
    }
}