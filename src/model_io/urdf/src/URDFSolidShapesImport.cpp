/*
 * Copyright (C) 2015 Fondazione Istituto Italiano di Tecnologia
 * Authors: Silvio Traversaro
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <iDynTree/ModelIO/URDFModelImport.h>
#include <iDynTree/ModelIO/URDFSolidShapesImport.h>

#include <iDynTree/Model/Model.h>

#include <fstream>
#include <map>

#include "URDFParsingUtils.h"

namespace iDynTree
{

bool solidShapesFromURDF(const std::string & urdf_filename,
                        const Model & model,
                        const std::string urdfGeometryType,
                              ModelSolidShapes & output)
{
    std::ifstream ifs(urdf_filename.c_str());

    if( !ifs.is_open() )
    {
        std::cerr << "[ERROR] iDynTree::geometriesFromURDF : error opening file "
                  << urdf_filename << std::endl;
        return false;
    }

    std::string xml_string( (std::istreambuf_iterator<char>(ifs) ), (std::istreambuf_iterator<char>() ) );

    std::cerr << "xml_string : " <<xml_string << std::endl;

    return solidShapesFromURDFString(xml_string,urdf_filename,model,urdfGeometryType,output);
}

/**
 * URDF files provide a model-wide material database,
 * in the form of <material> tags under <robot>.
 *
 * This material can be refenced from <material>
 * under <link> using the `name` attribute.
 *
 * See http://wiki.ros.org/urdf/XML/link
 */
typedef std::map<std::string,iDynTree::Vector4> URDFModelMaterialDatabase;

void parseURDFMaterial(const URDFModelMaterialDatabase & materialDB,
                       iDynTree::Vector4 & material_rgba,
                       TiXmlElement* materialXml)
{
    bool parsingSuccessfull = false;
    if( materialXml )
    {
        TiXmlElement* colorXml = materialXml->FirstChildElement("color");

        if( colorXml )
        {
            const char * rgba = colorXml->Attribute("rgba");

            if( rgba )
            {
                vector4FromString(std::string(rgba),material_rgba);
                parsingSuccessfull = true;
            }
            else
            {
                reportError("","parseURDFMaterial","Impossible to parse URDF material, color tag has now rgba element");
            }
        }

        TiXmlElement* textureXml = materialXml->FirstChildElement("texture");

        if( textureXml )
        {
            const char * filename = colorXml->Attribute("filename");

            if( filename )
            {
                reportError("","parseURDFMaterial","Impossible to parse URDF material, texture tag not supported by iDynTree.");
            }
        }

        if( !parsingSuccessfull )
        {
            const char * materialName = materialXml->Attribute("name");

            // If there is a valid name, query the model material database
            if( materialName )
            {
                std::string materialNameCpp(materialName);
                URDFModelMaterialDatabase::const_iterator it = materialDB.find(materialNameCpp);

                if( it != materialDB.end() )
                {
                    material_rgba = it->second;
                    parsingSuccessfull = true;
                }
                else
                {
                    std::stringstream ss;
                    ss << "Impossible to parse URDF material, material " << materialNameCpp << " not found in model database.";
                    reportWarning("","parseURDFMaterial",ss.str().c_str());
                }
            }
            else
            { 
                // Error print suppressed until https://github.com/robotology-playground/icub-model-generator/issues/36 is fixed
                // reportError("","parseURDFMaterial","Impossible to parse URDF material, material name attribute not found.");
            }
        }

    }

    if( !parsingSuccessfull )
    {
        if( materialXml )
        {
            // Error print suppressed until https://github.com/robotology-playground/icub-model-generator/issues/36 is fixed
            // reportError("","parseURDFMaterial","Impossible to parse URDF material, setting material to white.");
        }
        // Set default material
        material_rgba(0) = 1.0;
        material_rgba(1) = 1.0;
        material_rgba(2) = 1.0;
        material_rgba(3) = 1.0;
    }

}

void parseURDFModelMaterialDatabase(TiXmlElement* robotXml,
                                    URDFModelMaterialDatabase& materialDB)
{
    URDFModelMaterialDatabase emptyDB;

    for (TiXmlElement* materialXml = robotXml->FirstChildElement("material");
          materialXml; materialXml = materialXml->NextSiblingElement("material"))
    {
         // Get the linkName
         std::string urdfMaterialName = materialXml->Attribute("name");

         // Parse the color
         iDynTree::Vector4 color;

         // Material child of robot element cannot refer to the model
         // database,
         parseURDFMaterial(emptyDB,color,materialXml);

         // Save material in DB
         materialDB[urdfMaterialName] = color;
     }

     return;
}

