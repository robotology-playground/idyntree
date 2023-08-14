// SPDX-FileCopyrightText: Fondazione Istituto Italiano di Tecnologia (IIT)
// SPDX-License-Identifier: BSD-3-Clause

#include "FrameVisualization.h"
#include "IrrlichtUtils.h"


void iDynTree::FrameVisualization::setFrameTransform(size_t index, const iDynTree::Transform &transformation)
{
    irr::scene::ISceneNode * frameSceneNode = m_frames[index].visualizationNode;
    frameSceneNode->setPosition(idyntree2irr_pos(transformation.getPosition()));
    frameSceneNode->setRotation(idyntree2irr_rot(transformation.getRotation()));
}

iDynTree::FrameVisualization::FrameVisualization()
    : m_smgr(0)
{

}

size_t iDynTree::FrameVisualization::addFrame(const iDynTree::Transform &transformation, double arrowLength)
{
    m_frames.emplace_back();

    m_frames.back().visualizationNode = addFrameAxes(m_smgr, 0, arrowLength);
    setFrameTransform(m_frames.size() - 1, transformation);
    m_frames.back().label.init(m_smgr, m_frames.back().visualizationNode);

    return m_frames.size() - 1;
}

bool iDynTree::FrameVisualization::setVisible(size_t frameIndex, bool isVisible)
{
    if (frameIndex >= m_frames.size()) {
        reportError("FrameVisualization","setVisible","frameIndex out of bounds.");
        return false;
    }

    m_frames[frameIndex].visualizationNode->setVisible(isVisible);

    return true;
}

size_t iDynTree::FrameVisualization::getNrOfFrames() const
{
    return m_frames.size();
}

bool iDynTree::FrameVisualization::getFrameTransform(size_t frameIndex, iDynTree::Transform &currentTransform) const
{
    if (frameIndex >= m_frames.size()) {
        reportError("FrameVisualization","getFrameTransform","frameIndex out of bounds.");
        return false;
    }

    irr::scene::ISceneNode * frameSceneNode = m_frames[frameIndex].visualizationNode;
    currentTransform.setPosition(irr2idyntree_pos(frameSceneNode->getAbsolutePosition()));
    currentTransform.setRotation(irr2idyntree_rot(frameSceneNode->getRotation()));

    return true;
}

bool iDynTree::FrameVisualization::updateFrame(size_t frameIndex, const iDynTree::Transform &transformation)
{
    if (frameIndex >= m_frames.size()) {
        reportError("FrameVisualization","updateFrame","frameIndex out of bounds.");
        return false;
    }
    setFrameTransform(frameIndex, transformation);
    return true;
}

iDynTree::ILabel *iDynTree::FrameVisualization::getFrameLabel(size_t frameIndex)
{
    if (frameIndex >= m_frames.size()) {
        reportError("FrameVisualization","getFrameLabel","frameIndex out of bounds.");
        return nullptr;
    }

    return &m_frames[frameIndex].label;
}

iDynTree::FrameVisualization::~FrameVisualization()
{
    close();
}

void iDynTree::FrameVisualization::init(irr::scene::ISceneManager *smgr)
{
    assert(smgr);
    m_smgr = smgr;
    m_smgr->grab(); //Increment the reference count
}

void iDynTree::FrameVisualization::close()
{
    for (auto& frames: m_frames) {
        if (frames.visualizationNode) {
            frames.visualizationNode->removeAll();
            frames.visualizationNode = nullptr;
        }
    }
    m_frames.resize(0);

    if (m_smgr)
    {
        m_smgr->drop(); //Drop the element (dual of "grab")
        m_smgr = 0;
    }
}
