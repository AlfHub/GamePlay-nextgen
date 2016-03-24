#include "Base.h"
#include "SerializerJson.h"
#include "Serializable.h"
#include "Serializer.h"
#include "FileSystem.h"
#include "Vector2.h"
#include "Vector3.h"

namespace gameplay
{

SerializerJson::SerializerJson(const char* path, Stream* stream, unsigned int versionMajor, unsigned int versionMinor)
    : Serializer(path, stream, versionMajor, versionMinor), _root(NULL)
{
}

SerializerJson::~SerializerJson()
{
}
        
SerializerJson* SerializerJson::create(const char* path, Stream* stream)
{
    unsigned int length = stream->length();
    char* buffer = new char[length + 1];
    stream->read(buffer, sizeof(char), length);
    buffer[length] = '\0';
    JSONNODE* root = json_parse(buffer);
    if (root == NULL)
        return NULL;

    SerializerJson* serializer = NULL;

    JSONNODE* versionNode = json_get(root, "version");
    int versionMajor = SERIALIZER_VERSION[0];
    int versionMinor = SERIALIZER_VERSION[1];
    if (versionNode)
    {
        json_char* str = json_as_string(versionNode);
        std::string version = std::string(str);
        json_free(str);
        if (version.length() > 0)
        {
            std::string major = version.substr(0, 1);
            versionMajor = std::stoi(major);
        }
        if (version.length() > 2)
        {
            std::string minor = version.substr(2, 1);
            versionMinor = std::stoi(minor);
        }
        serializer = new SerializerJson(path, stream, versionMajor, versionMinor);
        serializer->_document = buffer;
        serializer->_root = root;
        serializer->_nodes.push(root);
    }
    SAFE_DELETE_ARRAY(buffer);
    return serializer;
}

Serializer* SerializerJson::createWriter(const char* path)
{
    Stream* stream = FileSystem::open(path, FileSystem::WRITE);
    if (stream == NULL)
        return NULL;

    SerializerJson* serializer = new SerializerJson(path, stream, SERIALIZER_VERSION[0], SERIALIZER_VERSION[1]);
    serializer->_type = Serializer::WRITER;
    serializer->_root = json_new(JSON_NODE);
    std::string version;
    version.append(std::to_string(SERIALIZER_VERSION[0]));
    version.append(".");
    version.append(std::to_string(SERIALIZER_VERSION[1]));
    json_push_back(serializer->_root, json_new_a("version", version.c_str()));
    serializer->_nodes.push(serializer->_root);
    serializer->_path = path;
    
    return serializer;
}

void SerializerJson::close()
{
    if (_stream)
    {
        if (_type == Serializer::WRITER)
        {
            json_char* buffer = json_write_formatted(_root);
            std::string str = buffer;
            _stream->write(str.c_str(), sizeof(char), str.length());
            json_free(buffer);
        }
        if (_root)
            json_delete(_root);
        _stream->close();
    }
}

Serializer::Format SerializerJson::getFormat() const
{
    return Serializer::JSON;
}

void SerializerJson::writeEnum(const char* propertyName, const char* enumName, int value, int defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(enumName);
    
    if (value == defaultValue)
        return;
    
    const char* str = Serializer::getActivator()->enumToString(enumName, value);
    if (str)
        writeString(propertyName, str, "");
}
    
void SerializerJson::writeBool(const char* propertyName, bool value, bool defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);
    
    if (value == defaultValue)
        return;
    
    JSONNODE* node = _nodes.top();
    json_push_back(node, json_new_b(propertyName, value));
}

void SerializerJson::writeInt(const char* propertyName, int value, int defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);
    
    if (value == defaultValue)
        return;
    
    JSONNODE* node = _nodes.top();
    json_push_back(node, json_new_i(propertyName, value));
}

void SerializerJson::writeFloat(const char* propertyName, float value, float defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);
    
    if (value == defaultValue)
        return;

    JSONNODE* node = _nodes.top();
    json_push_back(node, json_new_f(propertyName, value));
}

