/*
 * Copyright (C) 2014,2017 Fondazione Istituto Italiano di Tecnologia
 * Authors: Francesco Romano
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 * Originally developed for Prioritized Optimal Control (2014)
 * Refactored in 2017.
 * Design inspired by
 * - ACADO toolbox (http://acado.github.io)
 * - ADRL Control Toolbox (https://adrlab.bitbucket.io/ct/ct_doc/doc/html/index.html)
 */

#include "iDynTree/OptimalControlProblem.h"
#include "iDynTree/DynamicalSystem.h"
#include "iDynTree/ConstraintsGroup.h"
#include "iDynTree/Constraint.h"
#include "iDynTree/Cost.h"
#include "iDynTree/TimeRange.h"
#include "iDynTree/Core/VectorDynSize.h"
#include "iDynTree/Core/Utils.h"
#include <map>
#include <cassert>
#include <sstream>

namespace iDynTree {
    namespace optimalcontrol {

        typedef std::map< std::string, std::shared_ptr<ConstraintsGroup>> ConstraintsGroupsMap;

        typedef struct{
            std::shared_ptr<Cost> cost;
            double weight;
            TimeRange timeRange;
        } TimedCost;

        typedef std::map< std::string, TimedCost> CostsMap;

        class OptimalControlProblem::OptimalControlProblemPimpl
        {
        public:
            TimeRange horizon;
            std::shared_ptr<DynamicalSystem> dynamicalSystem;
            ConstraintsGroupsMap constraintsGroups;
            CostsMap costs;
            VectorDynSize stateLowerBound, stateUpperBound, controlLowerBound, controlUpperBound; //if they are empty is like there is no bound
            std::vector<std::string> mayerCostnames;
        };


        //MARK: OptimalControlProblem implementation

        OptimalControlProblem::OptimalControlProblem()
            : m_pimpl(new OptimalControlProblemPimpl())
        {
            assert(m_pimpl);
            m_pimpl->horizon = TimeRange::Instant(0.0);
            m_pimpl->dynamicalSystem = nullptr;
        }

        OptimalControlProblem::~OptimalControlProblem()
        {
            if(m_pimpl){
                delete(m_pimpl);
                m_pimpl = nullptr;
            }
        }

        bool OptimalControlProblem::setTimeHorizon(double startingTime, double finalTime)
        {
            if (!(m_pimpl->horizon.setTimeInterval(startingTime,finalTime)))
                return false;

            CostsMap::iterator costIterator;
            for (auto mayerCost : m_pimpl->mayerCostnames){
                if (mayerCost.size() > 0){
                    costIterator = m_pimpl->costs.find(mayerCost);
                    if (costIterator == m_pimpl->costs.end()){
                        std::ostringstream errorMsg;
                        errorMsg << "Unable to find cost named "<<mayerCost<< std::endl;
                        reportError("OptimalControlProblem", "updateCostTimeRange", errorMsg.str().c_str());
                        return false;
                    }

                    costIterator->second.timeRange = TimeRange::Instant(finalTime);
                }
            }

            return true;
        }

        double OptimalControlProblem::initialTime() const
        {
            return m_pimpl->horizon.initTime();
        }

        double OptimalControlProblem::finalTime() const
        {
            return m_pimpl->horizon.endTime();
        }

        bool OptimalControlProblem::setDynamicalSystemConstraint(std::shared_ptr<DynamicalSystem> dynamicalSystem)
        {
            m_pimpl->dynamicalSystem = dynamicalSystem;
            return true;
        }

        const std::weak_ptr<const DynamicalSystem> OptimalControlProblem::dynamicalSystem() const
        {
            return m_pimpl->dynamicalSystem;
        }

        bool OptimalControlProblem::addGroupOfConstraints(std::shared_ptr<ConstraintsGroup> groupOfConstraints)
        {
            std::pair< ConstraintsGroupsMap::iterator, bool> groupResult;
            groupResult = m_pimpl->constraintsGroups.insert(std::pair< std::string, std::shared_ptr<ConstraintsGroup>>(groupOfConstraints->name(), groupOfConstraints));

            if(!groupResult.second){
                std::ostringstream errorMsg;
                errorMsg << "A group (or a constraint) named " << groupOfConstraints->name() <<" already exists.";
                reportError("OptimalControlProblem", "addGroupOfConstraints", errorMsg.str().c_str());
                return false;
            }
            return true;
        }

        bool OptimalControlProblem::removeGroupOfConstraints(const std::string &name)
        {
            ConstraintsGroupsMap::iterator groupIterator;
            groupIterator = m_pimpl->constraintsGroups.find(name);
            if(groupIterator != m_pimpl->constraintsGroups.end()){
                if(m_pimpl->constraintsGroups.erase(name)){
                    return true;
                } else {
                    std::ostringstream errorMsg;
                    errorMsg << "Failed to group named "<<name<< std::endl;
                    reportError("OptimalControlProblem", "removeGroupOfConstraints", errorMsg.str().c_str());
                    return false;
                }
            }

            std::ostringstream errorMsg;
            errorMsg << "Unable to find group named "<<name<< std::endl;
            reportError("OptimalControlProblem", "removeGroupOfConstraints", errorMsg.str().c_str());
            return false;
        }

