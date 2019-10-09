//
// Created by Matthew Cohen on 2019-09-07.
//

#include "globals.h"

namespace mc
{
    namespace Globals
    {
        int SEED = 0;
        int FIN_TIME = 0;
        int ARRIVE_MIN = 0;
        int INIT_TIME = 0;
        int ARRIVE_MAX = 0;
        float QUIT_PROB = 0;
        int CPU_MIN = 0;
        int CPU_MAX = 0;
        int DISK1_MIN = 0;
        int DISK1_MAX = 0;
        int DISK2_MIN = 0;
        int DISK2_MAX = 0;

        int currentTime = 0;
        unsigned int pidCounter = 1;

        int randomInt(int min, int max)
        {
            return (rand() % (max - min + 1)) + min;
        }
    }

}