/**
 * Extract path from filename.
 * \todo check if it works on Windows.
 * See example in http://www.cplusplus.com/reference/string/string/find_last_of/ .
 */
std::string extractPathFromFilename(const std::string filename)
{
    const std::string path_separators = "/\\";


    // If not path separated is found in the string, the filename is referred
    // to the current directory
    size_t lastSep = filename.find_last_of(path_separators);
    if( lastSep == std::string::npos )
    {
        return "";
    }

    return filename.substr(0,lastSep+1);
}
/**
 * Retrieve the location of an external mesh references in the URDF.
 * For now just support meshes contained in the local directory of the urdf file,
 * as the one contained in this folders :
 *  https://github.com/bulletphysics/bullet3/tree/master/data .
 */
std::string getURDFMeshAbsolutePathFilename(const std::string & urdf_filename,
                                            const std::string & mesh_uri)
{
    return extractPathFromFilename(urdf_filename)+mesh_uri;
}

/**
 * Get the filetype of the mesh from the name. It should be either dae (collada) or stl.
 */
std::string getFileExt(const std::string & mesh_uri)
{
    const std::string fileExtSeparator = ".";

    // If not path separated is found in the string, the filename is referred
    // to the current directory
    size_t lastSep = mesh_uri.find_last_of(fileExtSeparator);
    if( lastSep == std::string::npos )
    {
        return "";
    }

    return mesh_uri.substr(lastSep+1);
}


bool addURDFGeometryToModelGeometries(const iDynTree::Model& model,
                                      const std::string urdfLinkName,
                                      const std::string urdfGeomName,
                                      const Transform urdfLink_H_geometry,
                                      const std::string urdf_filename,
                                      const URDFModelMaterialDatabase& materialDB,
                                            ModelSolidShapes & modelGeoms,
                                            TiXmlElement* geomXml,
                                            TiXmlElement* materialXml)
{
    // A urdf link can be either a iDynTree link or an iDynTree frame, solve
    // this ambiguity and get idyntreeLinkName and idynTreeLink_H_geometry
    bool is_iDynTreeLink = model.isLinkNameUsed(urdfLinkName);
    std::string idyntreeLinkName;
    Transform idyntreeLink_H_geometry;

    if( is_iDynTreeLink )
    {
        idyntreeLink_H_geometry = urdfLink_H_geometry;
        idyntreeLinkName = urdfLinkName;
    }
    else
    {
        FrameIndex urdfLinkFrameIndex = model.getFrameIndex(urdfLinkName);

        if( urdfLinkFrameIndex == FRAME_INVALID_INDEX )
        {
            std::cerr << "[ERROR] In parsing geometry of urdf link " << urdfLinkName << std::endl;
            return false;
        }

        idyntreeLink_H_geometry = model.getFrameTransform(urdfLinkFrameIndex)*urdfLink_H_geometry;
        idyntreeLinkName        = model.getLinkName(model.getFrameLink(urdfLinkFrameIndex));
    }

    SolidShape * pGeom = 0;
    bool parseOk = true;
    if( geomXml->FirstChildElement("box") != 0 )
    {
        std::string vec;
        if( geomXml->FirstChildElement("box")->Attribute("size") )
        {
            vec = geomXml->FirstChildElement("box")->Attribute("size");
        }
        else
        {
            std::cerr << "[ERROR] box geometry of link " << urdfLinkName << " does not have size attribute" << std::endl;
            return false;
        }

        Box * pBox = new Box();
        Vector3 boxDim;
        parseOk = vector3FromString(vec,boxDim);
        pBox->x = boxDim(0);
        pBox->y = boxDim(1);
        pBox->z = boxDim(2);
        pGeom = static_cast<SolidShape *>(pBox);
    }

    if( geomXml->FirstChildElement("sphere") != 0 )
    {
        double radiusD;
        parseOk = stringToDoubleWithClassicLocale(geomXml->FirstChildElement("sphere")->Attribute("radius"), radiusD);

        if (parseOk)
        {
            Sphere * pSphere = new Sphere();
            pSphere->radius = radiusD;
            pGeom = static_cast<SolidShape *>(pSphere);
        }
    }

    if( geomXml->FirstChildElement("cylinder") != 0 )
    {
        double radiusD, lengthD;
        parseOk = stringToDoubleWithClassicLocale(geomXml->FirstChildElement("cylinder")->Attribute("radius"),radiusD);
        parseOk = parseOk && stringToDoubleWithClassicLocale(geomXml->FirstChildElement("cylinder")->Attribute("length"),lengthD);

        if (parseOk)
        {
            Cylinder * pCylinder = new Cylinder();
            pCylinder->radius = radiusD;
            pCylinder->length = lengthD;
            pGeom = static_cast<SolidShape *>(pCylinder);
        }
    }

    if( geomXml->FirstChildElement("mesh") != 0 )
    {
        ExternalMesh * pExternalMesh = new ExternalMesh();

        const char * filename = geomXml->FirstChildElement("mesh")->Attribute("filename");

        if( filename )
        {
            // For now we just support urdf with local meshes, see as an example
            // https://github.com/bulletphysics/bullet3/tree/master/data
            std::string localName = filename;
            pExternalMesh->filename = getURDFMeshAbsolutePathFilename(urdf_filename,localName);
            pExternalMesh->scale(0) = pExternalMesh->scale(1) = pExternalMesh->scale(2) = 1.0;

            if ( geomXml->FirstChildElement("mesh")->Attribute("scale") )
            {
                std::string scaleAttr = geomXml->FirstChildElement("mesh")->Attribute("scale");
                parseOk = vector3FromString(scaleAttr,pExternalMesh->scale);
            }

            pGeom = static_cast<SolidShape *>(pExternalMesh);
        }
        else
        {
            std::cerr << "[ERROR] mesh geometry of link " << urdfLinkName << " does not have filename attribute" << std::endl;
        }
    }

    if( !pGeom || !parseOk )
    {
        std::cerr << "[ERROR] In parsing geometry of urdf link " << urdfLinkName << std::endl;
        return false;
    }

    // parse material (eventually checking the material database)
    parseURDFMaterial(materialDB,pGeom->material,materialXml);

    pGeom->name = urdfGeomName;
    pGeom->link_H_geometry = idyntreeLink_H_geometry;

    LinkIndex idyntreeLinkIndex = model.getLinkIndex(idyntreeLinkName);
    modelGeoms.linkSolidShapes[idyntreeLinkIndex].push_back(pGeom);

    return true;
}

