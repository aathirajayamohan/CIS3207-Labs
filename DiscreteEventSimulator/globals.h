#ifndef CONST_H
#define CONST_H

#define DEBUG_MESSAGES 0
#define CREATE_LOG_FILE 1

#include <stdlib.h>

/*
 * Global constants to be used through out the program
*/

namespace mc
{
    namespace Globals
    {
        extern int SEED;
        extern int FIN_TIME;
        extern int ARRIVE_MIN;
        extern int INIT_TIME;
        extern int ARRIVE_MAX;
        extern float QUIT_PROB;
        extern int CPU_MIN;
        extern int CPU_MAX;
        extern int DISK1_MIN;
        extern int DISK1_MAX;
        extern int DISK2_MIN;
        extern int DISK2_MAX;

        extern int currentTime;
        extern unsigned int pidCounter;


        /*
         * System Events Types
         *
         * PROCESS_ARRIVAL -
         * CPU_FINISH -
         * */

        enum EVENT_TYPE {
            PROCESS_ARRIVAL,
            PROCESS_ARRIVE_CPU,
            PROCESS_FINISH_CPU,
            PROCESS_EXIT_SYSTEM,
            PROCESS_ARRIVE_DISK1,
            PROCESS_FINISH_DISK1,
            PROCESS_ARRIVE_DISK2,
            PROCESS_FINISH_DISK2,
            SIMULATION_FINISHED
        };

        typedef struct {
            int time;
            int received;
            unsigned int pid;
            EVENT_TYPE type;
        } Event;

        struct CompareEvents {
            bool operator()(Event const& e1, Event const& e2) {
                return e1.time > e2.time;
            }
        };

        int randomInt(int min, int max);
    }

}

#endif