void SerializerJson::writeVector(const char* propertyName, const Vector2& value, const Vector2& defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);
    
    if (value == defaultValue)
        return;
    
    // "properyName" : [ x, y ]
    JSONNODE* node = _nodes.top();
    JSONNODE* array = json_new(JSON_ARRAY);
    json_set_name(array, propertyName);
    json_push_back(array, json_new_f(NULL, value.x));
    json_push_back(array, json_new_f(NULL, value.y));
    json_push_back(node, array);
}

void SerializerJson::writeVector(const char* propertyName, const Vector3& value, const Vector3& defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);

    if (value == defaultValue)
        return;

    // "properyName" : [ x, y, z ]
    JSONNODE* node = _nodes.top();
    JSONNODE* array = json_new(JSON_ARRAY);
    json_set_name(array, propertyName);
    json_push_back(array, json_new_f(NULL, value.x));
    json_push_back(array, json_new_f(NULL, value.y));
    json_push_back(array, json_new_f(NULL, value.z));
    json_push_back(node, array);
}

void SerializerJson::writeVector(const char* propertyName, const Vector4& value, const Vector4& defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);
    
    if (value == defaultValue)
        return;
    
    // "properyName" : [ x, y, z, w ]
    JSONNODE* node = _nodes.top();
    JSONNODE* array = json_new(JSON_ARRAY);
    json_set_name(array, propertyName);
    json_push_back(array, json_new_f(NULL, value.x));
    json_push_back(array, json_new_f(NULL, value.y));
    json_push_back(array, json_new_f(NULL, value.z));
    json_push_back(array, json_new_f(NULL, value.w));
    json_push_back(node, array);
}

void SerializerJson::writeColor(const char* propertyName, const Vector3& value, const Vector3& defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);
    
    if (value == defaultValue)
        return;
    
    // "property" : "#rrggbb"
   JSONNODE* node = _nodes.top();
   char buffer[9];
   sprintf(buffer, "%02x%02x%02x", (int)(value.x * 255.0f), (int)(value.y * 255.0f), (int)(value.z * 255.0f));
   std::ostringstream s;
   s << "#" << buffer;
    json_push_back(node, json_new_a(propertyName, s.str().c_str()));
}
    
void SerializerJson::writeColor(const char* propertyName, const Vector4& value, const Vector4& defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);

    if (value == defaultValue)
        return;
    
    // "property" : "#rrggbbaa"
    JSONNODE* node = _nodes.top();
    std::ostringstream s;
    s << "#" << std::hex << value.toColor();    
    json_push_back(node, json_new_a(propertyName, s.str().c_str()));
}
    
void SerializerJson::writeMatrix(const char* propertyName, const Matrix& value, const Matrix& defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);

    if (value == defaultValue)
        return;

    // "properyName" : [ m0, ... , m15 ]
    JSONNODE* node = _nodes.top();
    JSONNODE* array = json_new(JSON_ARRAY);
    json_set_name(array, propertyName);
    json_push_back(array, json_new_f(NULL, value.m[0]));
    json_push_back(array, json_new_f(NULL, value.m[1]));
    json_push_back(array, json_new_f(NULL, value.m[2]));
    json_push_back(array, json_new_f(NULL, value.m[3]));
    json_push_back(array, json_new_f(NULL, value.m[4]));
    json_push_back(array, json_new_f(NULL, value.m[5]));
    json_push_back(array, json_new_f(NULL, value.m[6]));
    json_push_back(array, json_new_f(NULL, value.m[7]));
    json_push_back(array, json_new_f(NULL, value.m[8]));
    json_push_back(array, json_new_f(NULL, value.m[9]));
    json_push_back(array, json_new_f(NULL, value.m[10]));
    json_push_back(array, json_new_f(NULL, value.m[11]));
    json_push_back(array, json_new_f(NULL, value.m[12]));
    json_push_back(array, json_new_f(NULL, value.m[13]));
    json_push_back(array, json_new_f(NULL, value.m[14]));
    json_push_back(array, json_new_f(NULL, value.m[15]));
    json_push_back(node, array);
}

JSONNODE* SerializerJson::createNode(JSONNODE* parent, const char* propertyName, Serializable* object, bool moreProperties)
{
    if (json_type(parent) == JSON_NODE && propertyName || json_type(parent) == JSON_ARRAY)
    {
        JSONNODE* node = json_new(JSON_NODE);
        json_push_back(parent, node);
        
        if (json_type(parent) == JSON_NODE && propertyName)
            json_set_name(node, propertyName);
        
        if (moreProperties)
            _nodes.push(node);
        
        return node;
    }
    else
    {
        return parent;
    }
}

