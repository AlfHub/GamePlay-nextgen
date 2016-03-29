#include "Base.h"
#include "VertexFormat.h"

namespace gameplay
{

    
VertexFormat::VertexFormat() :
    _vertexSize(0)
{
}
    
VertexFormat::VertexFormat(const Element* elements, unsigned int elementCount) :
    _vertexSize(0)
{
    GP_ASSERT(elements);

    // Copy elements and compute vertex size
    for (unsigned int i = 0; i < elementCount; ++i)
    {
        // Copy element
        Element element;
        memcpy(reinterpret_cast<void*>(&element), reinterpret_cast<const void*>(&elements[i]), sizeof(Element));
        _elements.push_back(element);

        _vertexSize += element.size * sizeof(float);
    }
}

VertexFormat::~VertexFormat()
{
}

const VertexFormat::Element& VertexFormat::getElement(unsigned int index) const
{
    GP_ASSERT(index < _elements.size());
    return _elements[index];
}

unsigned int VertexFormat::getElementCount() const
{
    return (unsigned int)_elements.size();
}

unsigned int VertexFormat::getVertexSize() const
{
    return _vertexSize;
}

bool VertexFormat::operator == (const VertexFormat& f) const
{
    if (_elements.size() != f._elements.size())
        return false;

    for (size_t i = 0, count = _elements.size(); i < count; ++i)
    {
        if (_elements[i] != f._elements[i])
            return false;
    }

    return true;
}

bool VertexFormat::operator != (const VertexFormat& f) const
{
    return !(*this == f);
}

const char* VertexFormat::getSerializedClassName() const
{
    return "gameplay::VertexFormat";
}

void VertexFormat::serialize(Serializer* serializer)
{
    serializer->writeObjectList("elements", _elements.size());
    for (unsigned int i = 0; i < _elements.size(); i++)
    {
        serializer->writeObject(NULL, &_elements[i]);
    }
}

void VertexFormat::deserialize(Serializer* serializer)
{
    unsigned int elementCount = serializer->readObjectList("elements");
    for (unsigned int i = 0; i < elementCount;  i++)
    {
        // Copy element
        Element element;
        serializer->readObject(NULL, &element);
        _elements.push_back(element);
        _vertexSize += element.size * sizeof(float);
    }
}

const char* VertexFormat::enumToString(const char* enumName, int value)
{
    if (std::strcmp("gameplay::VertexFormat::Usage", enumName) == 0)
    {
        switch (value)
        {
            case VertexFormat::POSITION:
                return "POSITION";
            case VertexFormat::NORMAL:
                return "NORMAL";
            case VertexFormat::COLOR:
                return "COLOR";
            case VertexFormat::TANGENT:
                return "TANGENT";
            case VertexFormat::BINORMAL:
                return "BINORMAL";
            case VertexFormat::BLENDWEIGHTS:
                return "BLENDWEIGHTS";
            case VertexFormat::BLENDINDICES:
                return "BLENDINDICES";
            case VertexFormat::TEXCOORD0:
                return "TEXCOORD0";
            case VertexFormat::TEXCOORD1:
                return "TEXCOORD1";
            case VertexFormat::TEXCOORD2:
                return "TEXCOORD2";
            case VertexFormat::TEXCOORD3:
                return "TEXCOORD3";
            case VertexFormat::TEXCOORD4:
                return "TEXCOORD4";
            case VertexFormat::TEXCOORD5:
                return "TEXCOORD5";
            case VertexFormat::TEXCOORD6:
                return "TEXCOORD6";
            case VertexFormat::TEXCOORD7:
                return "TEXCOORD7";
            default:
                return "UNKNOWN";
        }
    }
    return NULL;
}

int VertexFormat::enumParse(const char* enumName, const char* str)
{
    if (std::strcmp("gameplay::VertexFormat::Usage", enumName) == 0)
    {
        if (std::strcmp("POSITION", str) == 0)
            return VertexFormat::POSITION;
        else if (std::strcmp("NORMAL", str) == 0)
            return VertexFormat::NORMAL;
        else if (std::strcmp("COLOR", str) == 0)
            return VertexFormat::COLOR;
        else if (std::strcmp("TANGENT", str) == 0)
            return VertexFormat::TANGENT;
        else if (std::strcmp("BINORMAL", str) == 0)
            return VertexFormat::BINORMAL;
        else if (std::strcmp("BLENDWEIGHTS", str) == 0)
            return VertexFormat::BLENDWEIGHTS;
        else if (std::strcmp("BLENDINDICES", str) == 0)
            return VertexFormat::BLENDINDICES;
        else if (std::strcmp("TEXCOORD0", str) == 0)
            return VertexFormat::TEXCOORD0;
        else if (std::strcmp("TEXCOORD1", str) == 0)
            return VertexFormat::TEXCOORD1;
        else if (std::strcmp("TEXCOORD2", str) == 0)
            return VertexFormat::TEXCOORD2;
        else if (std::strcmp("TEXCOORD3", str) == 0)
            return VertexFormat::TEXCOORD3;
        else if (std::strcmp("TEXCOORD4", str) == 0)
            return VertexFormat::TEXCOORD4;
        else if (std::strcmp("TEXCOORD5", str) == 0)
            return VertexFormat::TEXCOORD5;
        else if (std::strcmp("TEXCOORD6", str) == 0)
            return VertexFormat::TEXCOORD6;
        else if (std::strcmp("TEXCOORD7", str) == 0)
            return VertexFormat::TEXCOORD7;
    }
    return -1;
}


VertexFormat::Element::Element() :
    usage(POSITION), size(0)
{
}

VertexFormat::Element::Element(Usage usage, unsigned int size) :
    usage(usage), size(size)
{
}

bool VertexFormat::Element::operator == (const VertexFormat::Element& e) const
{
    return (size == e.size && usage == e.usage);
}

bool VertexFormat::Element::operator != (const VertexFormat::Element& e) const
{
    return !(*this == e);
}

const char* VertexFormat::Element::getSerializedClassName() const
{
    return "gameplay::VertexFormat::Element";
}

void VertexFormat::Element::serialize(Serializer* serializer)
{
    serializer->writeEnum("usage", "gameplay::VertexFormat::Usage", usage, -1);
    serializer->writeInt("size", size, 0);
}

void VertexFormat::Element::deserialize(Serializer* serializer)
{
    usage = static_cast<VertexFormat::Usage>(serializer->readEnum("usage", "gameplay::VertexFormat::Usage", -1));
    size = serializer->readInt("size", 0);
}

}
