/******************************************************************************
 *                                                                            *
 * Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

/**
 * @file main.cpp
 * @authors: Silvio Traversaro <silvio.traversaro@iit.it>
 */

#include <cstdlib>
#include <yarp/os/all.h>

#include "robotstatepublisher.h"

using namespace std;


/****************************************************************/
int main(int argc, char *argv[])
{
    // initialize yarp network:
    // the very first thing to do
    yarp::os::Network yarp;

    // load command-line options
    yarp::os::ResourceFinder rf;
    rf.configure(argc,argv);

    // print available command-line options
    // upon specifying --help; refer to xml
    // descriptor for the relative documentation
    if (rf.check("help"))
    {
        cout<<"Options"<<endl;
        cout<<"\t--name                   <name>: prefix of the yarprobotstatepublisher ports"<<endl;
        cout<<"\t--prefix                 <prefix-name>: prefix of the published TFs"<<endl;
        cout<<"\t--model                  <file-name>: file name of the model to load at startup"<<endl;
        cout<<"\t--base-frame             <frame-name>: specify the base frame of the published tf tree"<<endl;
        cout<<"\t--jointstates-topic      <topic-name>: source topic that streams the joint state"<<endl;
        return EXIT_SUCCESS;
    }

    if (!yarp.checkNetwork(10.0))
    {
        yError()<<"YARP network is not available";
        return EXIT_FAILURE;
    }

    YARPRobotStatePublisherModule statepublisher;
    return statepublisher.runModule(rf);
}