void SerializerJson::writeString(const char* propertyName, const char* value, const char* defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);

    if ((value == defaultValue) || (value && defaultValue && strcmp (value, defaultValue) == 0))
        return;

    JSONNODE* node = _nodes.top();
    json_push_back(node, json_new_a(propertyName, value));
}

void SerializerJson::writeStringList(const char* propertyName, unsigned int count)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);
    if (count == 0)
        return;

    JSONNODE* node = _nodes.top();
    JSONNODE* list = json_new(JSON_ARRAY);
    json_push_back(node, list);
    json_set_name(list, propertyName);

    _nodes.push(list);
    _nodesListCounts.push(count);
}

void SerializerJson::writeObject(const char* propertyName, Serializable* object)
{
    GP_ASSERT(_type == Serializer::WRITER);
    
    if (object == NULL)
        return;

    JSONNODE* parentNode = _nodes.top();
    JSONNODE* writeNode = NULL;
    JSONNODE* xrefNode = NULL;
    Ref* xref = dynamic_cast<Ref*>(object);
    if (xref && xref->getRefCount() > 1)
    {
        std::map<unsigned long, JSONNODE*>::const_iterator itr = _xrefsWrite.find(reinterpret_cast<unsigned long>(xref));
        std::string url;
        if (itr == _xrefsWrite.end())
        {
            writeNode = createNode(parentNode, propertyName, object, true);
            url = std::to_string(reinterpret_cast<unsigned long>(object));
            _xrefsWrite[reinterpret_cast<unsigned long>(xref)] = writeNode;
        }
        else
        {
            writeNode = createNode(parentNode, propertyName, object, false);
            std::ostringstream o;
            o << "@" << std::to_string(reinterpret_cast<unsigned long>(object));
            url = o.str();
            xrefNode = itr->second;
        }
        json_push_back(writeNode, json_new_a("class", object->getSerializedClassName()));
        json_push_back(writeNode, json_new_a("xref", url.c_str()));
    }
    else
    {
        writeNode = createNode(parentNode, propertyName, object, true);
        json_push_back(writeNode, json_new_a("class", object->getSerializedClassName()));
    }
    
    if (xrefNode == NULL)
        object->serialize(this);
    
    if (!xrefNode)
        _nodes.pop();
    
    if (json_type(parentNode) == JSON_ARRAY)
    {
        if (json_size(parentNode) == _nodesListCounts.top())
        {
            _nodes.pop();
            _nodesListCounts.pop();
        }
    }
}

void SerializerJson::writeObjectList(const char* propertyName, unsigned int count)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);
    if (count == 0)
        return;
    
    JSONNODE* node = _nodes.top();
    JSONNODE* list = json_new(JSON_ARRAY);
    json_push_back(node, list);
    json_set_name(list, propertyName);
    
    _nodes.push(list);
    _nodesListCounts.push(count);
}

void SerializerJson::writeIntArray(const char* propertyName, const int* data, unsigned int count)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);

    if (!data || count == 0)
        return;

    // "properyName" : [ 0, ... , count - 1 ]
    JSONNODE* node = _nodes.top();
    JSONNODE* array = json_new(JSON_ARRAY);
    json_set_name(array, propertyName);
    for ( unsigned int i = 0; i < count; i++)
    {
        json_push_back(array, json_new_f(NULL, data[i]));
    }
    json_push_back(node, array);
}

void SerializerJson::writeFloatArray(const char* propertyName, const float* data, unsigned int count)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);
    
    if (!data || count == 0)
        return;
    
    // "properyName" : [ 0.0, ... , count - 1 ]
    JSONNODE* node = _nodes.top();
    JSONNODE* array = json_new(JSON_ARRAY);
    json_set_name(array, propertyName);
    for ( unsigned int i = 0; i < count; i++)
    {
        json_push_back(array, json_new_f(NULL, data[i]));
    }
    json_push_back(node, array);
}

