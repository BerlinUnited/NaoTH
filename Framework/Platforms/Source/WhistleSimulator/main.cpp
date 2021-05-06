/*
 * File:   main.cpp
 * Author: schlottb[at]informatik.hu-berlin.de
 *
 * Created on 2017.05.21
 */

#include <glib.h>
#include <glib-object.h>

#include <PlatformInterface/Platform.h>
//#include <Tools/Debug/Stopwatch.h>
#include <ModuleFramework/ModuleManager.h>
#include <ModuleFramework/ModuleCreator.h>

#include <cstring>
#include "WhistleSimulator.h"

using namespace std;
using namespace naoth;

// kind of a HACK, needed by the logsimulator
extern ModuleManager* getModuleManager(Cognition* c);
extern ModuleManager* getModuleManager(Motion* m);

#define TO_STRING_INT(x) #x
#define TO_STRING(x) TO_STRING_INT(x)


void print_info() {
    std::cout << "=========================================="  << std::endl;
    std::cout << "WhistleSimulator compiled on: " << __DATE__ << " at " << __TIME__ << std::endl;

#ifdef REVISION
    std::cout << "Revision number: " << TO_STRING(REVISION) << std::endl;
#endif
#ifdef USER_NAME
    std::cout << "Owner: " << TO_STRING(USER_NAME) << std::endl;
#endif
#ifdef BRANCH_PATH
    std::cout << "Branch path: " << TO_STRING(BRANCH_PATH) << std::endl;
#endif
    std::cout << "==========================================\n"  << std::endl;
}

int main(int argc, char** argv) {
    print_info();

    g_type_init();

    bool backendMode = false;
    bool realTime = false;
    unsigned short port = 5401;
    unsigned short rate = 0;
    unsigned short channels = 0;
    unsigned short samples = 1024;
    unsigned short overlap = 0;

    char* logpath = getenv("NAOTH_AUDIOFILE");
    if (logpath == NULL && argc > 1) {
        logpath = argv[argc - 1];
    }

    // search for parameters
    for (int i = 1; i <= argc - 1; i++) {

        if (strcmp(argv[i], "-h") == 0) {
            std::cout << "syntax: (-h)? (-p <port number>)? <logfile>" << std::endl;
            std::cout << "\"--port\" debug port number, range of valid values: [1,65535]" << std::endl;
            std::cout << "\"-a\" autoplay the audio file immediately" << std::endl;
            std::cout << "\"--rate\" sample rate of raw audio file (8000, 22050, 32000, 44100, 48000)" << std::endl;
            std::cout << "\"--channels\" channel count of raw audio file (1, 2, 3, 4)" << std::endl;
            std::cout << "\"--samples\" amount of samples to provide audio file" << std::endl;
            std::cout << "\"--overlap\" amount of samples overlap" << std::endl;
            std::cout << "\"-h\" help" << std::endl;
            return (EXIT_SUCCESS);
        }
        if (strcmp(argv[i], "--port") == 0) {
            port = (unsigned short)strtol(argv[++i], 0, 10);
            if (port == 0) {
                cerr << "invalid port number" << endl;
                return (EXIT_FAILURE);
            }
        }

        if (strcmp(argv[i], "--rate") == 0) {
            rate = (unsigned short)strtol(argv[++i], 0, 10);
            if (!(rate == 8000 || rate == 32000 || rate == 44100 || rate == 22050 || rate == 48000)) {
                cerr << "invalid sample rate " << endl;
                return (EXIT_FAILURE);
            }
        }

        if (strcmp(argv[i], "--channels") == 0) {
            channels = (unsigned short)strtol(argv[++i], 0, 10);
            if (channels < 1 || channels > 4) {
                cerr << "invalid channel count" << endl;
                return (EXIT_FAILURE);
            }
        }

        if (strcmp(argv[i], "--samples") == 0) {
            samples = (unsigned short)strtol(argv[++i], 0, 10);
            if (samples == 0) {
                cerr << "invalid samples count" << endl;
                return (EXIT_FAILURE);
            }
        }

        if (strcmp(argv[i], "--overlap") == 0) {
            overlap = (unsigned short)strtol(argv[++i], 0, 10);
            if (overlap == 0 || overlap > samples) {
                cerr << "invalid overlap count" << endl;
                return (EXIT_FAILURE);
            }
        }
    }
    if (logpath == NULL) {
        cerr << "You need to give the path to the logfile as argument" << endl;
        cerr << "arguments: (-h)? (-p <port number>)? <logfile>" << endl;
        cerr << "\"--port\" debug port number, range of valid values: [1,65535]" << std::endl;
        cerr << "\"-h\" help" << endl;
        return (EXIT_FAILURE);
    }


    // create the simulator instance
    WhistleSimulator sim(logpath, backendMode, realTime, port);

    sim.setParams(channels, rate, samples, overlap);

    // init the platform
    Platform::getInstance().init(&sim);

    //init_agent(sim);
    Cognition* theCognition = createCognition();
    Motion* theMotion = createMotion(); // crashes at inverse kinematics

    // ACHTUNG: C-Cast (!)
    ModuleManager* theCognitionManager = getModuleManager(theCognition);
    if (!theCognitionManager) {
        std::cerr << "ERROR: theCognition is not of type ModuleManager" << std::endl;
        return (EXIT_FAILURE);
    }

    // register processes
    sim.registerCognition((naoth::Callable*)(theCognition));
    sim.registerMotion((naoth::Callable*)(theMotion));

    // start the execution
    sim.main();

    deleteCognition(theCognition);
    deleteMotion(theMotion);

    return (EXIT_SUCCESS);
}
