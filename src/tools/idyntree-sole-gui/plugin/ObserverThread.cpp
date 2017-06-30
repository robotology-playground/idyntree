// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Author: Marco Randazzo, Marco Maggiali, Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include "include/ObserverThread.h"

#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>

#include <iDynTree/Core/VectorDynSize.h>
#include <iDynTree/ConvexHullHelpers.h>
#include <iDynTree/Model/Model.h>
#include <iDynTree/ModelIO/ModelLoader.h>


#include <QPainter>


using namespace yarp::os;

void addVectorOfStringToProperty(yarp::os::Property& prop, std::string key, std::vector<std::string> & list)
{
    prop.addGroup(key);
    yarp::os::Bottle & bot = prop.findGroup(key).addList();
    for (size_t i=0; i < list.size(); i++)
    {
        bot.addString(list[i].c_str());
    }
    return;
}

void addBottleToProperty(yarp::os::Property& prop, std::string key, yarp::os::Bottle * list)
{
    prop.addGroup(key);
    yarp::os::Bottle & bot = prop.findGroup(key).addList();
    for (size_t i=0; i < list->size(); i++)
    {
        bot.addString(list->get(i).asString().c_str());
    }
    return;
}

bool getVectorOfStringFromProperty(yarp::os::Property& prop, std::string key, std::vector<std::string> & list)
{
    yarp::os::Bottle *propAxesNames=prop.find(key).asList();
    if (propAxesNames==0)
    {
       yError() <<"Error parsing parameters: \"" << key << "\" should be followed by a list\n";
       return false;
    }

    list.resize(propAxesNames->size());
    for (int ax=0; ax < propAxesNames->size(); ax++)
    {
        list[ax] = propAxesNames->get(ax).asString().c_str();
    }
    return true;
}

ObserverThread::ObserverThread(yarp::os::ResourceFinder& config, int period) : RateThread(period), mutex(1)
{
    yDebug("ObserverThread running at %g ms.", getRate());

    bool ok = this->init(config);
    assert(ok);
}

