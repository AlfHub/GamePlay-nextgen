#ifndef SERIALIZABLE_H_
#define SERIALIZABLE_H_

#include "Serializer.h"

namespace gameplay
{
    
/**
 * Defines a object that can be read(deserialized) and written(serailized) to file.
 *
 * To ensure compatibility for encoding. Object properties must be serialized in the same
 * order there are deserialized.
 */
class Serializable
{
public:

    /**
     * Destructor
     */
    virtual ~Serializable() { };
    
    /**
     * Gets serialized class name string represenation for the object.
     *
     * This is used by the Serializer when reading/writing objects for rtti.
     * Type names should be namespaced. Ex "gameplay::Node"
     */
    virtual const char* getSerializedClassName() const = 0;

    /**
     * Serializes an object's properties.
     *
     * Note: Only call Serializer::writeXxxxx methods in impl. or errors will occur.
     *
     * @param serializer The serializer to write properties to.
     */
    virtual void serialize(Serializer* serializer) = 0;

    /**
     * Deserializes an object properties using a serializer.
     *
     * Note: Only call Serializer::readXxxxx methods in impl. or errors will occur.
     *
     * @param serializer The serializer to read properties from.
     */
    virtual void deserialize(Serializer* serializer) = 0;

};

}

#endif