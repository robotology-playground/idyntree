/*
 * Copyright (C) 2016 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 */

#ifndef IDYNTREE_INTERNAL_INVERSEKINEMATICSNLP_H
#define IDYNTREE_INTERNAL_INVERSEKINEMATICSNLP_H

#include <IpTNLP.hpp>
#include <iDynTree/Core/VectorFixSize.h>
#include <iDynTree/Core/VectorDynSize.h>
#include <iDynTree/Core/MatrixDynSize.h>
#include <iDynTree/Core/Twist.h>
#include <iDynTree/Core/Transform.h>

#include <map>

// use expression as sub-expression,
// then make type of full expression int, discard result
#define UNUSED_VARIABLE(x) (void)(sizeof((x), 0))

namespace internal {
namespace kinematics {
    class InverseKinematicsNLP;
    class InverseKinematicsData;
}
}

/*!
 * @brief Implementation of the inverse kinematics with IPOPT
 *
 * Implements the following optimization problem
 * \f{align}{
 * \min_{x} & \Sum_i || w_H_{f_i} (w_H_{f_i}^d)^{-1} ||^2 + w_q || q_j - q_j^d ||^2 \\
 * \text{s.t.} & w_H_{f_i} = w_H_{f_i}^d \\
 *             & q_{min} \leq q \leq q_{max}
 * \f}
 * where
 * \f{align}{
 * & q = \{w_H_{base}, q_j\} \in SE(3) \times \mathbb{R}^n \\
 * & w_H_{f_i} \in SE(3)
 * \f}
 *
 * A target (entirely or partially, with partial meaning the two components composing
 * a tranform, i.e. position and orientation) can be enforced by either considering
 * it as a constraint or as a cost
 *
 * \note Implementation details
 * this class coordinates with InverseKinematicsData to manages internal data
 * In particular to manages constraints and targets it uses an std::map which is an
 * ordered container. Once the order is defined at configuration time, it is exploited
 * during the optimization by iterating on its elements. As this order is the same as the
 * IPOPT variables the fact that we use a map instead of an unordered_map (which is faster
 * at construction and for random access) is relevant.
 *
 * @todo specify weight for different targets
 * @todo together with InverseKinematicsData there is some cleanup to do
 * as remove unnecessary variables. This is needed for two reasons:
 * - it has never been optimized
 * - we migrated from iDynTree full Model to reduced Model, so we now have equivalence
 *   between iDynTree DoFs and optimizer DoFs which was not present before
 */
class internal::kinematics::InverseKinematicsNLP : public Ipopt::TNLP {

    /*! @brief information about a Frame during optimization
     * All the values are computed given the current robot configuration
     * i.e. the one specified in the IPOPT callbacks (x)
     */
    struct FrameInfo {
        iDynTree::Transform transform; /*!< frame w.r.t. global frame, i.e. \f$ {}^w R_f \f$ */
        iDynTree::MatrixDynSize jacobian; /*!< Jacobian */
        iDynTree::MatrixFixSize<4, 3> quaternionDerivativeMap; /*!< map used during the derivative if the quaternion representation is used */
    };
    typedef std::map<int, FrameInfo> FrameInfoMap;

    InverseKinematicsData& m_data; /*!< Reference to the InverseKinematicsData object. Non IPOPT-specific data are saved and accessed in that object */

    //Buffers and variables used in the optimization
    iDynTree::MatrixFixSize<3, 4> quaternionDerivativeInverseMapBuffer; /*!< this is used to contain the quaternionDerivativeInverseMap, computed once for each optimization step */
    iDynTree::MatrixDynSize finalJacobianBuffer; /*!< Buffer to contain the Jacobian as modified to handle quaternions */

    FrameInfoMap constraintsInfo; /*!< FrameInfo map for the constraints */
    FrameInfoMap targetsInfo; /*!< FrameInfo map for the targets */

