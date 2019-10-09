#ifndef FILE_H
#define FILE_H

#define REQUIRED_NUM_OF_VARS 12


#include <string>
#include <fstream>
#include "globals.h"

namespace mc
{


    /**
     * \brief Read values from a text file and stores them for global use later
     * \param fileName The name of the config to load
     * \return 1 if all REQUIRED_NUM_OF_VARS are loaded, 0 if file is formatted incorrectly
    */
    int readConfigFile(std::string fileName);
}

#endif