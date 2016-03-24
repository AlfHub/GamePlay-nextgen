#ifndef SERIALIZERJSON_H_
#define SERIALIZERJSON_H_

#include "Serializer.h"
#include "Ref.h"
#include "json/libjson.h"

namespace gameplay
{

/**
 * Defines a json serializer.
 *
 * @see Serializer
 */
class SerializerJson : public Serializer
{
    friend class Serializer;
    
public:

    /**
     * Creates a json serializer.
     *
     * @param path The path to open create and open the serializer from.
     * @return The new json serializer.
     */
    static Serializer* createWriter(const char* path);

    /**
     * @see Serializer::close
     */
    void close();
    
    /**
     * @see Serializer::getFormat
     */
    Format getFormat() const;

    /**
     * @see Serializer::writeEnum
     */
    void writeEnum(const char* propertyName, const char* enumName, int value, int defaultValue);

    /**
     * @see Serializer::writeBool
     */
    void writeBool(const char* propertyName, bool value, bool defaultValue);

    /**
     * @see Serializer::writeInt
     */
    void writeInt(const char* propertyName, int value, int defaultValue);

    /**
     * @see Serializer::writeFloat
     */
    void writeFloat(const char* propertyName, float value, float defaultValue);

    /**
     * @see Serializer::writeVector
     */
    void writeVector(const char* propertyName, const Vector2& value, const Vector2& defaultValue);
    
    /**
     * @see Serializer::writeVector
     */
    void writeVector(const char* propertyName, const Vector3& value, const Vector3& defaultValue);
    
    /**
     * @see Serializer::writeVector
     */
    void writeVector(const char* propertyName, const Vector4& value, const Vector4& defaultValue);

    /**
     * @see Serializer::writeColor
     */
    void writeColor(const char* propertyName, const Vector3& value, const Vector3& defaultValue);

    /**
     * @see Serializer::writeColor
     */
    void writeColor(const char* propertyName, const Vector4& value, const Vector4& defaultValue);

    /**
     * @see Serializer::writeMatrix
     */
    void writeMatrix(const char* propertyName, const Matrix& value, const Matrix& defaultValue);

    /**
     * @see Serializer::writeString
     */
    void writeString(const char* propertyName, const char* value, const char* defaultValue);

    /**
     * @see Serializer::writeStringList
     */
    void writeStringList(const char* propertyName, unsigned int count);

    /**
     * @see Serializer::writeObject
     */
    void writeObject(const char* propertyName, Serializable* value);

    /**
     * @see Serializer::writeObjectList
     */
    void writeObjectList(const char* propertyName, unsigned int count);

    /**
     * @see Serializer::writeIntArray
     */
    void writeIntArray(const char* propertyName, const int* data, unsigned int count);

    /**
     * @see Serializer::writeFloatArray
     */
    void writeFloatArray(const char* propertyName, const float* data, unsigned int count);
    
    /**
     * @see Serializer::writeByteArray
     */
    void writeByteArray(const char* propertyName, const unsigned char* data, unsigned int count);
    
    /**
     * @see Serializer::readEnum
     */
    int readEnum(const char* propertyName, const char* enumName, int defaultValue);

    /**
     * @see Serializer::readBool
     */
    bool readBool(const char* propertyName, bool defaultValue);

    /**
     * @see Serializer::readInt
     */
    int readInt(const char* propertyName, int defaultValue);

    /**
     * @see Serializer::readFloat
     */
    float readFloat(const char* propertyName, float defaultValue);

    /**
     * @see Serializer::readVector
     */
    Vector2 readVector(const char* propertyName, const Vector2& defaultValue);
    
    /**
     * @see Serializer::readVector
     */
    Vector3 readVector(const char* propertyName, const Vector3& defaultValue);
    
    /**
     * @see Serializer::readVector
     */
    Vector4 readVector(const char* propertyName, const Vector4& defaultValue);
    
    /**
     * @see Serializer::readColor
     */
    Vector3 readColor(const char* propertyName, const Vector3& defaultValue);
    
    /**
     * @see Serializer::readColor
     */
    Vector4 readColor(const char* propertyName, const Vector4& defaultValue);

    /**
     * @see Serializer::readMatrix
     */
    Matrix readMatrix(const char* propertyName, const Matrix& defaultValue);

    /**
     * @see Serializer::readString
     */
    void readString(const char* propertyName, std::string& value, const char* defaultValue);

    /**
     * @see Serializer::readStringList
     */
    unsigned int readStringList(const char* propertyName);

    /**
     * @see Serializer::readObject
     */
    Serializable* readObject(const char* propertyName, Serializable* dst);

    /**
     * @see Serializer::readObjectList
     */
    unsigned int readObjectList(const char* propertyName);

    /**
     * @see Serializer::readIntArray
     */
    unsigned int readIntArray(const char* propertyName, int** data);

    /**
     * @see Serializer::readFloatArray
     */
    unsigned int readFloatArray(const char* propertyName, float** data);
    
    /**
     * @see Serializer::readByteArray
     */
    unsigned int readByteArray(const char* propertyName, unsigned char** data);

    /**
     * Destructor
     */
    virtual ~SerializerJson();
    
protected:
    
    /**
     * Constructor.
     */
    SerializerJson(const char* path, Stream* stream, unsigned int versionMajor, unsigned int versionMinor);
    
    static SerializerJson* create(const char* path, Stream* stream);
    
private:
    
    JSONNODE* createNode(JSONNODE* parent, const char* propertyName, Serializable* object, bool moreProperties);
    
    void finishNode(JSONNODE* parent);
    
    std::string _document;
    JSONNODE* _root;
    std::stack<JSONNODE*> _nodes;
    std::stack<unsigned int> _nodesListCounts;
    std::map<unsigned long, JSONNODE*> _xrefsWrite;
    std::map<unsigned long, Ref*> _xrefsRead;
};

}

#endif
