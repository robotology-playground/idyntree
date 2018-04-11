/*
 * Copyright (C) 2014,2017 Fondazione Istituto Italiano di Tecnologia
 * Authors: Francesco Romano, Stefano Dafarra
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 * Originally developed for Prioritized Optimal Control (2014)
 * Refactored in 2017.
 * Design inspired by
 * - ACADO toolbox (http://acado.github.io)
 * - ADRL Control Toolbox (https://adrlab.bitbucket.io/ct/ct_doc/doc/html/index.html)
 */

#include "iDynTree/Integrators/ImplicitTrapezoidal.h"
#include "iDynTree/DynamicalSystem.h"
#include "iDynTree/Core/Utils.h"
#include <Eigen/Dense>
#include "iDynTree/Core/EigenHelpers.h"
#include <cstddef>
#include <sstream>
#include <string>

namespace iDynTree {
    namespace optimalcontrol {
        namespace integrators {

            bool ImplicitTrapezoidal::oneStepIntegration(double t0, double dT, const iDynTree::VectorDynSize &x0, iDynTree::VectorDynSize &x)
            {
                reportError(m_info.name().c_str(), "oneStepIntegration", "The ImplicitTrapezoidal method has not been implemented to integrate a dynamical system yet.");
            }

            ImplicitTrapezoidal::ImplicitTrapezoidal(const std::shared_ptr<DynamicalSystem> dynamicalSystem) : FixedStepIntegrator(dynamicalSystem)
            {
                m_infoData->name = "ImplicitTrapezoidal";
                m_infoData->isExplicit = false;
                m_infoData->numberOfStages = 1;
                m_computationBuffer.resize(m_dynamicalSystem_ptr->stateSpaceSize());
                m_computationBuffer2.resize(m_dynamicalSystem_ptr->stateSpaceSize());

                size_t nx = m_dynamicalSystem_ptr->stateSpaceSize();
                size_t nu = m_dynamicalSystem_ptr->controlSpaceSize();

                m_identity.resize(nx, nx);
                toEigen(m_identity) = iDynTreeEigenMatrix::Identity(nx, nx);

                m_stateJacBuffer.resize(nx, nx);
                m_controlJacBuffer.resize(nx,nu);

            }

            ImplicitTrapezoidal::~ImplicitTrapezoidal()
            {}

            bool ImplicitTrapezoidal::evaluateCollocationConstraint(const std::vector<VectorDynSize> &collocationPoints,
                                                                    const std::vector<VectorDynSize> &controlInputs,
                                                                    double time, VectorDynSize &constraintValue)
            {
                if (collocationPoints.size() != 2){
                    std::ostringstream errorMsg;
                    errorMsg << "The size of the matrix containing the collocation point does not match the expected one. Input = ";
                    errorMsg << collocationPoints.size() << ", Expected = 2.";
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraint", errorMsg.str().c_str());
                    return false;
                }

                if (controlInputs.size() != 2){
                    std::ostringstream errorMsg;
                    errorMsg << "The size of the matrix containing the control inputs does not match the expected one. Input = ";
                    errorMsg << controlInputs.size() << ", Expected = 2.";
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraint", errorMsg.str().c_str());
                    return false;
                }

                if (constraintValue.size() != m_dynamicalSystem_ptr->stateSpaceSize())
                    constraintValue.resize(m_dynamicalSystem_ptr->stateSpaceSize());

                if (!(m_dynamicalSystem_ptr->dynamics(collocationPoints[0], controlInputs[0], time, m_computationBuffer))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraint", "Error while evaluating the dynamical system.");
                    return false;
                }

                if (!(m_dynamicalSystem_ptr->dynamics(collocationPoints[1], controlInputs[1], time+m_dTmax, m_computationBuffer2))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraint", "Error while evaluating the dynamical system.");
                    return false;
                }

                toEigen(constraintValue) = -toEigen(collocationPoints[1]) + toEigen(collocationPoints[0]) +
                        m_dTmax/2*(toEigen(m_computationBuffer)+toEigen(m_computationBuffer2));

                return true;
            }