bool solidShapesFromURDFString(const std::string & urdf_string,
                               const std::string & urdf_filename,
                               const Model & model,
                               const std::string urdfGeometryType,
                                     iDynTree::ModelSolidShapes & output)
{
    iDynTree::ModelSolidShapes newGeoms;

    newGeoms.resize(model);

    if(urdfGeometryType != "visual" && urdfGeometryType != "collision")
    {
        std::cerr << "[ERROR] unknown urdfGeometryType " << urdfGeometryType << std::endl;
    }

    std::string tagToSearch = urdfGeometryType;

    bool parsingSuccessful = true;

    TiXmlDocument tiUrdfXml;
    tiUrdfXml.Parse(urdf_string.c_str());

    TiXmlElement* robotXml = tiUrdfXml.FirstChildElement("robot");

    if( !robotXml )
    {
        std::cerr << "Impossible to find robot element in urdf xml" << std::endl;
    }

    // Get material database
    URDFModelMaterialDatabase materialDB;
    parseURDFModelMaterialDatabase(robotXml,materialDB);

    // Get all the link elements of robot element
    for (TiXmlElement* linkXml = robotXml->FirstChildElement("link");
          linkXml; linkXml = linkXml->NextSiblingElement("link"))
    {
         // Get the linkName
         std::string urdfLinkName = linkXml->Attribute("name");

         // For each link, get all the visual or collision elements
         for (TiXmlElement* visXml = linkXml->FirstChildElement(tagToSearch);
              visXml; visXml = visXml->NextSiblingElement(tagToSearch))
         {
             // We first get the origin element
             TiXmlElement* originXml = visXml->FirstChildElement("origin");
             // Default value (if origin is missing) is identity
             Transform urdfLink_H_geometry;
             transformFromURDFXML(originXml,urdfLink_H_geometry);

             std::string geomName = tagToSearch;
             if( visXml->Attribute("name") )
             {
                  geomName = visXml->Attribute("name");
             }

             // For each visual or collision element, get its geometry object
             TiXmlElement* geomXml = visXml->FirstChildElement("geometry");
             if(geomXml == 0)
             {
                 std::cerr << "Error in parsing " << tagToSearch << " element, of link " << urdfLinkName << " geometry child element not found" << std::endl;
                 return false;
             }

             TiXmlElement* materialXml = visXml->FirstChildElement("material");

             parsingSuccessful = parsingSuccessful && addURDFGeometryToModelGeometries(model,urdfLinkName,geomName,urdfLink_H_geometry,urdf_filename,materialDB,
                                                                                       newGeoms,geomXml,materialXml);
         }
     }

     // If the parsing was successful,
     // copy the obtained sensors to the output structure
     if( parsingSuccessful )
     {
         output = newGeoms;
     }

     return parsingSuccessful;
}


}