bool ObserverThread::init(yarp::os::ResourceFinder& config)
{
    yarp::os::Property propRemapper;
    propRemapper.put("device", "remotecontrolboardremapper");

    yarp::os::Property propConfig;
    propConfig.fromString(config.toString().c_str());

    std::vector<std::string> usedDOFs;
    getVectorOfStringFromProperty(propConfig, "axesNames", usedDOFs);
    addVectorOfStringToProperty(propRemapper, "axesNames", usedDOFs);

    m_measuredEncodersInDeg.resize(usedDOFs.size());
    m_desiredJointPositionsInDeg.resize(usedDOFs.size());

    yarp::os::Bottle *propRemoteControlBoards=propConfig.find("remoteControlBoards").asList();
    if (propRemoteControlBoards==0)
    {
       yError() <<"Error parsing parameters: \"propRemoteControlBoards\" should be followed by a list\n";
       return false;
    }

    addBottleToProperty(propRemapper, "remoteControlBoards", propRemoteControlBoards);
    std::string model=config.check("model", Value("model.urdf")).asString();
    m_primarySole=config.check("primary-sole", Value("l_sole")).asString();

    std::vector<std::string> soleFrames;
    getVectorOfStringFromProperty(propConfig, "sole-frames", soleFrames);

    if (soleFrames.size() != 2)
    {
        std::cerr << "idyntree-sole-gui currently supports only 2 sole frames." << std::endl;
        return false;
    }

    if (std::find(soleFrames.begin(), soleFrames.end(), m_primarySole) == soleFrames.end())
    {
        std::cerr << "idyntree-sole-gui: the primary sole should be one of the two sole frames." << std::endl;
        return false;
    }

    if (soleFrames[0] != "l_sole")
    {
        m_secondarySole = soleFrames[0];
    }

    if (soleFrames[1] != "l_sole")
    {
        m_secondarySole = soleFrames[1];
    }

    propRemapper.put("localPortPrefix", "/soleGui");

    m_iencs = 0;
    m_iposdir = 0;

    m_wholeBodyDevice.open(propRemapper);
    m_wholeBodyDevice.view(m_iencs);
    m_wholeBodyDevice.view(m_iposdir);

    iDynTree::ModelLoader mdlLoader;
    bool ok = mdlLoader.loadReducedModelFromFile(config.findFileByName(model), usedDOFs);

    if (!ok)
    {
        yError() <<"Error loading reduced model\n";
        return false;
    }

    m_kinDyn.loadRobotModel(mdlLoader.model());


    int width =config.find("width" ).asInt();
    int height=config.find("height").asInt();


    int max_tax=0;

    //resize(width,height);

    // Open the pressuresensorsarray group
    Bottle pressuresensorsarrayBot = config.findGroup("pressuresensorsarray");
    m_skip_pressure = pressuresensorsarrayBot.isNull();

    if (!m_skip_pressure)
    {
        Property pressuresensorsarrayProp;
        pressuresensorsarrayProp.fromString(pressuresensorsarrayBot.toString());

        Property pressuresensorsarrayPropPrimarySole;
        pressuresensorsarrayPropPrimarySole.fromString(pressuresensorsarrayProp.findGroup(m_primarySole).toString());

        Property pressuresensorsarrayPropSecondarySole;
        pressuresensorsarrayPropSecondarySole.fromString(pressuresensorsarrayProp.findGroup(m_secondarySole).toString());

        yarp::os::Property propPrimaryAnalog;
        propPrimaryAnalog.put("device", "analogsensorclient");
        propPrimaryAnalog.put("local", "/soleGui/primarySolePressure");
        propPrimaryAnalog.put("remote", pressuresensorsarrayPropPrimarySole.find("remotePortName").asString());
        m_primarySolePressure.open(propPrimaryAnalog);

        yarp::os::Property propSecondaryAnalog;
        propSecondaryAnalog.put("device", "analogsensorclient");
        propSecondaryAnalog.put("local", "/soleGui/secondarySolePressure");
        propSecondaryAnalog.put("remote", pressuresensorsarrayPropSecondarySole.find("remotePortName").asString());
        m_secondarySolePressure.open(propSecondaryAnalog);

        m_primarySolePressure.view(m_primaryPressureAnalog);
        m_secondarySolePressure.view(m_secondaryPressureAnalog);

        std::vector<std::string> pressurePadsNamesPrimarySole;
        getVectorOfStringFromProperty(pressuresensorsarrayPropPrimarySole, "pressureFrameNames", pressurePadsNamesPrimarySole);
        m_measuredPressurePrimarySoleInN.resize(pressurePadsNamesPrimarySole.size());


        m_primaryPressurePadsInPrimarySole.resize(0);
        // Save the pressure pads position from URDF
        for (std::string & padName : pressurePadsNamesPrimarySole)
        {
            m_primaryPressurePadsInPrimarySole.push_back(m_kinDyn.getRelativeTransform(m_primarySole, padName).getPosition());
        }

        std::vector<std::string> pressurePadsNamesSecondarySole;
        getVectorOfStringFromProperty(pressuresensorsarrayPropSecondarySole, "pressureFrameNames", pressurePadsNamesSecondarySole);
        m_measuredPressureSecondarySoleInN.resize(pressurePadsNamesPrimarySole.size());

        m_secondaryPressurePadsInSecondarySole.resize(0);
        for (std::string & padName : pressurePadsNamesSecondarySole)
        {
            m_secondaryPressurePadsInSecondarySole.push_back(m_kinDyn.getRelativeTransform(m_secondarySole, padName).getPosition());
        }


    }
}

ObserverThread::~ObserverThread()
{
}

bool ObserverThread::threadInit()
{
    yDebug("ObserverThread initialising..");
    yDebug("..done!");

    yInfo("Waiting for port connection..");
    return true;
}