    struct COMInfo {
        iDynTree::Position com; /*!< COM position w.r.t. global frame */
        iDynTree::Vector2 projectedCom; /*!< COM projection in the support plane */
        iDynTree::MatrixDynSize comJacobian; /*!< 3 X (6 + nDofs) center of mass jacobian */
        iDynTree::MatrixDynSize comJacobianAnalytical; /*!< 3 x ( 3 + sizeOfRotationParams + nDofs) processed jacobian */
        iDynTree::MatrixDynSize projectedComJacobian; /*!< 2 x ( 3 + sizeOfRotationParams + nDofs) processed jacobian */
    };

    COMInfo comInfo;

    //Temporary optimized variables
    iDynTree::Position optimizedBasePosition; /*!< Hold the base frame origin at an optimization step */
    iDynTree::Vector4 optimizedBaseOrientation; /*!< Hold the base frame orientation at an optimization step. Note that if orientation is RPY, the last component should not be accessed */
    iDynTree::VectorDynSize jointsAtOptimisationStep; /*!< Hold the joints configuration at an optimization step */

    /*!
     * @brief update all the configuration dependent variables
     *
     * Update the robot configuration, the tranforms
     * the jacobian and all the variables depending on
     * the actual value of the optimization state
     * @param x the current optimization state
     * @return true if successfull, false otherwise
     */
    bool updateState(const Ipopt::Number * x);

    /*!
     * Specify which part of the Jacobian should be computed/updated
     */
    enum ComputeContraintJacobianOption {
        ComputeContraintJacobianOptionLinearPart = 1, /*!< Update the linear (position) part, i.e. first 3 lines */
        ComputeContraintJacobianOptionAngularPart = 1 << 1, /*!< Update the angular part, i.e. last 3/4 lines depending on the orientation parametrization */
    };

    /*!
     * @brief compute the IPOPT Jacobian given an iDynTree Jacobian
     *
     * Jacobians as outputted by iDynTree relates linear and angular velocities
     * with "internal" velocity (the velocity of the robot).
     * IPOPT is interested in the Jacobian as the derivative of the constraints
     * w.r.t. a variation of the optimization variable (not the time derivative).
     * This function is responsible of adapting the iDynTree Jacobian
     * to obtain a Jacobian usable by IPOPT
     *
     * @param[in] transformJacobian the Jacobian of the frame Transform
     * @param[in] quaternionDerivativeMapBuffer map for the quaternion derivative
     * @param[in] quaternionDerivativeInverseMapBuffer inverse map for the quaternion derivative
     * @param[in] computationOption bitwise mask of ComputeContraintJacobianOption
     * @param[out] constraintJacobianBuffer resulting IPOPT compatible Jacobian
     */
    void computeConstraintJacobian(const iDynTree::MatrixDynSize& transformJacobian,
                                   const iDynTree::MatrixFixSize<4, 3>& quaternionDerivativeMapBuffer,
                                   const iDynTree::MatrixFixSize<3, 4>& quaternionDerivativeInverseMapBuffer,
                                   const int computationOption,
                                   iDynTree::MatrixDynSize& constraintJacobianBuffer);

    void computeConstraintJacobianRPY(const iDynTree::MatrixDynSize& transformJacobian,
                                      const iDynTree::MatrixFixSize<3, 3>& rpyDerivativeMapBuffer,
                                      const iDynTree::MatrixFixSize<3, 3>& rpyDerivativeInverseMapBuffer,
                                      const int computationOption,
                                      iDynTree::MatrixDynSize& constraintJacobianBuffer);

    void computeConstraintJacobianCOMRPY(const iDynTree::MatrixDynSize& comJacobianBuffer,
                                         const iDynTree::MatrixFixSize<3, 3>& _rpyDerivativeInverseMap,
                                               iDynTree::MatrixDynSize& constraintComJacobianBuffer);

    /*!
     * @brief Map between RPY angles and angular velocity in the inertial frame
     *
     * The angular velocity \f$ \omega \f$ is related with the derivative
     * of the RPY angles by a 3x3 matrix, i.e.
     * \f[
     * {}^I \omega = M(w) \frac{d w}{dt},
     * \f]
     * where \f$ w \in \mathbb{R}^3 \f$ are the roll, pitch and yaw angles.
     * @param[in] rpyAngles roll, pitch, yaw angles
     * @param[out] map resulting 3x3 matrix
     */
    void omegaToRPYParameters(const iDynTree::Vector3& rpyAngles,
                              iDynTree::Matrix3x3& map);

public:
    /*! Constructor
     * @param data reference to the InverseKinematicsData object
     */
    InverseKinematicsNLP(InverseKinematicsData& data);

