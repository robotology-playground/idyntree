/*
 * Copyright (C) 2015 Fondazione Istituto Italiano di Tecnologia
 * Authors: Silvio Traversaro
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef IDYNTREE_INDICES_H
#define IDYNTREE_INDICES_H

#include <string>

namespace iDynTree
{
    typedef int LinkIndex;
    extern LinkIndex LINK_INVALID_INDEX;
    extern std::string LINK_INVALID_NAME;

    typedef int JointIndex;
    extern int JOINT_INVALID_INDEX;
    extern std::string JOINT_INVALID_NAME;

    typedef int DOFIndex;
    extern int DOF_INVALID_INDEX;
    extern std::string DOF_INVALID_NAME;


    typedef int FrameIndex;
    extern int FRAME_INVALID_INDEX;
    extern std::string FRAME_INVALID_NAME;

    typedef int TraversalIndex;
    extern TraversalIndex TRAVERSAL_INVALID_INDEX;

}

#endif /* IDYNTREE_INDICES_H */
