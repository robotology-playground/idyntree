// SPDX-FileCopyrightText: Fondazione Istituto Italiano di Tecnologia (IIT)
// SPDX-License-Identifier: BSD-3-Clause


#include <iDynTree/Model/Jacobians.h>

#include <iDynTree/Core/EigenHelpers.h>

#include <iDynTree/Model/Model.h>
#include <iDynTree/Model/Traversal.h>
#include <iDynTree/Model/LinkState.h>

namespace iDynTree
{

bool FreeFloatingJacobianUsingLinkPos(const Model& model,
                                      const Traversal& traversal,
                                      const JointPosDoubleArray& jointPositions,
                                      const LinkPositions& world_H_links,
                                      const LinkIndex jacobianLinkIndex,
                                      const Transform& jacobFrame_X_world,
                                      const Transform& baseFrame_X_jacobBaseFrame,
                                      const MatrixView<double>& jacobian)
{
    // We zero the jacobian
    toEigen(jacobian).setZero();

    // Compute base part
    const Transform & world_H_base = world_H_links(traversal.getBaseLink()->getIndex());
    toEigen(jacobian).block(0,0,6,6) = toEigen((jacobFrame_X_world*world_H_base*baseFrame_X_jacobBaseFrame).asAdjointTransform());

    // Compute joint part
    // We iterate from the link up in the traveral until we reach the base
    LinkIndex visitedLinkIdx = jacobianLinkIndex;

    while (visitedLinkIdx != traversal.getBaseLink()->getIndex())
    {
        LinkIndex parentLinkIdx = traversal.getParentLinkFromLinkIndex(visitedLinkIdx)->getIndex();
        IJointConstPtr joint = traversal.getParentJointFromLinkIndex(visitedLinkIdx);

        size_t dofOffset = joint->getDOFsOffset();
        for(int i=0; i < joint->getNrOfDOFs(); i++)
        {
            toEigen(jacobian).block(0,6+dofOffset+i,6,1) =
                toEigen(jacobFrame_X_world*(world_H_links(visitedLinkIdx)*joint->getMotionSubspaceVector(i,visitedLinkIdx,parentLinkIdx)));
        }

        visitedLinkIdx = parentLinkIdx;
    }

    return true;
}


}