    virtual ~InverseKinematicsNLP();

    /*!
     * @brief Initialize buffers given the specified problem size
     *
     * @param n size of the optimization variable
     * @param m size of the constraints
     */
    void initializeInternalData();

#pragma mark - IpOpt methods

    virtual bool get_nlp_info(Ipopt::Index& n,
                              Ipopt::Index& m,
                              Ipopt::Index& nnz_jac_g,
                              Ipopt::Index& nnz_h_lag,
                              IndexStyleEnum& index_style);

    virtual bool get_bounds_info(Ipopt::Index n, Ipopt::Number* x_l, Ipopt::Number* x_u,
                                 Ipopt::Index m, Ipopt::Number* g_l, Ipopt::Number* g_u);

    virtual bool get_starting_point(Ipopt::Index n,
                                    bool init_x, Ipopt::Number* x,
                                    bool init_z, Ipopt::Number* z_L, Ipopt::Number* z_U,
                                    Ipopt::Index m, bool init_lambda, Ipopt::Number* lambda);

    virtual bool eval_f(Ipopt::Index n, const Ipopt::Number* x,
                        bool new_x, Ipopt::Number& obj_value);

    virtual bool eval_grad_f(Ipopt::Index n, const Ipopt::Number* x, bool new_x,
                             Ipopt::Number* grad_f);

    virtual bool eval_g(Ipopt::Index n, const Ipopt::Number* x,
                        bool new_x, Ipopt::Index m, Ipopt::Number* g);

    virtual bool eval_jac_g(Ipopt::Index n, const Ipopt::Number* x, bool new_x,
                            Ipopt::Index m, Ipopt::Index nele_jac, Ipopt::Index* iRow,
                            Ipopt::Index *jCol, Ipopt::Number* values);

    virtual bool eval_h(Ipopt::Index n, const Ipopt::Number* x, bool new_x,
                        Ipopt::Number obj_factor, Ipopt::Index m, const Ipopt::Number* lambda,
                        bool new_lambda, Ipopt::Index nele_hess, Ipopt::Index* iRow,
                        Ipopt::Index* jCol, Ipopt::Number* values);

    virtual void finalize_solution(Ipopt::SolverReturn status, Ipopt::Index n,
                                   const Ipopt::Number* x, const Ipopt::Number* z_L,
                                   const Ipopt::Number* z_U, Ipopt::Index m, const Ipopt::Number* g,
                                   const Ipopt::Number* lambda, Ipopt::Number obj_value,
                                   const Ipopt::IpoptData* ip_data,
                                   Ipopt::IpoptCalculatedQuantities* ip_cq);

    virtual Ipopt::Index get_number_of_nonlinear_variables();

    virtual bool get_list_of_nonlinear_variables(Ipopt::Index num_nonlin_vars,
                                                 Ipopt::Index* pos_nonlin_vars);

    virtual bool intermediate_callback(Ipopt::AlgorithmMode mode,
                                                             Ipopt::Index iter, Ipopt::Number obj_value,
                                                             Ipopt::Number inf_pr, Ipopt::Number inf_du,
                                                             Ipopt::Number mu, Ipopt::Number d_norm,
                                                             Ipopt::Number regularization_size,
                                                             Ipopt::Number alpha_du, Ipopt::Number alpha_pr,
                                                             Ipopt::Index ls_trials,
                                                             const Ipopt::IpoptData* ip_data,
                                                             Ipopt::IpoptCalculatedQuantities* ip_cq);

    void testDerivatives(const iDynTree::VectorDynSize& derivativePoint, int frameIndex, double epsilon, double tolerance, int parametrization);

};

#endif /* end of include guard: IDYNTREE_INTERNAL_INVERSEKINEMATICSNLP_H */