void ObserverThread::run()
{
    // Read encoders
    m_iencs->getEncoders(m_measuredEncodersInDeg.data());

    // Read desired positions
    if (m_iposdir)
    {
        m_iposdir->getRefPositions(m_desiredJointPositionsInDeg.data());
    }

    iDynTree::VectorDynSize m_measuredEncodersInRad(m_measuredEncodersInDeg.size());
    iDynTree::VectorDynSize m_desiredJointPositionsInRad(m_desiredJointPositionsInDeg.size());

    // Convert in rad for iDynTree use
    for (int i = 0; i < m_measuredEncodersInRad.size(); i++)
    {
        double deg2rad = (3.14159265359/180.0);
        m_measuredEncodersInRad(i) = deg2rad*m_measuredEncodersInDeg[i];
        m_desiredJointPositionsInRad(i) = deg2rad*m_desiredJointPositionsInDeg[i];
    }

    // Update the model
    m_kinDyn.setJointPos(m_measuredEncodersInRad);

    // Compute the com in the primarySole frame
    const iDynTree::Model& model = m_kinDyn.model();
    m_comInPrimarySole = m_kinDyn.getRelativeTransform(m_primarySole, model.getLinkName(model.getDefaultBaseLink()))*m_kinDyn.getCenterOfMassPosition();

    // Compute the cop in the primary sole
    if (m_primaryPressureAnalog)
    {
        m_primaryPressureAnalog->read(m_measuredPressurePrimarySoleInN);
    }

    m_primaryCopInPrimarySole.zero();
    double primaryTotalForce = 0;
    for (int i=0; i < m_primaryPressurePadsInPrimarySole.size(); i++)
    {
        m_primaryCopInPrimarySole(0) += m_primaryPressurePadsInPrimarySole[i](0)*m_measuredPressurePrimarySoleInN[i];
        m_primaryCopInPrimarySole(1) += m_primaryPressurePadsInPrimarySole[i](1)*m_measuredPressurePrimarySoleInN[i];
        primaryTotalForce += m_measuredPressurePrimarySoleInN[i];
    }

    if ( primaryTotalForce > 0.0 )
    {
        m_primaryCopInPrimarySole(0) = m_primaryCopInPrimarySole(0)/primaryTotalForce;
        m_primaryCopInPrimarySole(1) = m_primaryCopInPrimarySole(1)/primaryTotalForce;
    }
    else
    {
        m_primaryCopInPrimarySole.zero();
    }

    // Compute the cop in the primary sole
    if (m_secondaryPressureAnalog)
    {
        m_secondaryPressureAnalog->read(m_measuredPressureSecondarySoleInN);
    }

    m_primarySole_X_secondarySole = m_kinDyn.getRelativeTransform(m_primarySole, m_secondarySole);

    m_secondaryCopInPrimarySole.zero();
    double secondaryTotalForce = 0;
    for (int i=0; i < m_secondaryPressurePadsInSecondarySole.size(); i++)
    {
        iDynTree::Position secPressurePadInPrimarySole = m_primarySole_X_secondarySole*(m_secondaryPressurePadsInSecondarySole[i]);

        m_secondaryCopInPrimarySole(0) += secPressurePadInPrimarySole(0)*m_measuredPressureSecondarySoleInN[i];
        m_secondaryCopInPrimarySole(1) += secPressurePadInPrimarySole(1)*m_measuredPressureSecondarySoleInN[i];
        secondaryTotalForce += m_measuredPressureSecondarySoleInN[i];
    }


    if ( secondaryTotalForce > 0.0 )
    {
        m_secondaryCopInPrimarySole(0) = m_secondaryCopInPrimarySole(0)/secondaryTotalForce;
        m_secondaryCopInPrimarySole(1) = m_secondaryCopInPrimarySole(1)/secondaryTotalForce;
    }
    else
    {
        m_secondaryCopInPrimarySole.zero();
    }

    // Compute total cop
    m_totalCopInPrimarySole.zero();
    if (primaryTotalForce+secondaryTotalForce > 0.0 )
    {
        m_totalCopInPrimarySole(0) = (m_primaryCopInPrimarySole(0)*primaryTotalForce + m_secondaryCopInPrimarySole(0)*secondaryTotalForce)/(primaryTotalForce+secondaryTotalForce);
        m_totalCopInPrimarySole(1) = (m_primaryCopInPrimarySole(1)*primaryTotalForce + m_secondaryCopInPrimarySole(1)*secondaryTotalForce)/(primaryTotalForce+secondaryTotalForce);
    }


    // Compute the desired com in the primarySole frame
    m_kinDyn.setJointPos(m_desiredJointPositionsInRad);
    m_desiredComInPrimarySole = m_kinDyn.getRelativeTransform(m_primarySole, model.getLinkName(model.getDefaultBaseLink()))*m_kinDyn.getCenterOfMassPosition();


}