void SerializerJson::writeByteArray(const char* propertyName, const unsigned char* data, unsigned int count)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::WRITER);
    
    if (!data || count == 0)
        return;
    
    // "properyName" : "base64_encode(data)"
    JSONNODE* node = _nodes.top();
    json_push_back(node, json_new_a(propertyName, json_encode64((const void*)data, sizeof(unsigned char) * count)));
}

int SerializerJson::readEnum(const char* propertyName, const char* enumName, int defaultValue)
{
    GP_ASSERT(enumName);
    
    std::string str;
    readString(propertyName, str, "");
    
    return Serializer::getActivator()->enumParse(enumName, str.c_str());
}

bool SerializerJson::readBool(const char* propertyName, bool defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);
    
    JSONNODE* node = _nodes.top();
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_BOOL)
            GP_ERROR("Invalid json bool for propertyName:%s", propertyName);
        return json_as_bool(property);
    }
    
    return defaultValue;
}

int SerializerJson::readInt(const char* propertyName, int defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);

    JSONNODE* node = _nodes.top();
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_NUMBER)
            GP_ERROR("Invalid json number for propertyName:%s", propertyName);
        
        return json_as_int(property);
    }
    return defaultValue;
}

float SerializerJson::readFloat(const char* propertyName, float defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);

    JSONNODE* node = _nodes.top();
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_NUMBER)
            GP_ERROR("Invalid json number for propertyName:%s", propertyName);
        
        return json_as_float(property);
    }
    return defaultValue;
}

Vector2 SerializerJson::readVector(const char* propertyName, const Vector2& defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);
    
    JSONNODE* node = _nodes.top();
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_ARRAY || json_size(property) < 2)
            GP_ERROR("Invalid json array from Vector2 for propertyName:%s", propertyName);
        
        Vector2 value;
        JSONNODE* x = json_at(property, 0);
        value.x  = json_as_float(x);
        JSONNODE* y = json_at(property, 1);
        value.y  = json_as_float(y);
        return value;
    }
    return defaultValue;
}

Vector3 SerializerJson::readVector(const char* propertyName, const Vector3& defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);

    JSONNODE* node = _nodes.top();
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_ARRAY || json_size(property) < 3)
            GP_ERROR("Invalid json array from Vector3 for propertyName:%s", propertyName);
        
        Vector3 value;
        JSONNODE* x = json_at(property, 0);
        value.x  = json_as_float(x);
        JSONNODE* y = json_at(property, 1);
        value.y  = json_as_float(y);
        JSONNODE* z = json_at(property, 2);
        value.z  = json_as_float(z);
        return value;
    }
    return defaultValue;
}

Vector4 SerializerJson::readVector(const char* propertyName, const Vector4& defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);

    JSONNODE* node = _nodes.top();
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_ARRAY || json_size(property) < 4)
            GP_ERROR("Invalid json array from Vector4 for propertyName:%s", propertyName);
        
        Vector4 value;
        JSONNODE* x = json_at(property, 0);
        value.x  = json_as_float(x);
        JSONNODE* y = json_at(property, 1);
        value.y  = json_as_float(y);
        JSONNODE* z = json_at(property, 2);
        value.z  = json_as_float(z);
        JSONNODE* w = json_at(property, 3);
        value.w  = json_as_float(w);
        return value;
    }
    return defaultValue;
}

Vector3 SerializerJson::readColor(const char* propertyName, const Vector3& defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);
    
    JSONNODE* node = _nodes.top();
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_STRING)
            GP_ERROR("Invalid json string from color for propertyName:%s", propertyName);
        json_char* str = json_as_string(property);
        Vector3 value = Vector3::fromColorString(str);
        json_free(str);
        return value;
    }
    return defaultValue;
}

Vector4 SerializerJson::readColor(const char* propertyName, const Vector4& defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);
    
    JSONNODE* node = _nodes.top();
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_STRING)
            GP_ERROR("Invalid json string from color for propertyName:%s", propertyName);
        json_char* str = json_as_string(property);
        Vector4 value = Vector4::fromColorString(str);
        json_free(str);
        return value;
    }
    return defaultValue;
}

