/*
 * Copyright (C) 2013 IIT - Istituto Italiano di Tecnologia
 * Author: Silvio Traversaro
 * CopyPolicy: Released under the terms of the GNU LGPL v2.0 (or any later version)
 *
 */

#include "iCub/iDynTree/yarp_iDynTree.h"

#include <yarp/math/Math.h>
#include <cstring>

using namespace yarp::math;



bool YarptoiDynTree(const yarp::sig::Vector & yarpVector, iDynTree::Wrench & iDynTreeWrench)
{
    if( yarpVector.size() != 6 )
    {
        return false;
    }

    memcpy(iDynTreeWrench.getLinearVec3().data(),yarpVector.data(),3*sizeof(double));
    memcpy(iDynTreeWrench.getAngularVec3().data(),yarpVector.data()+3,3*sizeof(double));
    return true;
}


bool iDynTreetoYarp(const iDynTree::Wrench & iDynTreeWrench,yarp::sig::Vector & yarpVector)
{
    if( yarpVector.size() != 6 )
    {
        yarpVector.resize(6);
    }

    memcpy(yarpVector.data(),iDynTreeWrench.getLinearVec3().data(),3*sizeof(double));
    memcpy(yarpVector.data()+3,iDynTreeWrench.getAngularVec3().data(),3*sizeof(double));
    return true;
}

bool YarptoiDynTree(const yarp::sig::Vector& yarpVector, iDynTree::Position& iDynTreePosition)
{
    if( yarpVector.size() != 3 )
    {
        return false;
    }

    memcpy(iDynTreePosition.data(),yarpVector.data(),3*sizeof(double));
    return true;
}

bool iDynTreetoYarp(const iDynTree::Position& iDynTreePosition, yarp::sig::Vector& yarpVector)
{
    if( yarpVector.size() != 3 )
    {
        yarpVector.resize(3);
    }

    memcpy(yarpVector.data(),iDynTreePosition.data(),3*sizeof(double));
    return true;
}