        bool OptimalControlProblem::addContraint(std::shared_ptr<Constraint> newConstraint)
        {
            if (!newConstraint){
                reportError("OptimalControlProblem", "addContraint", "Invalid constraint pointer.");
                return false;
            }

            std::shared_ptr<ConstraintsGroup> dummyGroup =
                    std::make_shared<ConstraintsGroup>(newConstraint->name(),newConstraint->constraintSize());

            if(!dummyGroup){
                reportError("OptimalControlProblem", "addContraint", "Failed in adding constraint.");
                return false;
            }

            if(!(dummyGroup->addConstraint(newConstraint, TimeRange::AnyTime()))){
                return false;
            }
            return addGroupOfConstraints(dummyGroup);
        }

        bool OptimalControlProblem::removeConstraint(const std::string &name)
        {
            ConstraintsGroupsMap::iterator groupIterator;
            groupIterator = m_pimpl->constraintsGroups.find(name);
            if(groupIterator != m_pimpl->constraintsGroups.end()){
                if(groupIterator->second->isAnyTimeGroup()){
                    if(m_pimpl->constraintsGroups.erase(name)){
                        return true;
                    }
                    else {
                        std::ostringstream errorMsg;
                        errorMsg << "Failed to remove constraint named "<<name<< std::endl;
                        reportError("OptimalControlProblem", "removeConstraint", errorMsg.str().c_str());
                        return false;
                    }
                } else {
                    std::ostringstream errorMsg;
                    errorMsg << name << " is a group. To remove it use the method removeGroupOfConstraints instead."<< std::endl;
                    reportError("OptimalControlProblem", "removeConstraint", errorMsg.str().c_str());
                    return false;
                }

            }
            std::ostringstream errorMsg;
            errorMsg << "Unable to find constraint named "<<name<< std::endl;
            reportError("OptimalControlProblem", "removeConstraint", errorMsg.str().c_str());
            return false;
        }

        unsigned int OptimalControlProblem::countConstraints() const
        {
            unsigned int number = 0;

            for(auto group: m_pimpl->constraintsGroups){
                number += group.second->numberOfConstraints();
            }

            return number;
        }

        const std::vector<std::string> OptimalControlProblem::listConstraints() const
        {
            std::vector<std::string> output;
            std::vector<std::string> temp;

            for(auto group: m_pimpl->constraintsGroups){
                temp =  group.second->listConstraints();
                output.insert(output.end(), temp.begin(), temp.end());
            }
            return output;
        }

        const std::vector<std::string> OptimalControlProblem::listGroups() const
        {
            std::vector<std::string> output;

            for(auto group: m_pimpl->constraintsGroups){
                output.insert(output.end(), group.second->numberOfConstraints(), group.second->name());
            }
            return output;
        }

        bool OptimalControlProblem::addMayerTerm(double weight, std::shared_ptr<Cost> cost)
        {
            if (!cost){
                reportError("OptimalControlProblem", "addMayerTerm", "Empty cost pointer");
                return false;
            }
            TimedCost newCost;
            newCost.cost = cost;
            newCost.weight = weight;
            newCost.timeRange.setTimeInterval(m_pimpl->horizon.endTime(), m_pimpl->horizon.endTime());

            std::pair<CostsMap::iterator, bool> costResult;
            costResult = m_pimpl->costs.insert(std::pair<std::string, TimedCost>(cost->name(), newCost));
            if(!costResult.second){
                std::ostringstream errorMsg;
                errorMsg << "A cost named " << cost->name() <<" already exists.";
                reportError("OptimalControlProblem", "addMayerTerm", errorMsg.str().c_str());
                return false;
            }
            m_pimpl->mayerCostnames.push_back(cost->name());
            return true;
        }

        bool OptimalControlProblem::addLagrangeTerm(double weight, std::shared_ptr<Cost> cost)
        {
            if (!cost){
                reportError("OptimalControlProblem", "addLagrangeTerm", "Empty cost pointer");
                return false;
            }

            TimedCost newCost;
            newCost.cost = cost;
            newCost.weight = weight;
            newCost.timeRange = TimeRange::AnyTime();

            std::pair<CostsMap::iterator, bool> costResult;
            costResult = m_pimpl->costs.insert(std::pair<std::string, TimedCost>(cost->name(), newCost));
            if(!costResult.second){
                std::ostringstream errorMsg;
                errorMsg << "A cost named " << cost->name() <<" already exists.";
                reportError("OptimalControlProblem", "addLagrangeTerm", errorMsg.str().c_str());
                return false;
            }
            return true;
        }

