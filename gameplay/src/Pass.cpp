#include "Base.h"
#include "Pass.h"
#include "Technique.h"
#include "Material.h"
#include "Node.h"

namespace gameplay
{

Pass::Pass() :
    _id(""), _technique(NULL), _effect(NULL), _vaBinding(NULL)
{
}

Pass::Pass(const char* id, Technique* technique) :
    _id(id ? id : ""), _technique(technique), _effect(NULL), _vaBinding(NULL)
{
    RenderState::_parent = _technique;
}

Pass::~Pass()
{
    SAFE_RELEASE(_effect);
    SAFE_RELEASE(_vaBinding);
}

bool Pass::initialize(const char* vshPath, const char* fshPath, const char* defines)
{
    GP_ASSERT(vshPath);
    GP_ASSERT(fshPath);

    SAFE_RELEASE(_effect);
    SAFE_RELEASE(_vaBinding);

    // Attempt to create/load the effect.
    _effect = Effect::createFromFile(vshPath, fshPath, defines);
    if (_effect == NULL)
    {
        GP_WARN("Failed to create effect for pass. vertexShader = %s, fragmentShader = %s, defines = %s", vshPath, fshPath, defines ? defines : "");
        return false;
    }

    return true;
}

const char* Pass::getId() const
{
    return _id.c_str();
}

Effect* Pass::getEffect() const
{
    return _effect;
}

void Pass::setVertexAttributeBinding(VertexAttributeBinding* binding)
{
    SAFE_RELEASE(_vaBinding);

    if (binding)
    {
        _vaBinding = binding;
        binding->addRef();
    }
}

VertexAttributeBinding* Pass::getVertexAttributeBinding() const
{
    return _vaBinding;
}

void Pass::bind()
{
    GP_ASSERT(_effect);

    // Bind our effect.
    _effect->bind();

    // Bind our render state
    RenderState::bind(this);

    // If we have a vertex attribute binding, bind it
    if (_vaBinding)
    {
        _vaBinding->bind();
    }
}

void Pass::unbind()
{
    // If we have a vertex attribute binding, unbind it
    if (_vaBinding)
    {
        _vaBinding->unbind();
    }
}

Pass* Pass::clone(Technique* technique, NodeCloneContext &context) const
{
    GP_ASSERT(_effect);
    _effect->addRef();

    Pass* pass = new Pass(getId(), technique);
    pass->_effect = _effect;

    RenderState::cloneInto(pass, context);
    pass->_parent = technique;
    return pass;
}

const char* Pass::getSerializedClassName() const
{
    return "gameplay::Pass";
}

void Pass::serialize(Serializer* serializer)
{
    serializer->writeString("id", _id.c_str(), "");
    serializer->writeString("vertexShader", _effect->_vshPath.c_str(), "");
    serializer->writeString("fragmentShader", _effect->_fshPath.c_str(), "");
    serializer->writeString("defines", _effect->_defines.c_str(), "");
    RenderState::serialize(serializer);
}

void Pass::deserialize(Serializer* serializer)
{
    serializer->readString("id", _id, "");
    std::string vshPath;
    serializer->readString("vertexShader", vshPath, "");
    std::string fshPath;
    serializer->readString("fragmentShader", fshPath, "");
    std::string defines;
    serializer->readString("defines", defines, "");
    _effect = Effect::createFromFile(vshPath.c_str(), fshPath.c_str(), defines.c_str());
    RenderState::deserialize(serializer);
}

Serializable* Pass::createInstance()
{
    return static_cast<Serializable*>(new Pass());
}

}