            bool ImplicitTrapezoidal::evaluateCollocationConstraintJacobian(const std::vector<VectorDynSize> &collocationPoints,
                                                                            const std::vector<VectorDynSize> &controlInputs, double time,
                                                                            std::vector<MatrixDynSize> &stateJacobianValues,
                                                                            std::vector<MatrixDynSize> &controlJacobianValues)
            {
                if (collocationPoints.size() != 2){
                    std::ostringstream errorMsg;
                    errorMsg << "The size of the matrix containing the collocation point does not match the expected one. Input = ";
                    errorMsg << collocationPoints.size() << ", Expected = 2.";
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian", errorMsg.str().c_str());
                    return false;
                }

                if (controlInputs.size() != 2){
                    std::ostringstream errorMsg;
                    errorMsg << "The size of the matrix containing the control inputs does not match the expected one. Input = ";
                    errorMsg << controlInputs.size() << ", Expected = 2.";
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian", errorMsg.str().c_str());
                    return false;
                }

                size_t nx = m_dynamicalSystem_ptr->stateSpaceSize();
                size_t nu = m_dynamicalSystem_ptr->controlSpaceSize();

                //State Jacobians

                if (stateJacobianValues.size() != 2)
                    stateJacobianValues.resize(2);

                if ((stateJacobianValues[0].rows() != nx) || (stateJacobianValues[0].cols() != nx))
                    stateJacobianValues[0].resize(nx,nx);

                if (!(m_dynamicalSystem_ptr->dynamicsStateFirstDerivative(collocationPoints[0], controlInputs[0], time, m_stateJacBuffer))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian",
                                "Error while evaluating the dynamical system jacobian.");
                    return false;
                }

                toEigen(stateJacobianValues[0]) = toEigen(m_identity) + m_dTmax/2*toEigen(m_stateJacBuffer);

                if ((stateJacobianValues[1].rows() != nx) || (stateJacobianValues[1].cols() != nx))
                    stateJacobianValues[1].resize(nx,nx);

                if (!(m_dynamicalSystem_ptr->dynamicsStateFirstDerivative(collocationPoints[1], controlInputs[1], time+m_dTmax, m_stateJacBuffer))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian",
                                "Error while evaluating the dynamical system jacobian.");
                    return false;
                }

                toEigen(stateJacobianValues[1]) = -toEigen(m_identity) + m_dTmax/2*toEigen(m_stateJacBuffer);

                //Control Jacobians

                if (controlJacobianValues.size() != 2)
                    controlJacobianValues.resize(2);

                if ((controlJacobianValues[0].rows() != nx) || (controlJacobianValues[0].cols() != nu))
                    controlJacobianValues[0].resize(nx,nu);

                if (!(m_dynamicalSystem_ptr->dynamicsControlFirstDerivative(collocationPoints[0], controlInputs[0], time, m_controlJacBuffer))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian",
                                "Error while evaluating the dynamical system control jacobian.");
                    return false;
                }

                toEigen(controlJacobianValues[0]) = m_dTmax/2*toEigen(m_controlJacBuffer);

                if ((controlJacobianValues[1].rows() != nx) || (controlJacobianValues[1].cols() != nu))
                    controlJacobianValues[1].resize(nx,nu);

                if (!(m_dynamicalSystem_ptr->dynamicsControlFirstDerivative(collocationPoints[1], controlInputs[1], time, m_controlJacBuffer))){
                    reportError(m_info.name().c_str(), "evaluateCollocationConstraintJacobian",
                                "Error while evaluating the dynamical system control jacobian.");
                    return false;
                }

                toEigen(controlJacobianValues[1]) = m_dTmax/2*toEigen(m_controlJacBuffer);

                return true;

            }


        }
    }
}