        bool OptimalControlProblem::addLagrangeTerm(double weight, double startingTime, double finalTime, std::shared_ptr<Cost> cost)
        {
            TimeRange timeRange;

            if (!timeRange.setTimeInterval(startingTime, finalTime)){
                std::ostringstream errorMsg;
                errorMsg << "The cost named " << cost->name() <<" has invalid time settings.";
                reportError("OptimalControlProblem", "addLagrangeTerm", errorMsg.str().c_str());
                return false;
            }

            return addLagrangeTerm(weight, timeRange, cost);
        }

        bool OptimalControlProblem::addLagrangeTerm(double weight, const TimeRange &timeRange, std::shared_ptr<Cost> cost)
        {
            TimedCost newCost;
            newCost.cost = cost;
            newCost.weight = weight;
            newCost.timeRange = timeRange;

            std::pair<CostsMap::iterator, bool> costResult;
            costResult = m_pimpl->costs.insert(std::pair<std::string, TimedCost>(cost->name(), newCost));
            if(!costResult.second){
                std::ostringstream errorMsg;
                errorMsg << "A cost named " << cost->name() <<" already exists.";
                reportError("OptimalControlProblem", "addLagrangeTerm", errorMsg.str().c_str());
                return false;
            }
            return true;
        }

        bool OptimalControlProblem::updateCostTimeRange(const std::string &name, double newStartingTime, double newEndTime)
        {
            TimeRange timeRange;

            if (!timeRange.setTimeInterval(newStartingTime, newEndTime)){
                std::ostringstream errorMsg;
                errorMsg << "Invalid time settings. Cannot apply changes to cost " << name <<".";
                reportError("OptimalControlProblem", "updateCostTimeRange", errorMsg.str().c_str());
                return false;
            }

            return updateCostTimeRange(name, timeRange);
        }

        bool OptimalControlProblem::updateCostTimeRange(const std::string &name, const TimeRange &newTimeRange)
        {
            for (auto mayerCost : m_pimpl->mayerCostnames)
                if (name == mayerCost){
                    std::ostringstream errorMsg;
                    errorMsg << "Cannot change the TimeRange of cost named " << name << " since it is a terminal cost." << std::endl;
                    reportError("OptimalControlProblem", "updateCostTimeRange", errorMsg.str().c_str());
                }

            CostsMap::iterator costIterator;
            costIterator = m_pimpl->costs.find(name);
            if (costIterator == m_pimpl->costs.end()){
                std::ostringstream errorMsg;
                errorMsg << "Unable to find cost named "<<name<< std::endl;
                reportError("OptimalControlProblem", "updateCostTimeRange", errorMsg.str().c_str());
                return false;
            }

            costIterator->second.timeRange = newTimeRange;
            return true;
        }

        bool OptimalControlProblem::removeCost(const std::string &name)
        {
            if (m_pimpl->costs.erase(name)){
                for (auto mayerCost : m_pimpl->mayerCostnames)
                    if (name == mayerCost)
                        mayerCost.erase();

                return true;
            }

            std::ostringstream errorMsg;
            errorMsg << "Failed to remove cost named "<<name<< std::endl;
            reportError("OptimalControlProblem", "removeCost", errorMsg.str().c_str());
            return false;
        }

        bool OptimalControlProblem::setStateBoxConstraints(const VectorDynSize &minState, const VectorDynSize &maxState)
        {
            m_pimpl->stateLowerBound = minState;
            m_pimpl->stateUpperBound = maxState;
            return true;
        }

        bool OptimalControlProblem::setControlBoxConstraints(const VectorDynSize &minControl, const VectorDynSize &maxControl)
        {
            m_pimpl->controlLowerBound = minControl;
            m_pimpl->controlUpperBound = maxControl;
            return true;
        }

        bool OptimalControlProblem::costsEvaluation(double time, const VectorDynSize &state, const VectorDynSize &control, double &costValue)
        {
            costValue = 0;
            double addCost;
            for(auto cost : m_pimpl->costs){
                if (cost.second.timeRange.isInRange(time)){
                    if(!cost.second.cost->costEvaluation(time, state, control, addCost)){
                        std::ostringstream errorMsg;
                        errorMsg << "Error while evaluating cost " << cost.second.cost->name() <<" .";
                        reportError("OptimalControlProblem", "costsEvaluation", errorMsg.str().c_str());
                        return false;
                    }
                    costValue += cost.second.weight*addCost;
                }
            }

            return true;
        }

        bool OptimalControlProblem::isFeasiblePoint(double time, const VectorDynSize &state, const VectorDynSize &control)
        {
            for(auto group : m_pimpl->constraintsGroups){
                if(!group.second->isFeasibilePoint(time, state, control))
                    return false;
            }
            return true;
        }

    }
}