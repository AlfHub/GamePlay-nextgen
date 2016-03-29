#include "Base.h"
#include "Mesh.h"
#include "MeshPart.h"
#include "Effect.h"
#include "Model.h"
#include "Material.h"

namespace gameplay
{

Mesh::Mesh() :
    _primitiveType(TRIANGLES),
    _vertexCount(0), _vertexBuffer(0),
    _partCount(0), _parts(NULL), _dynamic(false)
{
}

Mesh::Mesh(const VertexFormat& vertexFormat) :
    _primitiveType(TRIANGLES), _vertexFormat(vertexFormat),
    _vertexCount(0), _vertexBuffer(0),
    _partCount(0), _parts(NULL), _dynamic(false)
{
}

Mesh::~Mesh()
{
    if (_parts)
    {
        for (unsigned int i = 0; i < _partCount; ++i)
        {
            SAFE_DELETE(_parts[i]);
        }
        SAFE_DELETE_ARRAY(_parts);
    }

    if (_vertexBuffer)
    {
        glDeleteBuffers(1, &_vertexBuffer);
        _vertexBuffer = 0;
    }
}

Mesh* Mesh::createMesh(const VertexFormat& vertexFormat, unsigned int vertexCount, bool dynamic)
{
    GLuint vbo;
    GL_ASSERT( glGenBuffers(1, &vbo) );
    GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, vbo) );
    GL_ASSERT( glBufferData(GL_ARRAY_BUFFER, vertexFormat.getVertexSize() * vertexCount, NULL, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) );

    Mesh* mesh = new Mesh(vertexFormat);
    mesh->_vertexCount = vertexCount;
    mesh->_vertexBuffer = vbo;
    mesh->_dynamic = dynamic;

    return mesh;
}


Mesh* Mesh::createQuad(float x, float y, float width, float height, float s1, float t1, float s2, float t2)
{
    float x2 = x + width;
    float y2 = y + height;

    float vertexData[] =
    {
        x, y2, 0,   0, 0, 1,    s1, t2,
        x, y, 0,    0, 0, 1,    s1, t1,
        x2, y2, 0,  0, 0, 1,    s2, t2,
        x2, y, 0,   0, 0, 1,    s2, t1,
    };

    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3),
        VertexFormat::Element(VertexFormat::NORMAL, 3),
        VertexFormat::Element(VertexFormat::TEXCOORD0, 2)
    };
    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 3), 4, false);
    if (mesh == NULL)
    {
        GP_ERROR("Failed to create mesh.");
        return NULL;
    }

    mesh->_primitiveType = TRIANGLE_STRIP;
    mesh->setVertexData(vertexData, 0, 4);

    return mesh;
}

Mesh* Mesh::createQuadFullscreen()
{
    float x = -1.0f;
    float y = -1.0f;
    float x2 = 1.0f;
    float y2 = 1.0f;

    float vertexData[] =
    {
        x, y2,   0, 1,
        x, y,    0, 0,
        x2, y2,  1, 1,
        x2, y,   1, 0
    };

    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 2),
        VertexFormat::Element(VertexFormat::TEXCOORD0, 2)
    };
    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 2), 4, false);
    if (mesh == NULL)
    {
        GP_ERROR("Failed to create mesh.");
        return NULL;
    }

    mesh->_primitiveType = TRIANGLE_STRIP;
    mesh->setVertexData(vertexData, 0, 4);

    return mesh;
}

Mesh* Mesh::createQuad(const Vector3& p1, const Vector3& p2, const Vector3& p3, const Vector3& p4)
{
    // Calculate the normal vector of the plane.
    Vector3 v1, v2, n;
    Vector3::subtract(p2, p1, &v1);
    Vector3::subtract(p3, p2, &v2);
    Vector3::cross(v1, v2, &n);
    n.normalize();

    float vertexData[] =
    {
        p1.x, p1.y, p1.z, n.x, n.y, n.z, 0, 1,
        p2.x, p2.y, p2.z, n.x, n.y, n.z, 0, 0,
        p3.x, p3.y, p3.z, n.x, n.y, n.z, 1, 1,
        p4.x, p4.y, p4.z, n.x, n.y, n.z, 1, 0
    };

    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3),
        VertexFormat::Element(VertexFormat::NORMAL, 3),
        VertexFormat::Element(VertexFormat::TEXCOORD0, 2)
    };

    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 3), 4, false);
    if (mesh == NULL)
    {
        GP_ERROR("Failed to create mesh.");
        return NULL;
    }

    mesh->_primitiveType = TRIANGLE_STRIP;
    mesh->setVertexData(vertexData, 0, 4);

    return mesh;
}

