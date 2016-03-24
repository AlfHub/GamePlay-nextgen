#include "Base.h"
#include "Serializer.h"
#include "SerializerBinary.h"
#include "SerializerJson.h"
#include "Scene.h"
#include "Node.h"
#include "MeshPart.h"
#include "Joint.h"

namespace gameplay
{

static Serializer::Activator* __activator = NULL;

Serializer::Activator::Activator()
{
}

Serializer::Activator::~Activator()
{
}
    
void Serializer::Activator::initializeClasses()
{
    // Register any Serializable classes that are used as pointers within the framework.
    /*
    Serializer::getActivator()->registerClass("gameplay::Game::Config", &Game::Config::createInstance);
    Serializer::getActivator()->registerClass("gameplay::Scene", &Scene::createInstance);
    Serializer::getActivator()->registerClass("gameplay::Node", &Node::createInstance);
    Serializer::getActivator()->registerClass("gameplay::Camera", &Camera::createInstance);
    Serializer::getActivator()->registerClass("gameplay::Light", &Light::createInstance);
    Serializer::getActivator()->registerClass("gameplay::Model", &Model::createInstance);
    Serializer::getActivator()->registerClass("gameplay::Mesh", &Mesh::createInstance);
    Serializer::getActivator()->registerClass("gameplay::MeshPart", &MeshPart::createInstance);
    Serializer::getActivator()->registerClass("gameplay::MeshSkin", &MeshSkin::createInstance);
    Serializer::getActivator()->registerClass("gameplay::Material", &Material::createInstance);
    Serializer::getActivator()->registerClass("gameplay::MaterialParameter", &MaterialParameter::createInstance);
    Serializer::getActivator()->registerClass("gameplay::Technique", &Technique::createInstance);
    Serializer::getActivator()->registerClass("gameplay::Pass", &Pass::createInstance);
    Serializer::getActivator()->registerClass("gameplay::Joint", &Joint::createInstance);
    Serializer::getActivator()->registerClass("gameplay::Texture", &Texture::createInstance);
    Serializer::getActivator()->registerClass("gameplay::Texture::Sampler", &Texture::Sampler::createInstance);
    */
    // TODO: All the other classes...
}

void Serializer::Activator::initializeEnums()
{
    // Register enums used within serialized objects in the framework
    /*
    Serializer::getActivator()->registerEnum("gameplay::Camera::Type", &Camera::enumToString, &Camera::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::Light::Type", &Light::enumToString, &Light::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::Mesh::IndexFormat", &Mesh::enumToString, &Mesh::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::Mesh::PrimitiveType", &Mesh::enumToString, &Mesh::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::VertexFormat::Usage", &VertexFormat::enumToString, &VertexFormat::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::RenderState::AutoBinding", &RenderState::enumToString, &RenderState::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::RenderState::BlendMode", &RenderState::enumToString, &RenderState::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::RenderState::CullFaceSide", &RenderState::enumToString, &RenderState::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::RenderState::FrontFace", &RenderState::enumToString, &RenderState::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::RenderState::DepthFunc", &RenderState::enumToString, &RenderState::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::RenderState::StencilFunc", &RenderState::enumToString, &RenderState::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::RenderState::StencilOp", &RenderState::enumToString, &RenderState::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::MaterialParameter::Type", &MaterialParameter::enumToString, &MaterialParameter::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::Texture::Format", &Texture::enumToString, &Texture::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::Texture::Filter", &Texture::enumToString, &Texture::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::Texture::Wrap", &Texture::enumToString, &Texture::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::Texture::Type", &Texture::enumToString, &Texture::enumParse);
    Serializer::getActivator()->registerEnum("gameplay::Texture::CubeFace", &Texture::enumToString, &Texture::enumParse);
    */
    // TODO: All the other classes...
}
    
Serializable* Serializer::Activator::createInstance(const char* className)
{
    if (!className)
        return NULL;
    
    Serializable* instance = NULL;
    std::string key = std::string(className);
    std::map<std::string,CreateInstanceCallback>::const_iterator itr = __activator->_classes.find(key);
    if ( itr == __activator->_classes.end() )
        return NULL;
    
    CreateInstanceCallback createInstance = itr->second;
    if (createInstance)
        instance = createInstance();
    return instance;
}
    
const char* Serializer::Activator::enumToString(const char* enumName, int value)
{
    if (!enumName)
        return NULL;
    
    std::string key = std::string(enumName);
    std::map<std::string,std::pair<EnumToStringCallback, EnumParseCallback> >::const_iterator itr = __activator->_enums.find(key);
    if ( itr != __activator->_enums.end() )
    {
        EnumToStringCallback enumToString = itr->second.first;
        if (enumToString)
            return enumToString(enumName, value);
    }
    return NULL;
}

int Serializer::Activator::enumParse(const char* enumName, const char* str)
{
    if (!enumName)
        return NULL;
    
    std::string key = std::string(enumName);
    std::map<std::string,std::pair<EnumToStringCallback, EnumParseCallback> >::const_iterator itr = __activator->_enums.find(key);
    if ( itr != __activator->_enums.end() )
    {
        EnumParseCallback enumParse = itr->second.second;
        if (enumParse)
            return enumParse(enumName, str);
    }
    return NULL;
}

void Serializer::Activator::registerClass(const char* className, CreateInstanceCallback createInstance)
{
    GP_ASSERT(className);
    
    std::string key = className;
    std::map<std::string,CreateInstanceCallback>::const_iterator itr = __activator->_classes.find(key);
    if ( itr == __activator->_classes.end() )
    {
        __activator->_classes[key] = createInstance;
    }
    else
    {
        GP_ERROR("className already registered:%s", className);
    }
}
    
void Serializer::Activator::registerEnum(const char* enumName, EnumToStringCallback toString, EnumParseCallback parse)
{
    GP_ASSERT(enumName);
    
    std::string key = enumName;
    std::map<std::string, std::pair<EnumToStringCallback, EnumParseCallback> >::const_iterator itr = __activator->_enums.find(key);
    if ( itr == __activator->_enums.end() )
    {
        __activator->_enums[key] = std::make_pair(toString, parse);
    }
    else
    {
        GP_ERROR("enumName already registered:%s", enumName);
    }
}

Serializer::Serializer(const char* path, Stream* stream, unsigned int versionMajor, unsigned int versionMinor)
    : _stream(stream), _type(READER)
{
    _path = path;
    _version[0] = versionMajor;
    _version[1] = versionMinor;
}

Serializer::~Serializer()
{
    _stream->close();
    SAFE_DELETE(_stream);
}

Serializer* Serializer::createReader(const char* path)
{
    GP_ASSERT(path);
    
    Stream* stream = FileSystem::open(path);
    if (!stream)
    {
        GP_WARN("Failed to open file '%s'.", path);
        return NULL;
    }
    Serializer* serializer = SerializerBinary::create(path, stream);
    if (!serializer)
    {
        stream->rewind();
        serializer = SerializerJson::create(path, stream);
    }
    return serializer;
}

const std::string& Serializer::getPath() const
{
    return _path;
}

Serializer::Activator* Serializer::getActivator()
{
    if (!__activator)
    {
        __activator = new Serializer::Activator();
        __activator->initializeClasses();
        __activator->initializeEnums();
    }
    
    return __activator;
}
    
unsigned int Serializer::getVersionMajor() const
{
    return (unsigned int)_version[0];
}

unsigned int Serializer::getVersionMinor() const
{
    return (unsigned int)_version[1];
}

}
