/*!
 * @file CubicSpline.h
 * @author Stefano Dafarra
 * @copyright 2016 iCub Facility - Istituto Italiano di Tecnologia
 *            Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * @date 2017
 *
 */


#ifndef IDYNTREE_CUBIC_SPLINE_H
#define IDYNTREE_CUBIC_SPLINE_H

#include <vector>
#include <iDynTree/Core/VectorFixSize.h>
#include <iDynTree/Core/VectorDynSize.h>

namespace iDynTree
{
    class CubicSpline {
        std::vector< iDynTree::Vector4 > m_coefficients;
        iDynTree::VectorDynSize m_velocities;
        iDynTree::VectorDynSize m_time;
        iDynTree::VectorDynSize m_y;
        iDynTree::VectorDynSize m_T;
        
        double m_v0;
        double m_vf;
        double m_a0;
        double m_af;
        
        bool computePhasesDuration();
        bool computeIntermediateVelocities();
        bool computeCoefficients();
        
        
    public:
        CubicSpline();
        ~CubicSpline();
        
        bool setData(const iDynTree::VectorDynSize& time, const iDynTree::VectorDynSize& yData);
        void setInitialConditions(double initialVelocity, double initialAcceleration);
        void setFinalConditions(double finalVelocity, double finalAcceleration);
        double evaluatePoint(double t);
    };
}

#endif