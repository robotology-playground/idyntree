// SPDX-FileCopyrightText: Fondazione Istituto Italiano di Tecnologia (IIT)
// SPDX-License-Identifier: BSD-3-Clause

#include "iDynTree/ModelIO/ModelLoader.h"

#include "URDFDocument.h"

#include <iDynTree/XMLParser.h>
#include <iDynTree/Sensors/ModelSensorsTransformers.h>


#include <string>
#include <vector>

namespace iDynTree
{

    ModelParserOptions::ModelParserOptions()
    : addSensorFramesAsAdditionalFrames(true)
    , originalFilename("") {}


    class ModelLoader::ModelLoaderPimpl {
    public:
        Model m_model;
        SensorsList m_sensors;
        bool m_isModelValid;
        ModelParserOptions m_options;

        bool setModelAndSensors(const Model& _model, const SensorsList& _sensors);
    };

    bool ModelLoader::ModelLoaderPimpl::setModelAndSensors(const Model& _model,
                                                           const SensorsList& _sensors)
    {

        m_model = _model;
        m_sensors = _sensors;

        // TODO \todo add a self consistency check of model/sensors
        m_isModelValid = true;

        if (!m_isModelValid)
        {
            reportError("ModelLoader","setModelAndSensors","Loading failed, resetting ModelLoader to be invalid");
            m_model = Model();
            m_sensors = SensorsList();
        }

        return m_isModelValid;
    }

    ModelLoader::ModelLoader()
    : m_pimpl(new ModelLoaderPimpl())
    {
        m_pimpl->m_isModelValid = false;
    }

    ModelLoader::~ModelLoader() {}

    const Model& ModelLoader::model()
    {
        return m_pimpl->m_model;
    }

    const SensorsList& ModelLoader::sensors()
    {
        return m_pimpl->m_sensors;
    }

    bool ModelLoader::isValid()
    {
        return m_pimpl->m_isModelValid;
    }


    const ModelParserOptions& ModelLoader::parsingOptions() const { return m_pimpl->m_options; }

    void ModelLoader::setParsingOptions(const ModelParserOptions& options)
    {
        m_pimpl->m_options = options;
    }

    bool ModelLoader::loadModelFromFile(const std::string& filename,
                                        const std::string& /*filetype*/,
                                        const std::vector<std::string>& packageDirs /* = {} */)
    {
        // Allocate parser
        std::shared_ptr<XMLParser> parser = std::make_shared<XMLParser>();
        parser->setDocumentFactory([](XMLParserState& state){ 
            return std::shared_ptr<XMLDocument>(new URDFDocument(state)); 
            });
        parser->setPackageDirs(packageDirs);
        if (!parser->parseXMLFile(filename)) {
            reportError("ModelLoader", "loadModelFromFile", "Error in parsing model from URDF.");
            return false;
        }
        // Retrieving the parsed document, which is an instance of URDFDocument
        std::shared_ptr<const XMLDocument> document = parser->document();
        std::shared_ptr<const URDFDocument> urdfDocument = std::dynamic_pointer_cast<const URDFDocument>(document);
        if (!urdfDocument) {
            reportError("ModelLoader", "loadModelFromFile", "Fatal error in retrieving the parsed model.");
            return false;
        }

        return m_pimpl->setModelAndSensors(urdfDocument->model(),urdfDocument->sensors());
    }

    bool ModelLoader::loadModelFromString(const std::string& modelString,
                                          const std::string& /*filetype*/,
                                          const std::vector<std::string>& packageDirs /* = {} */)
    {
        // Allocate parser
        std::shared_ptr<XMLParser> parser = std::make_shared<XMLParser>();
        parser->setDocumentFactory([](XMLParserState& state){
            return std::shared_ptr<XMLDocument>(new URDFDocument(state)); 
            });
        parser->setPackageDirs(packageDirs);
        if (!parser->parseXMLString(modelString)) {
            reportError("ModelLoader", "loadModelFromString", "Error in parsing model from URDF.");
            return false;
        }
        // Retrieving the parsed document, which is an instance of URDFDocument
        std::shared_ptr<const XMLDocument> document = parser->document();
        std::shared_ptr<const URDFDocument> urdfDocument = std::dynamic_pointer_cast<const URDFDocument>(document);
        if (!urdfDocument) {
            reportError("ModelLoader", "loadModelFromString", "Fatal error in retrieving the parsed model.");
            return false;
        }

        return m_pimpl->setModelAndSensors(urdfDocument->model(),urdfDocument->sensors());
    }

    bool ModelLoader::loadReducedModelFromFullModel(const Model& fullModel,
                                                    const std::vector< std::string >& consideredJoints,
                                                    const std::string /*filetype*/)
    {
        SensorsList _sensorsFull, _sensorsReduced;
        Model _modelReduced;
        _modelReduced.setPackageDirs(fullModel.getPackageDirs());
        bool ok = createReducedModelAndSensors(fullModel,_sensorsFull,consideredJoints,_modelReduced,_sensorsReduced);

        if( !ok )
        {
            return false;
        }

        return m_pimpl->setModelAndSensors(_modelReduced,_sensorsReduced);
    }

    bool ModelLoader::loadReducedModelFromString(const std::string modelString,
                                                 const std::vector< std::string >& consideredJoints,
                                                 const std::string filetype,
                                                 const std::vector<std::string>& packageDirs /*= {}*/)
    {
        bool parsingCorrect = loadModelFromString(modelString, filetype, packageDirs);
        if (!parsingCorrect) return false;
        SensorsList _sensorsFull = m_pimpl->m_sensors, _sensorsReduced;
        Model _modelFull = m_pimpl->m_model, _modelReduced;
        _modelReduced.setPackageDirs(packageDirs);

        parsingCorrect = createReducedModelAndSensors(_modelFull, _sensorsFull,
                                                      consideredJoints,
                                                      _modelReduced, _sensorsReduced);

        if (!parsingCorrect)
        {
            return false;
        }

        return m_pimpl->setModelAndSensors(_modelReduced, _sensorsReduced);
    }

    bool ModelLoader::loadReducedModelFromFile(const std::string filename,
                                               const std::vector< std::string >& consideredJoints,
                                               const std::string filetype,
                                               const std::vector<std::string>& packageDirs /*= {}*/)
    {
        bool parsingCorrect = loadModelFromFile(filename, filetype, packageDirs);
        if (!parsingCorrect) return false;
        SensorsList _sensorsFull = m_pimpl->m_sensors, _sensorsReduced;
        Model _modelFull = m_pimpl->m_model, _modelReduced;
        _modelReduced.setPackageDirs(packageDirs);

        parsingCorrect = createReducedModelAndSensors(_modelFull,_sensorsFull,consideredJoints,_modelReduced,_sensorsReduced);

        if (!parsingCorrect)
        {
            return false;
        }

        return m_pimpl->setModelAndSensors(_modelReduced,_sensorsReduced);
    }
}