Matrix SerializerJson::readMatrix(const char* propertyName, const Matrix& defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);

    JSONNODE* node = _nodes.top();
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_ARRAY || json_size(property) < 16)
            GP_ERROR("Invalid json array from Matrix for propertyName:%s", propertyName);
        
        Matrix value;
        JSONNODE* m0 = json_at(property, 0);
        value.m[0]  = json_as_float(m0);
        JSONNODE* m1 = json_at(property, 1);
        value.m[1]  = json_as_float(m1);
        JSONNODE* m2 = json_at(property, 2);
        value.m[2]  = json_as_float(m2);
        JSONNODE* m3 = json_at(property, 3);
        value.m[3]  = json_as_float(m3);
        JSONNODE* m4 = json_at(property, 4);
        value.m[4]  = json_as_float(m4);
        JSONNODE* m5 = json_at(property, 5);
        value.m[5]  = json_as_float(m5);
        JSONNODE* m6 = json_at(property, 6);
        value.m[6]  = json_as_float(m6);
        JSONNODE* m7 = json_at(property, 7);
        value.m[7]  = json_as_float(m7);
        JSONNODE* m8 = json_at(property, 8);
        value.m[8]  = json_as_float(m8);
        JSONNODE* m9 = json_at(property, 9);
        value.m[9]  = json_as_float(m9);
        JSONNODE* m10 = json_at(property, 10);
        value.m[10]  = json_as_float(m10);
        JSONNODE* m11 = json_at(property, 11);
        value.m[11]  = json_as_float(m11);
        JSONNODE* m12 = json_at(property, 12);
        value.m[12]  = json_as_float(m12);
        JSONNODE* m13 = json_at(property, 13);
        value.m[13]  = json_as_float(m13);
        JSONNODE* m14 = json_at(property, 14);
        value.m[14]  = json_as_float(m14);
        JSONNODE* m15 = json_at(property, 15);
        value.m[15]  = json_as_float(m15);
        
        return value;
    }
    return defaultValue;
}

void SerializerJson::readString(const char* propertyName, std::string& value, const char* defaultValue)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);

    JSONNODE* node = _nodes.top();
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_STRING)
            GP_ERROR("Invalid json string for propertyName:%s", propertyName);

        json_char* str = json_as_string(property);
        value.clear();
        value.resize(strlen(str));
        std::strcpy(&value[0], str);
        json_free(str);
    }
    else
    {
        value.clear();
        value.resize(strlen(defaultValue));
        std::strcpy(&value[0], defaultValue);
    }
}

Serializable* SerializerJson::readObject(const char* propertyName, Serializable* dst)
{
    GP_ASSERT(_type == Serializer::READER);
    
    JSONNODE* parentNode = _nodes.top();
    JSONNODE* readNode = NULL;
    
    if (json_type(parentNode) == JSON_ARRAY)
    {
        unsigned int  arraySize = json_size(parentNode);
        readNode = json_at(parentNode, arraySize - _nodesListCounts.top());
        _nodes.push(readNode);
    }
    else if (json_type(parentNode) == JSON_NODE && propertyName)
    {
        JSONNODE* propertyNode = json_get(parentNode, propertyName);
        if (propertyNode == NULL)
            return NULL;
        if (json_type(propertyNode) != JSON_NODE)
        {
            GP_WARN("Invalid json object for propertyName:%s", propertyName);
            return NULL;
        }
        _nodes.push(propertyNode);
        readNode = propertyNode;
    }
    else
    {
        readNode = parentNode;
    }
    
    JSONNODE* classProperty = json_get(readNode, "class");
    char* className = json_as_string(classProperty);
    
    // Look for xref's
    unsigned long xrefAddress = 0L;
    JSONNODE* xrefProperty = json_get(readNode, "xref");
    if (xrefProperty)
    {
        json_char* str = json_as_string(xrefProperty);
        std::string url = str;
        json_free(str);
        std::string at = "@";
        if (url.compare(0, at.length(), at) != 0)
        {
            // no @ sign. This is xref'ed by others
            xrefAddress = std::strtol(url.c_str(), NULL, 10);
        }
        else
        {
            // This needs to lookup the node from the xref address. So save it for lookup
            std::string addressStr = url.substr(1, url.length());
            xrefAddress = std::strtol(addressStr.c_str(), NULL, 10);
            
            std::map<unsigned long, Ref*>::const_iterator itr = _xrefsRead.find(xrefAddress);
            if (itr != _xrefsRead.end())
            {
                Ref* ref = itr->second;
                
                finishNode(parentNode);

                return dynamic_cast<Serializable*>(ref);
            }
            else
            {
                GP_WARN("Unresolved xref:%u for class:%s", xrefAddress, className);
                json_free(className);
                return NULL;
            }
        }
    }
    
    Serializable* value = NULL;
    if (dst)
    {
        value = dst;
    }
    else
    {
        value = Serializer::getActivator()->createInstance(className);
        if (value == NULL)
        {
            GP_WARN("Failed to deserialize json object:%s for class:", className);
            json_free(className);
            return NULL;
        }
        json_free(className);
    }

    value->deserialize(this);
    
    if (xrefAddress)
    {
        _xrefsRead[xrefAddress] = dynamic_cast<Ref*>(value);
    }

    finishNode(parentNode);
    
    return value;
}

