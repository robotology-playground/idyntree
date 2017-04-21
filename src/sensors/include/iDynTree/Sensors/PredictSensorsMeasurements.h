/*
 * Copyright (C) 2015 Fondazione Istituto Italiano di Tecnologia
 * Author: Naveen Kuppuswamy
 * email:  naveen.kuppuswamy@iit.it
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

#ifndef PREDICTSENSORSMEASUREMENTS_HPP
#define PREDICTSENSORSMEASUREMENTS_HPP

#include <iDynTree/Model/LinkState.h>

namespace iDynTree
{

    class SensorsList;
    class Traversal;
    class Model;
    class VectorDynSize;
    class LinearMotionVector3;
    typedef LinearMotionVector3 LinAcceleration;
    class SensorsMeasurements;
    class FreeFloatingAcc;
    class FreeFloatingPos;
    class FreeFloatingVel;
    class SensorsMeasurement;
    class LinkPositions;
    class LinkVelArray;
    class LinkAccArray;
    class FreeFloatingGeneralizedTorques;


    /**
     * \brief Predict the measurement of a set of sensors.
     *
     * Given a SensorList object describing a list of sensor of a
     * model fill the output argument predictedMeasurement
     * with the predicted measurement of the sensors consistent with the state
     * and the acceleration/torques and forces of the Model.
     *
     *
     * This function takes in input the internal buffers used for the
     * computation to avoid dynamic memory allocation.
     *
     * \ingroup iDynTreeSensors
     *
     * @param[in] model the model used to predict the sensor measurements.
     * @param[in] sensorList the sensors list used to predict the sensors measurements.
     * @param[in] traversal the Traversal used for predict the sensor measurements.
     * @param[in] robotPos the position of the model used for prediction.
     * @param[in] robotVel the velocity of the model used for prediction.
     * @param[in] robotAcc the acceleration of the model used for prediction.
     * @param[in] gravity the gravity acceleration (in world frame) used for prediction.
     * @param[in] externalWrenches the net external wrench acting on each link.
     * @param[out] buf_properRobotAcc internal buffer, storing the proper acceleration of the model.
     * @param[out] buf_linkPos internal buffer, storing the position of every link in the model.
     * @param[out] buf_linkVel internal buffer, storing the velocity of every link in the model.
     * @param[out] buf_linkProperAcc internal buffer, storing the proper acceleration of every link in the model.
     * @param[out] buf_internalWrenches internal buffer, storing the internal wrenches.
     * @param[out] buf_outputTorques internal buffer, storing the generalized joint torques.
     * @param[out] predictedMeasurement the predicted measurements for the sensors.
     *
     * @return true if the sensors in the list are all valid
     */
     bool predictSensorsMeasurements(const Model & model,
                                     const SensorsList &sensorList,
                                     const Traversal & traversal,
                                     const FreeFloatingPos& robotPos,
                                     const FreeFloatingVel& robotVel,
                                     const FreeFloatingAcc& robotAcc,
                                     const LinAcceleration & gravity,
                                     const LinkNetExternalWrenches & externalWrenches,
                                           FreeFloatingAcc& buf_properRobotAcc,
                                           LinkPositions& buf_linkPos,
                                           LinkVelArray& buf_linkVel,
                                           LinkAccArray& buf_linkProperAcc,
                                           LinkInternalWrenches& buf_internalWrenches,
                                           FreeFloatingGeneralizedTorques& buf_outputTorques,
                                           SensorsMeasurements &predictedMeasurement);

    /**
     * \brief Predict the measurement of a set of sensors.
     *
     * Helper function that works on buffers already compute with Dynamics functions
     *
     * \ingroup iDynTreeSensors
     *
     * @param[in] model the model used to predict the sensor measurements.
     * @param[in] sensorList the sensors list used to predict the sensors measurements.
     * @param[in] traversal the Traversal used for predict the sensor measurements.
     * @param[in] buf_linkVel internal buffer, storing the velocity of every link in the model.
     * @param[in] buf_linkProperAcc internal buffer, storing the proper acceleration of every link in the model.
     * @param[in] buf_internalWrenches internal buffer, storing the internal wrenches.
     * @param[out] predictedMeasurement the predicted measurements for the sensors.
     *
     * @return true if the sensors in the list are all valid
     */
     bool predictSensorsMeasurementsFromRawBuffers(const Model & model,
                                                   const SensorsList &sensorList,
                                                   const Traversal & traversal,
                                                   const LinkVelArray& buf_linkVel,
                                                   const LinkAccArray& buf_linkProperAcc,
                                                   const LinkInternalWrenches& buf_internalWrenches,
                                                         SensorsMeasurements &predictedMeasurement);

}

#endif
