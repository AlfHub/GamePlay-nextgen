#include "Base.h"
#include "Technique.h"
#include "Material.h"
#include "Node.h"

namespace gameplay
{

Technique::Technique() :
    _id(""), _material(NULL)
{
}

Technique::Technique(const char* id, Material* material) :
    _id(id ? id : ""), _material(material)
{
    RenderState::_parent = material;
}

Technique::~Technique()
{
    for (size_t i = 0, count = _passes.size(); i < count; ++i)
    {
        SAFE_RELEASE(_passes[i]);
    }
}

const char* Technique::getId() const
{
    return _id.c_str();
}

unsigned int Technique::getPassCount() const
{
    return (unsigned int)_passes.size();
}

Pass* Technique::getPassByIndex(unsigned int index) const
{
    GP_ASSERT(index < _passes.size());
    return _passes[index];
}

Pass* Technique::getPass(const char* id) const
{
    GP_ASSERT(id);

    for (size_t i = 0, count = _passes.size(); i < count; ++i)
    {
        Pass* pass = _passes[i];
        GP_ASSERT(pass);
        if (strcmp(pass->getId(), id) == 0)
        {
            return pass;
        }
    }
    return NULL;
}

void Technique::setNodeBinding(Node* node)
{
    RenderState::setNodeBinding(node);

    for (size_t i = 0, count = _passes.size(); i < count; ++i)
    {
        _passes[i]->setNodeBinding(node);
    }
}

Technique* Technique::clone(Material* material, NodeCloneContext &context) const
{
    Technique* technique = new Technique(getId(), material);
    for (std::vector<Pass*>::const_iterator it = _passes.begin(); it != _passes.end(); ++it)
    {
        Pass* pass = *it;
        GP_ASSERT(pass);
        Pass* passCopy = pass->clone(technique, context);
        GP_ASSERT(passCopy);
        technique->_passes.push_back(passCopy);
    }
    RenderState::cloneInto(technique, context);
    technique->_parent = material;
    return technique;
}

const char* Technique::getSerializedClassName() const
{
    return "gameplay::Technique";
}

void Technique::serialize(Serializer* serializer)
{
    serializer->writeString("id", _id.c_str(), "");
    RenderState::serialize(serializer);
    serializer->writeObjectList("passes", _passes.size());
    for (unsigned int i = 0; i < _passes.size(); i++)
    {
        serializer->writeObject(NULL, _passes[i]);
    }
}

void Technique::deserialize(Serializer* serializer)
{
    serializer->readString("id", _id, "");
    RenderState::deserialize(serializer);
    unsigned int passCount = serializer->readObjectList("passes");
    for (unsigned int i = 0; i < passCount; i++)
    {
        Pass* pass = dynamic_cast<Pass*>(serializer->readObject(NULL));
        pass->_technique = this;
        pass->_parent = this;
        _passes.push_back(pass);
    }
}

Serializable* Technique::createInstance()
{
    return static_cast<Serializable*>(new Technique());
}

}