Mesh* Mesh::createLines(Vector3* points, unsigned int pointCount)
{
    GP_ASSERT(points);
    GP_ASSERT(pointCount);

    float* vertexData = new float[pointCount*3];
    memcpy(vertexData, points, pointCount*3*sizeof(float));

    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3)
    };
    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 1), pointCount, false);
    if (mesh == NULL)
    {
        GP_ERROR("Failed to create mesh.");
        SAFE_DELETE_ARRAY(vertexData);
        return NULL;
    }

    mesh->_primitiveType = LINE_STRIP;
    mesh->setVertexData(vertexData, 0, pointCount);

    SAFE_DELETE_ARRAY(vertexData);
    return mesh;
}

Mesh* Mesh::createBoundingBox(const BoundingBox& box)
{
    Vector3 corners[8];
    box.getCorners(corners);

    float vertexData[] =
    {
        corners[7].x, corners[7].y, corners[7].z,
        corners[6].x, corners[6].y, corners[6].z,
        corners[1].x, corners[1].y, corners[1].z,
        corners[0].x, corners[0].y, corners[0].z,
        corners[7].x, corners[7].y, corners[7].z,
        corners[4].x, corners[4].y, corners[4].z,
        corners[3].x, corners[3].y, corners[3].z, 
        corners[0].x, corners[0].y, corners[0].z,
        corners[0].x, corners[0].y, corners[0].z,
        corners[1].x, corners[1].y, corners[1].z,
        corners[2].x, corners[2].y, corners[2].z,
        corners[3].x, corners[3].y, corners[3].z, 
        corners[4].x, corners[4].y, corners[4].z,
        corners[5].x, corners[5].y, corners[5].z, 
        corners[2].x, corners[2].y, corners[2].z,
        corners[1].x, corners[1].y, corners[1].z,
        corners[6].x, corners[6].y, corners[6].z,
        corners[5].x, corners[5].y, corners[5].z
    };

    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3)
    };
    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 1), 18, false);
    if (mesh == NULL)
    {
        GP_ERROR("Failed to create mesh.");
        return NULL;
    }

    mesh->_primitiveType = LINE_STRIP;
    mesh->setVertexData(vertexData, 0, 18);

    return mesh;
}

const char* Mesh::getUrl() const
{
    return _url.c_str();
}

const VertexFormat& Mesh::getVertexFormat() const
{
    return _vertexFormat;
}

unsigned int Mesh::getVertexCount() const
{
    return _vertexCount;
}

unsigned int Mesh::getVertexSize() const
{
    return _vertexFormat.getVertexSize();
}

VertexBufferHandle Mesh::getVertexBuffer() const
{
    return _vertexBuffer;
}

bool Mesh::isDynamic() const
{
    return _dynamic;
}

Mesh::PrimitiveType Mesh::getPrimitiveType() const
{
    return _primitiveType;
}

void Mesh::setPrimitiveType(PrimitiveType type)
{
    _primitiveType = type;
}

