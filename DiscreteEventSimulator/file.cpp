#include "file.h"

namespace mc
{
    /*
    Takes the name of a config file as a parameter and
    defines the values for global use. File must be in
    a format that has one string followed by an integer
    per line.
*/
    int readConfigFile(std::string fileName)
    {
        // is used to make sure all 12 required values are read, if not
        // view it as an error as there is a problem with the config file
        int valuesRead = 0;
        std::fstream configFile;
        configFile.open(fileName);

        if (configFile.good())
        {
            std::string name = "";
            float value;

            while (!configFile.eof())
            {
                configFile >> name;

                if (name == "SEED")
                {
                    configFile >> value;
                    mc::Globals::SEED = value;

                    valuesRead++;
                }
                else if (name == "INIT_TIME")
                {
                    configFile >> value;
                    mc::Globals::INIT_TIME = value;

                    valuesRead++;
                }
                else if (name == "FIN_TIME")
                {
                    configFile >> value;
                    mc::Globals::FIN_TIME = value;

                    valuesRead++;
                }
                else if (name == "ARRIVE_MIN")
                {
                    configFile >> value;
                    mc::Globals::ARRIVE_MIN = value;

                    valuesRead++;
                }
                else if (name == "ARRIVE_MAX")
                {
                    configFile >> value;
                    mc::Globals::ARRIVE_MAX = value;

                    valuesRead++;
                }
                else if (name == "QUIT_PROB")
                {
                    configFile >> value;
                    mc::Globals::QUIT_PROB = value;

                    valuesRead++;
                }
                else if (name == "CPU_MIN")
                {
                    configFile >> value;
                    mc::Globals::CPU_MIN = value;

                    valuesRead++;
                }
                else if (name == "CPU_MAX")
                {
                    configFile >> value;
                    mc::Globals::CPU_MAX = value;

                    valuesRead++;
                }
                else if (name == "DISK1_MIN")
                {
                    configFile >> value;
                    mc::Globals::DISK1_MIN = value;

                    valuesRead++;
                }
                else if (name == "DISK1_MAX")
                {
                    configFile >> value;
                    mc::Globals::DISK1_MAX = value;

                    valuesRead++;
                }
                else if (name == "DISK2_MIN")
                {
                    configFile >> value;
                    mc::Globals::DISK2_MIN = value;

                    valuesRead++;
                }
                else if (name == "DISK2_MAX")
                {
                    configFile >> value;
                    mc::Globals::DISK2_MAX = value;

                    valuesRead++;
                }
                else
                {
                    printf("%s: unknown name/value in config file...\n", name.c_str());
                    exit(1);
                }

            }
        }
        else
        {
            printf("Unable to read %s\n", fileName.c_str());
        }

        configFile.close();
        return valuesRead == REQUIRED_NUM_OF_VARS ? 1 : 0;
    }
}