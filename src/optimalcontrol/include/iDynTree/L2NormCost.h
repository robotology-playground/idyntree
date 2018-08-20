/*
 * Copyright (C) 2014,2018 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 *
 * Originally developed for Prioritized Optimal Control (2014)
 * Refactored in 2018.
 * Design inspired by
 * - ACADO toolbox (http://acado.github.io)
 * - ADRL Control Toolbox (https://adrlab.bitbucket.io/ct/ct_doc/doc/html/index.html)
 */


#ifndef IDYNTREE_OPTIMALCONTROL_L2NORMCOST_H
#define IDYNTREE_OPTIMALCONTROL_L2NORMCOST_H

#include <iDynTree/QuadraticLikeCost.h>
#include <iDynTree/TimeVaryingObject.h>
#include <memory>
#include <string>

namespace iDynTree {

    class MatrixDynSize;
    class VectorDynSize;

    namespace optimalcontrol {

        /**
         * @warning This class is still in active development, and so API interface can change between iDynTree versions.
         * \ingroup iDynTreeExperimental
         */

        class L2NormCost : public QuadraticLikeCost {
            class L2NormCostImplementation;
            L2NormCostImplementation *m_pimpl;

        public:

            L2NormCost(const std::string& name, unsigned int stateDimension, unsigned int controlDimension); //assume identity as selector. Set the dimension to 0 to avoid weighting either the state or the cost

            L2NormCost(const std::string& name, const MatrixDynSize& stateSelector, const MatrixDynSize& controlSelector); //The selector premultiplies the state and the control in the L2-norm. Set some dimension to 0 to avoid weighting either the state or the cost

            L2NormCost(const L2NormCost& other) = delete;

            ~L2NormCost();

            void computeConstantPart(bool addItToTheCost = true); //by default the constant part is not added since it adds computational burden. Use it if you need the cost evaluation to be exactly equal to an L2 norm

            bool setStateWeight(const MatrixDynSize& stateWeights);

            bool setStateDesiredPoint(const VectorDynSize& desiredPoint);

            bool setStateDesiredTrajectory(std::shared_ptr<TimeVaryingVector> stateDesiredTrajectory);

            bool setControlWeight(const MatrixDynSize& controlWeights);

            bool setControlDesiredPoint(const VectorDynSize& desiredPoint);

            bool setControlDesiredTrajectory(std::shared_ptr<TimeVaryingVector> controlDesiredTrajectory);

            bool updatStateSelector(const MatrixDynSize& stateSelector);

            bool updatControlSelector(const MatrixDynSize& controlSelector);

        };

    }
}

#endif // IDYNTREE_OPTIMALCONTROL_L2NORMCOST_H