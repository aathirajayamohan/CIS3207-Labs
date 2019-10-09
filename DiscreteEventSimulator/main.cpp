#include <queue>
#include <stdlib.h>

#include "file.h"
#include "cpu.h"
#include "disk.h"


// Functions
void initSimulation(void);
void handleEvent(mc::Globals::Event e);
void processArrival(mc::Globals::Event e);

// main priority queue to store the events
std::priority_queue<mc::Globals::Event, std::vector<mc::Globals::Event>, mc::Globals::CompareEvents> eventsPQ;

// output file
std::ofstream outputFile;

mc::Cpu cpu(&eventsPQ, outputFile);
mc::Disk disk1(&eventsPQ, mc::DiskType::DISK_1, outputFile);
mc::Disk disk2(&eventsPQ, mc::DiskType::DISK_2, outputFile);

int m_simFinished = 0;

// below vars are used for STAT file for the priority queue
int totalNumberOfUpdates = 0;
int totalQueueSize = 0;
int maxQueueSize = 0;
float avgQueueSize = 0.0f;

int main()
{
    // read values from config file and only continue if the file was
    if (mc::readConfigFile("config.txt"))
    {
        // read successfully continue with setup
        initSimulation();
        // use the SEED provided for random numbers
        srand(mc::Globals::SEED);

        // loop until priority queue is empty
        while (!m_simFinished) {
            // set the current time to the time of the event removed
            mc::Globals::Event temp = eventsPQ.top();
            mc::Globals::currentTime = temp.time;
            eventsPQ.pop();
            handleEvent(temp);

            // keep track of the events queue size fro STAT file
            totalQueueSize += eventsPQ.size();
            if (maxQueueSize < eventsPQ.size())
                maxQueueSize = eventsPQ.size();

            cpu.update();
            disk1.update();
            disk2.update();
            totalNumberOfUpdates++;
        }
        // close the log file stream
        outputFile.close();

        cpu.end();
        disk1.end();
        disk2.end();

        avgQueueSize = (double)totalQueueSize / totalNumberOfUpdates;

        // Create the STAT file
        outputFile.open("stat.txt");

        outputFile << "----------------Queues----------------\n";
        outputFile << "Average size of CPU Queue: " << cpu. m_avgQueueSize << '\n';
        outputFile << "Max size of CPU Queue: " << cpu.m_maxQueueSize << '\n';
        outputFile << "Average size of DISK 1 Queue: " << disk1.m_avgQueueSize << '\n';
        outputFile << "Max size of DISK 1 Queue: " << disk1.m_maxQueueSize << '\n';
        outputFile << "Average size of DISK 2 Queue: " << disk2.m_avgQueueSize << '\n';
        outputFile << "Max size of DISK 2 Queue: " << disk2.m_maxQueueSize << '\n';
        outputFile << "Average size of Event Queue: " << avgQueueSize << '\n';
        outputFile << "Max size of Event Queue: " << maxQueueSize << '\n';

        outputFile << "\n----------------Utilization----------------\n";
        outputFile << "Utilization of CPU: " << cpu.m_util << '\n';
        outputFile << "Utilization of Disk 1: " << disk1.m_util << '\n';
        outputFile << "Utilization of Disk 2: " << disk2.m_util << '\n';

        outputFile << "\n----------------Response Time----------------\n";
        outputFile << "CPU max response time: " << cpu.m_maxResponseTime << '\n';
        outputFile << "DISK 1 max response time: " << disk1.m_maxResponseTime << '\n';
        outputFile << "DISK 2 max response time: " << disk2.m_maxResponseTime << '\n';
        outputFile << "CPU average response time: " << cpu.m_avgResponseTime << '\n';
        outputFile << "DISK 1 average response time: " << disk1.m_avgResponseTime << '\n';
        outputFile << "DISK 2 average response time: " << disk2.m_avgResponseTime << '\n';

        outputFile << "\n----------------Throughput----------------\n";
        outputFile << "Throughput of CPU: " << cpu.m_throughput << '\n';
        outputFile << "Throughput of DISK 1: " << disk1.m_throughput << '\n';
        outputFile << "Throughput of DISK 2: " << disk2.m_throughput << '\n';

        outputFile.close();

        puts("Simulation ran successfully");
    }
    return 0;
}

void initSimulation()
{
    // setup log file
    outputFile.open("log.csv");
    outputFile << "Time,PID,Event\n";

    mc::Globals::Event arrivalEvent;
    arrivalEvent.time = mc::Globals::INIT_TIME;
    arrivalEvent.pid = mc::Globals::pidCounter;
    arrivalEvent.type = mc::Globals::PROCESS_ARRIVAL;

    mc::Globals::Event finishedEvent;
    finishedEvent.time = mc::Globals::FIN_TIME;
    finishedEvent.pid = mc::Globals::pidCounter++;
    finishedEvent.type = mc::Globals::SIMULATION_FINISHED;

    eventsPQ.push(arrivalEvent);
    eventsPQ.push(finishedEvent);
}

void handleEvent(mc::Globals::Event e) {

    switch (e.type) {
        case mc::Globals::PROCESS_ARRIVAL:
            processArrival(e);
            break;
        case mc::Globals::PROCESS_ARRIVE_CPU:
            cpu.handleArrival(e);
            break;
        case mc::Globals::PROCESS_FINISH_CPU:
            cpu.handleExit(e, &disk1, &disk2);
            break;
        case mc::Globals::PROCESS_EXIT_SYSTEM:
            if (CREATE_LOG_FILE)
                outputFile << e.time << "," << e.pid << ",System Exit\n";
            if (DEBUG_MESSAGES)
                printf("At time %d, process %d exits the system\n", e.time, e.pid);
            break;
        case mc::Globals::PROCESS_ARRIVE_DISK1:
            disk1.handleArrival(e);
            break;
        case mc::Globals::PROCESS_ARRIVE_DISK2:
            disk2.handleArrival(e);
            break;
        case mc::Globals::PROCESS_FINISH_DISK1:
            disk1.handleExit(e);
            break;
        case mc::Globals::PROCESS_FINISH_DISK2:
            disk2.handleExit(e);
            break;
        case mc::Globals::SIMULATION_FINISHED:
            m_simFinished = 1;
            break;
        default:
            break;
    }

}

void processArrival(mc::Globals::Event e)
{
    if (CREATE_LOG_FILE)
        outputFile << e.time << "," << e.pid << ",System Enter\n";
    if (DEBUG_MESSAGES)
        printf("At time %d, process %d enters system\n", e.time, e.pid);

    mc::Globals::Event newEvent;
    newEvent.time = mc::Globals::currentTime;
    newEvent.pid = e.pid;
    newEvent.type = mc::Globals::PROCESS_ARRIVE_CPU;

    eventsPQ.push(newEvent);

    // the new time to use for the new event
    int nt = mc::Globals::randomInt(mc::Globals::ARRIVE_MIN, mc::Globals::ARRIVE_MAX) + mc::Globals::currentTime;

    // keep adding new events
    mc::Globals::Event temp;
    temp.time = nt;
    temp.pid = mc::Globals::pidCounter++;
    temp.type = mc::Globals::PROCESS_ARRIVAL;

    eventsPQ.push(temp);
}