void* Mesh::mapVertexBuffer()
{
    GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer) );

    return (void*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

bool Mesh::unmapVertexBuffer()
{
    return glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Mesh::setVertexData(const void* vertexData, unsigned int vertexStart, unsigned int vertexCount)
{
    GL_ASSERT( glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer) );

    if (vertexStart == 0 && vertexCount == 0)
    {
        GL_ASSERT( glBufferData(GL_ARRAY_BUFFER, _vertexFormat.getVertexSize() * _vertexCount, vertexData, _dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW) );
    }
    else
    {
        if (vertexCount == 0)
        {
            vertexCount = _vertexCount - vertexStart;
        }

        GL_ASSERT( glBufferSubData(GL_ARRAY_BUFFER, vertexStart * _vertexFormat.getVertexSize(), vertexCount * _vertexFormat.getVertexSize(), vertexData) );
    }
}

MeshPart* Mesh::addPart(PrimitiveType primitiveType, IndexFormat indexFormat, unsigned int indexCount, bool dynamic)
{
    MeshPart* part = MeshPart::create(this, _partCount, primitiveType, indexFormat, indexCount, dynamic);
    if (part)
    {
        // Increase size of part array and copy old subets into it.
        MeshPart** oldParts = _parts;
        _parts = new MeshPart*[_partCount + 1];
        for (unsigned int i = 0; i < _partCount; ++i)
        {
            _parts[i] = oldParts[i];
        }

        // Add new part to array.
        _parts[_partCount++] = part;

        // Delete old part array.
        SAFE_DELETE_ARRAY(oldParts);
    }

    return part;
}

unsigned int Mesh::getPartCount() const
{
    return _partCount;
}

MeshPart* Mesh::getPart(unsigned int index)
{
    GP_ASSERT(_parts);
    return _parts[index];
}

const BoundingBox& Mesh::getBoundingBox() const
{
    return _boundingBox;
}

void Mesh::setBoundingBox(const BoundingBox& box)
{
    _boundingBox = box;
}

const BoundingSphere& Mesh::getBoundingSphere() const
{
    return _boundingSphere;
}

void Mesh::setBoundingSphere(const BoundingSphere& sphere)
{
    _boundingSphere = sphere;
}

const char* Mesh::getSerializedClassName() const
{
    return "gameplay::Mesh";
}

void Mesh::serialize(Serializer* serializer)
{
    serializer->writeString("url", _url.c_str(), "");
    serializer->writeEnum("primitiveType", "gameplay::Mesh::PrimitiveType", _primitiveType, Mesh::TRIANGLES);
    serializer->writeObject("vertexFormat", &_vertexFormat);
    serializer->writeInt("vertexCount", _vertexCount, 0);
    do
    {
        void* vertexData = mapVertexBuffer();
        serializer->writeByteArray("vertexData", (unsigned char*)vertexData, _vertexFormat.getVertexSize() * _vertexCount);
    } while(!unmapVertexBuffer());
    serializer->writeBool("dynamic", _dynamic, true);
    serializer->writeObjectList("parts", _partCount);
    for (unsigned int i = 0; i < _partCount; i++)
    {
        serializer->writeObject(NULL, _parts[i]);
    }
    serializer->writeObject("boundingBox", _boundingBox.isEmpty() ? NULL : &_boundingBox);
    serializer->writeObject("boundingSphere", _boundingSphere.isEmpty() ? NULL : &_boundingSphere);
}

void Mesh::deserialize(Serializer* serializer)
{
    serializer->readString("url", _url, "");
    _primitiveType = static_cast<Mesh::PrimitiveType>(serializer->readEnum("primitiveType", "gameplay::Mesh::PrimitiveType", Mesh::TRIANGLES));
    serializer->readObject("vertexFormat", &_vertexFormat);
    _vertexCount = serializer->readInt("vertexCount", 0);
    unsigned char* vertexData = new unsigned char[_vertexFormat.getVertexSize() * _vertexCount];
    serializer->readByteArray("vertexData", &vertexData);
    _dynamic = serializer->readBool("dynamic", true);
    
    GLuint vbo;
    GL_ASSERT(glGenBuffers(1, &vbo));
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, _vertexFormat.getVertexSize() * _vertexCount, NULL, _dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
    
    _vertexBuffer = vbo;
    setVertexData((void*)vertexData, 0, _vertexCount);
    SAFE_DELETE_ARRAY(vertexData);
    
    _partCount = serializer->readObjectList("parts");
    _parts = new MeshPart*[_partCount];
    for (unsigned int i = 0; i < _partCount; i++)
    {
        _parts[i] = dynamic_cast<MeshPart*>(serializer->readObject(NULL));
        _parts[i]->_mesh = this;
        _parts[i]->_meshIndex = i;
    }
    serializer->readObject("boundingBox", &_boundingBox);
    serializer->readObject("boundingSphere", &_boundingSphere);
}

Serializable* Mesh::createInstance()
{
    return static_cast<Serializable*>(new Mesh());
}

const char* Mesh::enumToString(const char* enumName, int value)
{
    if (std::strcmp("gameplay::Mesh::PrimitiveType", enumName) == 0)
    {
        switch (value)
        {
            case Mesh::TRIANGLES:
                return "TRIANGLES";
            case Mesh::TRIANGLE_STRIP:
                return "TRIANGLE_STRIP";
            case Mesh::LINES:
                return "LINES";
            case Mesh::LINE_STRIP:
                return "LINE_STRIP";
            case Mesh::POINTS:
                return "POINTS";
            default:
                return NULL;
        }
    }
    else if (std::strcmp("gameplay::Mesh::IndexFormat", enumName) == 0)
    {
        switch (value)
        {
            case Mesh::INDEX8:
                return "INDEX8";
            case Mesh::INDEX16:
                return "INDEX16";
            case Mesh::INDEX32:
                return "INDEX32";
            default:
                return NULL;
        }
    }
    return NULL;
}

int Mesh::enumParse(const char* enumName, const char* str)
{
    if (std::strcmp("gameplay::Mesh::PrimitiveType", enumName) == 0)
    {
        if (std::strcmp("TRIANGLES", str) == 0)
            return Mesh::TRIANGLES;
        else if (std::strcmp("TRIANGLE_STRIP", str) == 0)
            return Mesh::TRIANGLE_STRIP;
        else if (std::strcmp("LINES", str) == 0)
            return Mesh::LINES;
        else if (std::strcmp("LINE_STRIP", str) == 0)
            return Mesh::LINE_STRIP;
        else if (std::strcmp("POINTS", str) == 0)
            return Mesh::POINTS;
    }
    else if (std::strcmp("gameplay::Mesh::IndexFormat", enumName) == 0)
    {
        if (std::strcmp("INDEX8", str) == 0)
            return Mesh::INDEX8;
        else if (std::strcmp("INDEX16", str) == 0)
            return Mesh::INDEX16;
        else if (std::strcmp("INDEX32", str) == 0)
            return Mesh::INDEX32;
    }
    return -1;
}

}
