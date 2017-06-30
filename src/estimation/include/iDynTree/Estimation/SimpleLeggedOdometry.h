/*
 * Copyright (C) 2015 Robotics, Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Silvio Traversaro
 * email: silvio.traversaro@iit.it
 *
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#ifndef IDYNTREE_SIMPLE_LEGGED_ODOMETRY2_
#define IDYNTREE_SIMPLE_LEGGED_ODOMETRY2_

#include <iDynTree/Core/Transform.h>
#include <iDynTree/Model/Indices.h>
#include <iDynTree/Model/Model.h>
#include <iDynTree/Model/Traversal.h>
#include <iDynTree/Model/JointState.h>
#include <iDynTree/Model/LinkState.h>

namespace iDynTree
{

/**
 * \ingroup iDynTreeEstimation
 *
 * A simple legged odometry for a legged robot.
 *
 * Under the assumption that at least a link of the robot at the time is
 * not moving (no slippage), it computes the estimate of the transform
 * between a inertial/world frame and the robot floating base.
 *
 * The algorithm implemented is the following :
 *
 * -# During initialization  the user of the class specifies (through init()):
 *       * a frame that is rigidly attached to a link that is not moving (`initialFixedFrame`)
 *       * an optional transform between the desired location of the world and the fixed frame (`world_H_initialFixedFrame`)
 *    At the beginning, the `world_H_fixed` is the one specified (default is the identity)
 *
 * -# At this point, the `getWorldFrameTransform(iDynTree::FrameIndex frame_id)` will return the `world_H_frame`
 *      ( \f${}^{world} H_{frame}\f$ ) transform simply by computing the forward kinematics from the fixed frame
 *     to the frame specified by `frame_id` : `world_H_frame = world_H_fixed * fixed_H_frame(qj)`, i.e.
 *                                          \f${}^{world} H_{frame} = {}^{world} H_{fixed} \cdot {}^{fixed} H_{frame}(q_j)\f$
 *
 * -# If the fixed frame changes, we can simply change the frame used as "fixed" (changeFixedLink()), and consistently update the
 *      `world_H_fixed` transform to be equal to `world_H_new_fixed =  world_H_old_fixed * old_fixed_H_new_fixed(qj)`, i.e.
 *      \f${}^{world} H_{new\_fixed} = {}^{world} H_{old\_fixed} \cdot {}^{old\_fixed} H_{new\_fixed}(q_j)\f$
 *
 * -# After the update, the `getWorldFrameTransform(iDynTree::FrameIndex frame_id)` can be obtained as in point 1b .
 *
 * To reset the location of the world, init can simply be called again.
 *
 */
class SimpleLeggedOdometry
{
    /**
     * Model used in odometry.
     */
    Model m_model;

    /**
     * Traversal used for the dynamics computations
     */
    Traversal m_traversal;

    /**
     * False initially, true after a valid model has been loaded.
     */
    bool m_isModelValid;

      /**
     * False initially, true after updateKinematics was successfully called.
     */
    bool m_kinematicsUpdated;

    /**
     * False initially, true after init was successfully called.
     */
    bool m_isOdometryInitialized;

    /**
     * The link index of the link considered fixed.
     */
    LinkIndex m_fixedLinkIndex;

    /**
     * Position of each link with respect to the base
     * link of the traversal (i.e. output
     * of the relative forward kinematics).
     */
    LinkPositions m_base_H_link;

    /**
     * Transform between the link currently considered fixed
     * and the world/inertial frame.
     * This is initialized in the init method and update
     * by the changeFixedLink method.
     */
    Transform m_world_H_fixedLink;

public:
    /**
     * Constructor
     */
    SimpleLeggedOdometry();

    /**
     * Destructor
     */
    ~SimpleLeggedOdometry();

    /**
     * \brief Set model used for the odometry.
     * The model is copied inside the class, to be used for the odometry estimation.
     *
     * @param[in] _model the kinematic and dynamic model used for the estimation.
     * @return true if all went well (model is well formed), false otherwise.
     */
    bool setModel(const Model & _model);

    /**
     * Load model from file.
     *
     * @param[in] filename path to the file to load.
     * @param[in] filetype (optional) explicit definiton of the filetype to load.
     *                     Only "urdf" is supported at the moment.
     * @return true if all went well (files were correctly loaded and consistent), false otherwise.
     *
     */
    bool loadModelFromFile(const std::string filename, const std::string filetype="");

