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

#ifndef IDYNTREE_OPTIMALCONTROL_FORWARDEULER_H
#define IDYNTREE_OPTIMALCONTROL_FORWARDEULER_H

#include <iDynTree/Integrators/FixedStepIntegrator.h>

namespace iDynTree {
    namespace optimalcontrol {

        class DynamicalSystem;

        namespace integrators {

        /**
         * @warning This class is still in active development, and so API interface can change between iDynTree versions.
         * \ingroup iDynTreeExperimental
         */

            class ForwardEuler : public FixedStepIntegrator{

                VectorDynSize m_computationBuffer;
                MatrixDynSize m_stateJacBuffer, m_controlJacBuffer, m_identity, m_zeroBuffer;
                bool m_hasStateSparsity = false;
                bool m_hasControlSparsity = false;
                std::vector<SparsityStructure> m_stateJacobianSparsity;
                std::vector<SparsityStructure> m_controlJacobianSparsity;

                bool allocateBuffers() override;

                bool oneStepIntegration(double t0, double dT, const VectorDynSize& x0, VectorDynSize& x) override;

            public:

                ForwardEuler();

                ForwardEuler(const std::shared_ptr<iDynTree::optimalcontrol::DynamicalSystem> dynamicalSystem);

                virtual ~ForwardEuler() override;

                bool evaluateCollocationConstraint(double time, const std::vector<VectorDynSize> &collocationPoints,
                                                   const std::vector<VectorDynSize> &controlInputs, double dT,
                                                   VectorDynSize &constraintValue) override;

                bool evaluateCollocationConstraintJacobian(double time, const std::vector<VectorDynSize> &collocationPoints,
                                                           const std::vector<VectorDynSize> &controlInputs, double dT,
                                                           std::vector<MatrixDynSize> &stateJacobianValues,
                                                           std::vector<MatrixDynSize> &controlJacobianValues) override;

                virtual bool getCollocationConstraintJacobianStateSparsity(std::vector<SparsityStructure>& stateJacobianSparsity) override;

                virtual bool getCollocationConstraintJacobianControlSparsity(std::vector<SparsityStructure>& controlJacobianSparsity) override;

            };
        }
    }
}

#endif // IDYNTREE_OPTIMALCONTROL_FORWARDEULER_H