void SerializerJson::finishNode(JSONNODE* parent)
{
    _nodes.pop();
    
    if (json_type(parent) == JSON_ARRAY)
    {
        _nodesListCounts.top() -= 1;
        if (_nodesListCounts.top() == 0)
        {
            _nodes.pop();
            _nodesListCounts.pop();
        }
    }
}

unsigned int SerializerJson::readStringList(const char* propertyName)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);

    JSONNODE* node = _nodes.top();
    JSONNODE* list = json_get(node, propertyName);
    unsigned int count = json_size(list);
    if (count > 0)
    {
        _nodes.push(list);
        _nodesListCounts.push(count);
    }
    return count;
}

unsigned int SerializerJson::readObjectList(const char* propertyName)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);
    
    JSONNODE* node = _nodes.top();
    JSONNODE* list = json_get(node, propertyName);
    unsigned int count = json_size(list);
    if (count > 0)
    {
        _nodes.push(list);
        _nodesListCounts.push(count);
    }
    return count;
}

unsigned int SerializerJson::readIntArray(const char* propertyName, int** data)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);
    
    JSONNODE* node = _nodes.top();
    unsigned int count = 0;
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_ARRAY )
            GP_ERROR("Invalid json array for propertyName:%s", propertyName);
        
        count = json_size(property);
        int* buffer = NULL;
        if (*data == NULL)
        {
            buffer = new int[count];
        }
        else
        {
            buffer = *data;
        }
        
        for (unsigned int i = 0; i < count; i++)
        {
            JSONNODE* item = json_at(property, i);
            buffer[i] = json_as_int(item);
        }
        *data = buffer;
    }
    return count;
}

unsigned int SerializerJson::readFloatArray(const char* propertyName, float** data)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);
    
    JSONNODE* node = _nodes.top();
    unsigned int count = 0;
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_ARRAY )
            GP_ERROR("Invalid json array for propertyName:%s", propertyName);
        
        count = json_size(property);
        float* buffer = NULL;
        if (*data == NULL)
        {
            buffer = new float[count];
        }
        else
        {
            buffer = *data;
        }
        
        for (unsigned int i = 0; i < count; i++)
        {
            JSONNODE* item = json_at(property, i);
            buffer[i] = json_as_float(item);
        }
        *data = buffer;
    }
    return count;
}
    
unsigned int SerializerJson::readByteArray(const char* propertyName, unsigned char** data)
{
    GP_ASSERT(propertyName);
    GP_ASSERT(_type == Serializer::READER);
    
    unsigned long size = 0L;
    JSONNODE* node = _nodes.top();
    JSONNODE* property = json_get(node, propertyName);
    if (property)
    {
        if (json_type(property) != JSON_STRING)
            GP_ERROR("Invalid json base64 string for propertyName:%s", propertyName);
        
        json_char* str = json_as_string(property);
        void* decoded = json_decode64(str, &size);
        
        unsigned char* buffer = NULL;
        if (*data == NULL)
        {
            *data = (unsigned char*)decoded;
        }
        else
        {
            memcpy(*data, decoded, size);
            json_free(decoded);
        }
        json_free(str);
    }
    return (unsigned int)size;
}

}