void ObserverThread::threadRelease()
{
    yDebug("ObserverThread releasing...");
    m_wholeBodyDevice.close();
    if (m_skip_pressure)
    {
        m_primarySolePressure.close();
        m_secondarySolePressure.close();
    }
    skin_port.close();
    yDebug("... done.");
}

QPointF toQt2D(iDynTree::Position& pos)
{
    return QPointF(pos(0), pos(1));
}

QPolygonF toQt2D(iDynTree::Polygon& poly)
{
    QPolygonF qpoly;

    for (unsigned int i =0; i< poly.getNrOfVertices(); i++)
    {
        qpoly << toQt2D(poly(i));
    }

    return qpoly;
}

void ObserverThread::drawPolygon(QPainter* qpainter, iDynTree::Polygon& poly)
{
    QPolygonF qpoly = toQt2D(poly);
    qpainter->drawPolygon(qpoly);
}

void ObserverThread::draw(QPainter* qpainter, int widthInPixels, int heightInPixels)
{
    // The painter draws directly in the primary sole xy plane (the gui need to appropriatly set the world transform)
    double maxXvizInM = 0.03;
    double maxYvizInM = 0.05;
    double minXvizInM = -0.03;
    double minYvizInM = -0.05;

    // Compute world transform
    QTransform worldTransform;

    worldTransform.translate(widthInPixels/2, heightInPixels/2);

    // Rotate 90 deg counterclockwise
    worldTransform.rotate(90.0);

    // Computing scaling factor
    double scalingX = heightInPixels/(maxXvizInM-minXvizInM);
    double scalingY = widthInPixels/(maxYvizInM-minYvizInM);

    // Do not deform the view area
    double scaling = std::min(scalingX, scalingY);

    worldTransform.scale(scaling, scaling);
    qpainter->setWorldMatrixEnabled(true);
    qpainter->setWorldTransform(worldTransform);

    QPen pen = qpainter->pen();
    pen.setColor(QColor("black"));
    // The pen as a width of 1 millimeter
    pen.setWidthF(0.001);
    qpainter->setPen(pen);

    // Draw sole area
    pen = qpainter->pen();
    pen.setColor(QColor("green"));
    qpainter->setPen(pen);

    double safetyMarginX=0.01;
    double safetyMarginY=0.005;
    iDynTree::Polygon primarySolePolygonInPrimarySole = iDynTree::Polygon::XYRectangleFromOffsets(0.025, 0.015, 0.01, 0.01);
    iDynTree::Polygon secondarySolePolygonInSecondarySole = iDynTree::Polygon::XYRectangleFromOffsets(0.025, 0.015, 0.01, 0.01);
    iDynTree::Polygon primarySolePolygonWithSafetyInPrimarySole  = iDynTree::Polygon::XYRectangleFromOffsets(0.025-safetyMarginX, 0.015-safetyMarginX, 0.01-safetyMarginY, 0.01-safetyMarginY);
    iDynTree::Polygon secondarySolePolygonWithSafetyInSecondarySole = iDynTree::Polygon::XYRectangleFromOffsets(0.025-safetyMarginX, 0.015-safetyMarginX, 0.01-safetyMarginY, 0.01-safetyMarginY);

    iDynTree::Polygon secondarySolePolygonInPrimarySole = secondarySolePolygonInSecondarySole.applyTransform(m_primarySole_X_secondarySole);
    iDynTree::Polygon secondarySolePolygonWithSafetyInPrimarySole = secondarySolePolygonWithSafetyInSecondarySole.applyTransform(m_primarySole_X_secondarySole);

    // Draw soles
    this->drawPolygon(qpainter, primarySolePolygonInPrimarySole);
    this->drawPolygon(qpainter, secondarySolePolygonInPrimarySole);

    pen = qpainter->pen();
    pen.setColor(QColor("lime"));
    qpainter->setPen(pen);
    this->drawPolygon(qpainter, primarySolePolygonWithSafetyInPrimarySole);
    this->drawPolygon(qpainter, secondarySolePolygonWithSafetyInPrimarySole);

    // Draw pressure pads
    double padsSize = 0.0003;
    pen = qpainter->pen();
    pen.setColor(QColor("gray"));
    pen.setWidthF(0.001);
    qpainter->setPen(pen);

    for (int i=0; i < m_primaryPressurePadsInPrimarySole.size(); i++)
    {
        qpainter->drawEllipse(QPointF(m_primaryPressurePadsInPrimarySole[i](0), m_primaryPressurePadsInPrimarySole[i](1)), padsSize, padsSize);
    }

     for (int i=0; i < m_secondaryPressurePadsInSecondarySole.size(); i++)
    {
        iDynTree::Position secPressurePadInPrimarySole = m_primarySole_X_secondarySole*(m_secondaryPressurePadsInSecondarySole[i]);
        qpainter->drawEllipse(QPointF(secPressurePadInPrimarySole(0), secPressurePadInPrimarySole(1)), padsSize, padsSize);
    }

    // Draw desired COM (In Red)
    double pointRadiusInM = 0.0006;
    pen = qpainter->pen();
    pen.setColor(QColor("red"));
    pen.setWidthF(0.002);
    qpainter->setPen(pen);
    qpainter->drawEllipse(QPointF(m_desiredComInPrimarySole(0), m_desiredComInPrimarySole(1)), pointRadiusInM, pointRadiusInM);

    // Draw measured com (in green)
    pen = qpainter->pen();
    pen.setColor(QColor("green"));
    pen.setWidthF(0.002);
    qpainter->setPen(pen);
    qpainter->drawEllipse(QPointF(m_comInPrimarySole(0), m_comInPrimarySole(1)), pointRadiusInM, pointRadiusInM);

    // Draw Center of pressure (in blue)
    pen = qpainter->pen();
    pen.setColor(QColor("aquamarine"));
    pen.setWidthF(0.002);
    qpainter->setPen(pen);
    qpainter->drawEllipse(QPointF(m_primaryCopInPrimarySole(0), m_primaryCopInPrimarySole(1)), pointRadiusInM, pointRadiusInM);
    qpainter->drawEllipse(QPointF(m_secondaryCopInPrimarySole(0), m_secondaryCopInPrimarySole(1)), pointRadiusInM, pointRadiusInM);

    pen = qpainter->pen();
    pen.setColor(QColor("blue"));
    pen.setWidthF(0.002);
    qpainter->setPen(pen);
    qpainter->drawEllipse(QPointF(m_totalCopInPrimarySole(0), m_totalCopInPrimarySole(1)), pointRadiusInM, pointRadiusInM);

    //qpainter->drawText(QPointF(0.0, 0.0),QString("COM"));
}