    /**
     * Load model from file, specifieng the dof considered for the estimation.
     *
     * @param[in] filename path to the file to load.
     * @param[in] consideredDOFs list of dof to consider in the model.
     * @param[in] filetype (optional) explicit definiton of the filetype to load.
     *                     Only "urdf" is supported at the moment.
     * @return true if all went well (files were correctly loaded and consistent), false otherwise.
     *
     *
     * \note this will create e a reduced model only with the joint specified in consideredDOFs
     */
    bool loadModelFromFileWithSpecifiedDOFs(const std::string filename,
                                            const std::vector<std::string> & consideredDOFs,
                                            const std::string filetype="");

    /**
     * Get used model.
     *
     * @return the kinematic and dynamic model used for estimation.
     */
    const Model & model() const;

    /**
     * Set the measured joint positions.
     *
     * This is used to update the joints positions used by the odometry.
     *
     */
    bool updateKinematics(JointPosDoubleArray & jointPos);

    /**
     * Initialize the odometry.
     * This method initializes the world location w.r.t. to a frame
     * that is not the frame that is initially assumed fixed.
     * For this reason it is necessary to call updateKinematics at least once before calling this method.
     *
     * @param[in] initialFixedFrame Frame initially assumed to be fixed.
     * @param[in] initialFixedFrame_H_world Pose of the world w.r.t. the initial fixed frame (default: identity, i.e. the initialFixedFrame is the world).
     * @return true if all went well, false otherwise.
     */
    bool init(const std::string & initialFixedFrame,
              const Transform initialFixedFrame_H_world = Transform::Identity());

     /**
     * Initialize the odometry.
     * This method initializes the world location w.r.t. to a frame
     * that is not the frame that is initially assumed fixed.
     * For this reason it is necessary to call updateKinematics at least once before calling this method.
     *
     * @param[in] initialFixedFrameIndex Frame initially assumed to be fixed.
     * @param[in] initialFixedFrame_H_world Pose of the world w.r.t. the initial fixed frame (default: identity, i.e. the initialFixedFrame is the world).
     * @return true if all went well, false otherwise.
     */
    bool init(const FrameIndex initialFixedFrameIndex,
              const Transform initialFixedFrame_H_world = Transform::Identity());

    /**
     * Initialize the odometry, specifying separately initial fixed frame and world.
     * This method initializes the world location w.r.t. to a frame
     * that is not the frame that is initially assumed fixed,
     * for this reason it is necessary to call updateKinematics at least once before calling this method.
     *
     * @param[in] initialFixedFrame Frame initially assumed to be fixed.
     * @param[in] initialReferenceFrameForWorld Frame in which the initial world is expressed.
     * @param[in] initialReferenceFrame_H_world Pose of the world w.r.t. the initial reference frame (default: identity, i.e. the initialReferenceFrameForWorld is the world).
     * @return true if all went well, false otherwise.
     */
    bool init(const std::string & initialFixedFrame,
              const std::string & initialReferenceFrameForWorld,
              const Transform initialReferenceFrame_H_world = Transform::Identity());

    /**
     * Initialize the odometry, specifying separately initial fixed frame and world.
     * This method initializes the world location w.r.t. to a frame
     * that is not the frame that is initially assumed fixed,
     * for this reason it is necessary to call updateKinematics at least once before calling this method.
     *
     * @param[in] initialFixedFrameIndex Frame initially assumed to be fixed.
     * @param[in] initialReferenceFrameIndexForWorld Frame in which the initial world is expressed.
     * @param[in] initialReferenceFrame_H_world Pose of the world w.r.t. the initial reference frame (default: identity, i.e. the initialReferenceFrameForWorld is the world).
     * @return true if all went well, false otherwise.
     */
    bool init(const FrameIndex initialFixedFrameIndex,
              const FrameIndex initialReferenceFrameIndexForWorld,
              const Transform initialReferenceFrame_H_world = Transform::Identity());

    /**
     * Change the link that the odometry assumes to be fixed with
     * respect to the inertial/world frame.
     */
    bool changeFixedFrame(const std::string & newFixedFrame);

    /**
     * Change the link that the odometry assumes to be fixed
     * with respect to the inertial/world frame.
     */
    bool changeFixedFrame(const FrameIndex newFixedFrame);

    /**
     * Get the link currently considered fixed with respect to the inertial frame.
     *
     * \note This can be diffent from what was set with changeFixedFrame, because
     *       multiple frames can belong to the same link.
     *
     * @return the name of the link currently considered fixed.
     */
    std::string getCurrentFixedLink();

    /**
     * Get the world_H_link transform for an arbitrary link.
     *
     * \note this method works only for link, not for arbitrary frames.
     */
    iDynTree::Transform getWorldLinkTransform(const LinkIndex frame_index);
};


} // End namespace iDynTree

#endif // IDYNTREE_SIMPLE_LEGGED_ODOMETRY2_


