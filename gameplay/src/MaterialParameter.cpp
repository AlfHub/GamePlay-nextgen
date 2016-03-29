#include "Base.h"
#include "MaterialParameter.h"
#include "Node.h"

namespace gameplay
{

MaterialParameter::MaterialParameter() :
    _type(MaterialParameter::NONE), _count(1), _dynamic(false), _name(""),
    _uniform(NULL), _loggerDirtyBits(0)
{
    clearValue();
}

MaterialParameter::MaterialParameter(const char* name) :
    _type(MaterialParameter::NONE), _count(1), _dynamic(false), _name(name ? name : ""),
    _uniform(NULL), _loggerDirtyBits(0)
{
    clearValue();
}

MaterialParameter::~MaterialParameter()
{
    clearValue();
}

MaterialParameter::Type MaterialParameter::getType() const
{
    return _type;
}

void MaterialParameter::clearValue()
{
    // Release parameters
    switch (_type)
    {
    case MaterialParameter::SAMPLER:
        if (_value.samplerValue)
            const_cast<Texture::Sampler*>(_value.samplerValue)->release();
        break;
    case MaterialParameter::SAMPLER_ARRAY:
        if (_value.samplerArrayValue)
        {
            for (unsigned int i = 0; i < _count; ++i)
            {
                const_cast<Texture::Sampler*>(_value.samplerArrayValue[i])->release();
            }
        }
        break;
    default:
        // Ignore all other cases.
        break;
    }

    // Free dynamic data
    if (_dynamic)
    {
        switch (_type)
        {
        case MaterialParameter::FLOAT:
        case MaterialParameter::FLOAT_ARRAY:
        case MaterialParameter::VECTOR2:
        case MaterialParameter::VECTOR3:
        case MaterialParameter::VECTOR4:
        case MaterialParameter::MATRIX:
            SAFE_DELETE_ARRAY(_value.floatPtrValue);
            break;
        case MaterialParameter::INT:
        case MaterialParameter::INT_ARRAY:
            SAFE_DELETE_ARRAY(_value.intPtrValue);
            break;
        case MaterialParameter::METHOD:
            SAFE_RELEASE(_value.method);
            break;
        case MaterialParameter::SAMPLER_ARRAY:
            SAFE_DELETE_ARRAY(_value.samplerArrayValue);
            break;
        default:
            // Ignore all other cases.
            break;
        }

        _dynamic = false;
        _count = 1;
    }

    memset(&_value, 0, sizeof(_value));
    _type = MaterialParameter::NONE;
}

const char* MaterialParameter::getName() const
{
    return _name.c_str();
}

Texture::Sampler* MaterialParameter::getSampler(unsigned int index) const
{
    if (_type == MaterialParameter::SAMPLER)
        return const_cast<Texture::Sampler*>(_value.samplerValue);
    if (_type == MaterialParameter::SAMPLER_ARRAY && index < _count)
        return const_cast<Texture::Sampler*>(_value.samplerArrayValue[index]);
    return NULL;
}

void MaterialParameter::setValue(float value)
{
    clearValue();

    _value.floatValue = value;
    _type = MaterialParameter::FLOAT;
}

void MaterialParameter::setValue(int value)
{
    clearValue();

    _value.intValue = value;
    _type = MaterialParameter::INT;
}

void MaterialParameter::setValue(const float* values, unsigned int count)
{
    clearValue();

    _value.floatPtrValue = const_cast<float*> (values);
    _count = count;
    _type = MaterialParameter::FLOAT_ARRAY;
}

void MaterialParameter::setValue(const int* values, unsigned int count)
{
    clearValue();

    _value.intPtrValue = const_cast<int*> (values);
    _count = count;
    _type = MaterialParameter::INT_ARRAY;
}

void MaterialParameter::setValue(const Vector2& value)
{
    clearValue();

    // Copy data by-value into a dynamic array.
    float* array = new float[2];
    memcpy(array, &value.x, sizeof(float) * 2);

    _value.floatPtrValue = array;
    _dynamic = true;
    _count = 1;
    _type = MaterialParameter::VECTOR2;
}

void MaterialParameter::setValue(const Vector2* values, unsigned int count)
{
    GP_ASSERT(values);
    clearValue();

    _value.floatPtrValue = const_cast<float*> (&values[0].x);
    _count = count;
    _type = MaterialParameter::VECTOR2;
}

void MaterialParameter::setValue(const Vector3& value)
{
    clearValue();

    // Copy data by-value into a dynamic array.
    float* array = new float[3];
    memcpy(array, &value.x, sizeof(float) * 3);

    _value.floatPtrValue = array;
    _dynamic = true;
    _count = 1;
    _type = MaterialParameter::VECTOR3;
}

void MaterialParameter::setValue(const Vector3* values, unsigned int count)
{
    GP_ASSERT(values);
    clearValue();

    _value.floatPtrValue = const_cast<float*> (&values[0].x);
    _count = count;
    _type = MaterialParameter::VECTOR3;
}

void MaterialParameter::setValue(const Vector4& value)
{
    clearValue();

    // Copy data by-value into a dynamic array.
    float* array = new float[4];
    memcpy(array, &value.x, sizeof(float) * 4);

    _value.floatPtrValue = array;
    _dynamic = true;
    _count = 1;
    _type = MaterialParameter::VECTOR4;
}

void MaterialParameter::setValue(const Vector4* values, unsigned int count)
{
    GP_ASSERT(values);
    clearValue();

    _value.floatPtrValue = const_cast<float*> (&values[0].x);
    _count = count;
    _type = MaterialParameter::VECTOR4;
}

void MaterialParameter::setValue(const Matrix& value)
{
    // If this parameter is already storing a single dynamic matrix, no need to clear it.
    if (!(_dynamic && _count == 1 && _type == MaterialParameter::MATRIX && _value.floatPtrValue != NULL))
    {
        clearValue();

        // Allocate a new dynamic matrix.
        _value.floatPtrValue = new float[16];
    }

    memcpy(_value.floatPtrValue, value.m, sizeof(float) * 16);

    _dynamic = true;
    _count = 1;
    _type = MaterialParameter::MATRIX;
}

void MaterialParameter::setValue(const Matrix* values, unsigned int count)
{
    GP_ASSERT(values);
    clearValue();

    _value.floatPtrValue = const_cast<Matrix&> (values[0]).m;
    _count = count;
    _type = MaterialParameter::MATRIX;
}

void MaterialParameter::setValue(const Texture::Sampler* sampler)
{
    GP_ASSERT(sampler);
    clearValue();

    const_cast<Texture::Sampler*>(sampler)->addRef();
    _value.samplerValue = const_cast<Texture::Sampler*>(sampler);
    _type = MaterialParameter::SAMPLER;
}

void MaterialParameter::setValue(const Texture::Sampler** samplers, unsigned int count)
{
    GP_ASSERT(samplers);
    clearValue();

    for (unsigned int i = 0; i < count; ++i)
    {
        const_cast<Texture::Sampler*>(samplers[i])->addRef();
    }
    _value.samplerArrayValue = const_cast<Texture::Sampler**>(samplers);
    _count = count;
    _type = MaterialParameter::SAMPLER_ARRAY;
}

Texture::Sampler* MaterialParameter::setValue(const char* texturePath, bool generateMipmaps)
{
    GP_ASSERT(texturePath);
    clearValue();

    Texture::Sampler* sampler = Texture::Sampler::create(texturePath, generateMipmaps);
    if (sampler)
    {
        _value.samplerValue = sampler;
        _type = MaterialParameter::SAMPLER;
    }
    return sampler;
}

void MaterialParameter::setFloat(float value)
{
    setValue(value);
}

void MaterialParameter::setFloatArray(const float* values, unsigned int count, bool copy)
{
    GP_ASSERT(values);
    clearValue();

    if (copy)
    {
        _value.floatPtrValue = new float[count];
        memcpy(_value.floatPtrValue, values, sizeof(float) * count);
        _dynamic = true;
    }
    else
    {
        _value.floatPtrValue = const_cast<float*> (values);
    }

    _count = count;
    _type = MaterialParameter::FLOAT_ARRAY;
}

void MaterialParameter::setInt(int value)
{
    setValue(value);
}

void MaterialParameter::setIntArray(const int* values, unsigned int count, bool copy)
{
    GP_ASSERT(values);
    clearValue();

    if (copy)
    {
        _value.intPtrValue = new int[count];
        memcpy(_value.intPtrValue, values, sizeof(int) * count);
        _dynamic = true;
    }
    else
    {
        _value.intPtrValue = const_cast<int*> (values);
    }

    _count = count;
    _type = MaterialParameter::INT_ARRAY;
}

void MaterialParameter::setVector(const Vector2& value)
{
    setValue(value);
}

void MaterialParameter::setVectorArray(const Vector2* values, unsigned int count, bool copy)
{
    GP_ASSERT(values);
    clearValue();

    if (copy)
    {
        _value.floatPtrValue = new float[2 * count];
        memcpy(_value.floatPtrValue, const_cast<float*> (&values[0].x), sizeof(float) * 2 * count);
        _dynamic = true;
    }
    else
    {
        _value.floatPtrValue = const_cast<float*> (&values[0].x);
    }

    _count = count;
    _type = MaterialParameter::VECTOR2;
}

void MaterialParameter::setVector(const Vector3& value)
{
    setValue(value);
}

void MaterialParameter::setVectorArray(const Vector3* values, unsigned int count, bool copy)
{
    GP_ASSERT(values);
    clearValue();

    if (copy)
    {
        _value.floatPtrValue = new float[3 * count];
        memcpy(_value.floatPtrValue, const_cast<float*> (&values[0].x), sizeof(float) * 3 * count);
        _dynamic = true;
    }
    else
    {
        _value.floatPtrValue = const_cast<float*> (&values[0].x);
    }

    _count = count;
    _type = MaterialParameter::VECTOR3;
}

void MaterialParameter::setVector(const Vector4& value)
{
    setValue(value);
}

void MaterialParameter::setVectorArray(const Vector4* values, unsigned int count, bool copy)
{
    GP_ASSERT(values);
    clearValue();

    if (copy)
    {
        _value.floatPtrValue = new float[4 * count];
        memcpy(_value.floatPtrValue, const_cast<float*> (&values[0].x), sizeof(float) * 4 * count);
        _dynamic = true;
    }
    else
    {
        _value.floatPtrValue = const_cast<float*> (&values[0].x);
    }

    _count = count;
    _type = MaterialParameter::VECTOR4;
}

void MaterialParameter::setMatrix(const Matrix& value)
{
    setValue(value);
}

void MaterialParameter::setMatrixArray(const Matrix* values, unsigned int count, bool copy)
{
    GP_ASSERT(values);
    clearValue();

    if (copy)
    {
        _value.floatPtrValue = new float[16 * count];
        memcpy(_value.floatPtrValue, const_cast<Matrix&> (values[0]).m, sizeof(float) * 16 * count);
        _dynamic = true;
    }
    else
    {
        _value.floatPtrValue = const_cast<Matrix&> (values[0]).m;
    }

    _count = count;
    _type = MaterialParameter::MATRIX;
}

Texture::Sampler* MaterialParameter::setSampler(const char* texturePath, bool generateMipmaps)
{
    return setValue(texturePath, generateMipmaps);
}

void MaterialParameter::setSampler(const Texture::Sampler* value)
{
    setValue(value);
}

void MaterialParameter::setSamplerArray(const Texture::Sampler** values, unsigned int count, bool copy)
{
    GP_ASSERT(values);
    clearValue();

    if (copy)
    {
        _value.samplerArrayValue = new Texture::Sampler*[count];
        memcpy(_value.samplerArrayValue, values, sizeof(Texture::Sampler*) * count);
        _dynamic = true;
    }
    else
    {
        _value.samplerArrayValue = const_cast<Texture::Sampler**>(values);
    }

    for (unsigned int i = 0; i < count; ++i)
    {
        const_cast<Texture::Sampler*>(_value.samplerArrayValue[i])->addRef();
    }

    _count = count;
    _type = MaterialParameter::SAMPLER_ARRAY;
}

void MaterialParameter::bind(Effect* effect)
{
    GP_ASSERT(effect);

    // If we had a Uniform cached that is not from the passed in effect,
    // we need to update our uniform to point to the new effect's uniform.
    if (!_uniform || _uniform->getEffect() != effect)
    {
        _uniform = effect->getUniform(_name.c_str());

        if (!_uniform)
        {
            if ((_loggerDirtyBits & UNIFORM_NOT_FOUND) == 0)
            {
                // This parameter was not found in the specified effect, so do nothing.
                GP_WARN("Material parameter for uniform '%s' not found in effect: '%s'.", _name.c_str(), effect->getId());
                _loggerDirtyBits |= UNIFORM_NOT_FOUND;
            }
            return;
        }
    }

    switch (_type)
    {
    case MaterialParameter::FLOAT:
        effect->setValue(_uniform, _value.floatValue);
        break;
    case MaterialParameter::FLOAT_ARRAY:
        effect->setValue(_uniform, _value.floatPtrValue, _count);
        break;
    case MaterialParameter::INT:
        effect->setValue(_uniform, _value.intValue);
        break;
    case MaterialParameter::INT_ARRAY:
        effect->setValue(_uniform, _value.intPtrValue, _count);
        break;
    case MaterialParameter::VECTOR2:
        effect->setValue(_uniform, reinterpret_cast<Vector2*>(_value.floatPtrValue), _count);
        break;
    case MaterialParameter::VECTOR3:
        effect->setValue(_uniform, reinterpret_cast<Vector3*>(_value.floatPtrValue), _count);
        break;
    case MaterialParameter::VECTOR4:
        effect->setValue(_uniform, reinterpret_cast<Vector4*>(_value.floatPtrValue), _count);
        break;
    case MaterialParameter::MATRIX:
        effect->setValue(_uniform, reinterpret_cast<Matrix*>(_value.floatPtrValue), _count);
        break;
    case MaterialParameter::SAMPLER:
        effect->setValue(_uniform, _value.samplerValue);
        break;
    case MaterialParameter::SAMPLER_ARRAY:
        effect->setValue(_uniform, (const Texture::Sampler**)_value.samplerArrayValue, _count);
        break;
    case MaterialParameter::METHOD:
        if (_value.method)
            _value.method->setValue(effect);
        break;
    default:
        {
             if ((_loggerDirtyBits & PARAMETER_VALUE_NOT_SET) == 0)
             {
                 GP_WARN("Material parameter value not set for: '%s' in effect: '%s'.", _name.c_str(), effect->getId());
                 _loggerDirtyBits |= PARAMETER_VALUE_NOT_SET;
             }
            break;
        }
    }
}

void MaterialParameter::bindValue(Node* node, const char* binding)
{
    GP_ASSERT(binding);

    if (strcmp(binding, "&Node::getBackVector") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getBackVector);
    }
    else if (strcmp(binding, "&Node::getDownVector") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getDownVector);
    }
    else if (strcmp(binding, "&Node::getTranslationWorld") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getTranslationWorld);
    }
    else if (strcmp(binding, "&Node::getTranslationView") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getTranslationView);
    }
    else if (strcmp(binding, "&Node::getForwardVector") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getForwardVector);
    }
    else if (strcmp(binding, "&Node::getForwardVectorWorld") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getForwardVectorWorld);
    }
    else if (strcmp(binding, "&Node::getForwardVectorView") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getForwardVectorView);
    }
    else if (strcmp(binding, "&Node::getLeftVector") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getLeftVector);
    }
    else if (strcmp(binding, "&Node::getRightVector") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getRightVector);
    }
    else if (strcmp(binding, "&Node::getRightVectorWorld") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getRightVectorWorld);
    }
    else if (strcmp(binding, "&Node::getUpVector") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getUpVector);
    }
    else if (strcmp(binding, "&Node::getUpVectorWorld") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getUpVectorWorld);
    }
    else if (strcmp(binding, "&Node::getActiveCameraTranslationWorld") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getActiveCameraTranslationWorld);
    }
    else if (strcmp(binding, "&Node::getActiveCameraTranslationView") == 0)
    {
        bindValue<Node, Vector3>(node, &Node::getActiveCameraTranslationView);
    }
    else if (strcmp(binding, "&Node::getScaleX") == 0)
    {
        bindValue<Node, float>(node, &Node::getScaleX);
    }
    else if (strcmp(binding, "&Node::getScaleY") == 0)
    {
        bindValue<Node, float>(node, &Node::getScaleY);
    }
    else if (strcmp(binding, "&Node::getScaleZ") == 0)
    {
        bindValue<Node, float>(node, &Node::getScaleZ);
    }
    else if (strcmp(binding, "&Node::getTranslationX") == 0)
    {
        bindValue<Node, float>(node, &Node::getTranslationX);
    }
    else if (strcmp(binding, "&Node::getTranslationY") == 0)
    {
        bindValue<Node, float>(node, &Node::getTranslationY);
    }
    else if (strcmp(binding, "&Node::getTranslationZ") == 0)
    {
        bindValue<Node, float>(node, &Node::getTranslationZ);
    }
    else
    {
        GP_WARN("Unsupported material parameter binding '%s'.", binding);
    }
}

unsigned int MaterialParameter::getAnimationPropertyComponentCount(int propertyId) const
{
    switch (propertyId)
    {
        case ANIMATE_UNIFORM:
        {
            switch (_type)
            {
                // These types don't support animation.
                case NONE:
                case MATRIX:
                case SAMPLER:
                case SAMPLER_ARRAY:
                case METHOD:
                    return 0;
                case FLOAT:
                case FLOAT_ARRAY:
                case INT:
                case INT_ARRAY:
                    return _count;
                case VECTOR2:
                    return 2 * _count;
                case VECTOR3:
                    return 3 * _count;
                case VECTOR4:
                    return 4 * _count;
                default:
                    return 0;
            }
        }
        break;
    }

    return 0;
}

void MaterialParameter::getAnimationPropertyValue(int propertyId, AnimationValue* value)
{
    GP_ASSERT(value);
    switch (propertyId)
    {
        case ANIMATE_UNIFORM:
        {
            switch (_type)
            {
                case FLOAT:
                    value->setFloat(0, _value.floatValue);
                    break;
                case FLOAT_ARRAY:
                    GP_ASSERT(_value.floatPtrValue);
                    for (unsigned int i = 0; i < _count; i++)
                    {
                        value->setFloat(i, _value.floatPtrValue[i]);
                    }
                    break;
                case INT:
                    value->setFloat(0, _value.intValue);
                    break;
                case INT_ARRAY:
                    GP_ASSERT(_value.intPtrValue);
                    for (unsigned int i = 0; i < _count; i++)
                    {
                        value->setFloat(i, _value.intPtrValue[i]);
                    }
                    break;
                case VECTOR2:
                    value->setFloats(0, _value.floatPtrValue, _count * 2);
                    break;
                case VECTOR3:
                    value->setFloats(0, _value.floatPtrValue, _count * 3);
                    break;
                case VECTOR4:
                    value->setFloats(0, _value.floatPtrValue, _count * 4);
                    break;
                case NONE:
                case MATRIX:
                case METHOD:
                case SAMPLER:
                case SAMPLER_ARRAY:
                    // Unsupported material parameter types for animation.
                    break;
                default:
                    break;
            }
        }
        break;
    }
}

void MaterialParameter::setAnimationPropertyValue(int propertyId, AnimationValue* value, float blendWeight)
{
    GP_ASSERT(value);
    GP_ASSERT(blendWeight >= 0.0f && blendWeight <= 1.0f);

    switch (propertyId)
    {
        case ANIMATE_UNIFORM:
        {
            switch (_type)
            {
                case FLOAT:
                    _value.floatValue = Curve::lerp(blendWeight, _value.floatValue, value->getFloat(0));
                    break;
                case FLOAT_ARRAY:
                    applyAnimationValue(value, blendWeight, 1);
                    break;
                case INT:
                    _value.intValue = Curve::lerp(blendWeight, _value.intValue, value->getFloat(0));
                    break;
                case INT_ARRAY:
                    GP_ASSERT(_value.intPtrValue);
                    for (unsigned int i = 0; i < _count; i++)
                        _value.intPtrValue[i] = Curve::lerp(blendWeight, _value.intPtrValue[i], value->getFloat(i));
                    break;
                case VECTOR2:
                    applyAnimationValue(value, blendWeight, 2);
                    break;
                case VECTOR3:
                    applyAnimationValue(value, blendWeight, 3);
                    break;
                case VECTOR4:
                    applyAnimationValue(value, blendWeight, 4);
                    break;
                case NONE:
                case MATRIX:
                case METHOD:
                case SAMPLER:
                case SAMPLER_ARRAY:
                    // Unsupported material parameter types for animation.
                    break;
                default:
                    break;
            }
        }
        break;
    }
}

void MaterialParameter::applyAnimationValue(AnimationValue* value, float blendWeight, int components)
{
    GP_ASSERT(value);
    GP_ASSERT(_value.floatPtrValue);

    unsigned int count = _count * components;
    for (unsigned int i = 0; i < count; i++)
        _value.floatPtrValue[i] = Curve::lerp(blendWeight, _value.floatPtrValue[i], value->getFloat(i));
}

const char* MaterialParameter::getSerializedClassName() const
{
    return "gameplay::MaterialParameter";
}

void MaterialParameter::serialize(Serializer* serializer)
{
    serializer->writeString("name", _name.c_str(), "");
    serializer->writeEnum("type", "gameplay::MaterialParameter::Type", _type, NONE);
    switch (_type)
    {
        case FLOAT:
            serializer->writeFloat("value", _value.floatValue, 0.0f);
            break;
        case FLOAT_ARRAY:
            serializer->writeFloatArray("value", _value.floatPtrValue, _count);
            break;
        case INT:
            serializer->writeInt("value", _value.floatValue, 0.0f);
            break;
        case INT_ARRAY:
            serializer->writeIntArray("value", _value.intPtrValue, _count);
            break;
        case VECTOR2:
            serializer->writeFloatArray("value", _value.floatPtrValue, _count * 2);
            break;
        case VECTOR3:
            serializer->writeFloatArray("value", _value.floatPtrValue, _count * 3);
            break;
        case VECTOR4:
            serializer->writeFloatArray("value", _value.floatPtrValue, _count * 4);
            break;
        case MATRIX:
            serializer->writeFloatArray("value", _value.floatPtrValue, _count * 16);
            break;
        case SAMPLER:
            serializer->writeObject("value", _value.samplerValue);
            break;
        case SAMPLER_ARRAY:
            serializer->writeObjectList("value", _count);
            for (unsigned int i = 0; i < _count; i++)
                serializer->writeObject(NULL, _value.samplerArrayValue[i]);
            break;
        case METHOD:
            break;
        default:
            break;
            
    }
}

void MaterialParameter::deserialize(Serializer* serializer)
{
    serializer->readString("name", _name, "");
    _type = static_cast<MaterialParameter::Type>(serializer->readEnum("type", "gameplay::MaterialParameter::Type", NONE));
    switch (_type)
    {
        case FLOAT:
            _value.floatValue = serializer->readFloat("value", 0.0f);
            _count = 1;
            break;
        case FLOAT_ARRAY:
            _count = serializer->readFloatArray("value", &_value.floatPtrValue);
            break;
        case INT:
            _value.intValue = serializer->readInt("value", 0.0f);
            _count = 1;
            break;
        case INT_ARRAY:
            _count = serializer->readIntArray("value", &_value.intPtrValue);
            break;
        case VECTOR2:
            _count = serializer->readFloatArray("value", &_value.floatPtrValue) / 2;
            break;
        case VECTOR3:
            _count = serializer->readFloatArray("value", &_value.floatPtrValue) / 3;
            break;
        case VECTOR4:
            _count = serializer->readFloatArray("value", &_value.floatPtrValue) / 4;
            break;
        case MATRIX:
            _count = serializer->readFloatArray("value", &_value.floatPtrValue) / 16;
            break;
        case SAMPLER:
            _value.samplerValue = dynamic_cast<Texture::Sampler*>(serializer->readObject("value"));
            _count = 1;
            break;
        case SAMPLER_ARRAY:
            _count = serializer->readObjectList("value");
            for (unsigned int i = 0; i < _count; i++)
                _value.samplerArrayValue[i] = dynamic_cast<Texture::Sampler*>(serializer->readObject(NULL));
            break;
        case METHOD:
            //std::string parameterName = parameter->getName();
            //std::string autoBinding = _autoBindings[parameterName];
            //serializer->writeString("autoBinding", autoBinding.c_str(), "");
            break;
        default:
            break;
            
    }
}

Serializable* MaterialParameter::createInstance()
{
    return static_cast<Serializable*>(new MaterialParameter());
}

const char* MaterialParameter::enumToString(const char* enumName, int value)
{
    if (std::strcmp("gameplay::MaterialParameter::Type", enumName) == 0)
    {
        switch (value)
        {
            case MaterialParameter::VECTOR2:
                return "VECTOR2";
            case MaterialParameter::VECTOR3:
                return "VECTOR3";
            case MaterialParameter::VECTOR4:
                return "VECTOR4";
            case MaterialParameter::MATRIX:
                return "MATRIX";
            case MaterialParameter::SAMPLER:
                return "SAMPLER";
            case MaterialParameter::SAMPLER_ARRAY:
                return "SAMPLER_ARRAY";
            case MaterialParameter::INT:
                return "INT";
            case MaterialParameter::INT_ARRAY:
                return "INT_ARRAY";
            case MaterialParameter::FLOAT:
                return "FLOAT";
            case MaterialParameter::FLOAT_ARRAY:
                return "FLOAT_ARRAY";
            case MaterialParameter::METHOD:
                return "METHOD";
            default:
                return NULL;
        }
    }
    return NULL;
}

int MaterialParameter::enumParse(const char* enumName, const char* str)
{
    if (std::strcmp("gameplay::MaterialParameter::Type", enumName) == 0)
    {
        if (std::strcmp("VECTOR2", str) == 0)
            return MaterialParameter::VECTOR2;
        else if (std::strcmp("VECTOR3", str) == 0)
            return MaterialParameter::VECTOR3;
        else if (std::strcmp("VECTOR4", str) == 0)
            return MaterialParameter::VECTOR4;
        else if (std::strcmp("MATRIX", str) == 0)
            return MaterialParameter::MATRIX;
        else if (std::strcmp("SAMPLER", str) == 0)
            return MaterialParameter::SAMPLER;
        else if (std::strcmp("SAMPLER_ARRAY", str) == 0)
            return MaterialParameter::SAMPLER_ARRAY;
        else if (std::strcmp("INT", str) == 0)
            return MaterialParameter::INT;
        else if (std::strcmp("INT_ARRAY", str) == 0)
            return MaterialParameter::INT_ARRAY;
        else if (std::strcmp("FLOAT", str) == 0)
            return MaterialParameter::FLOAT;
        else if (std::strcmp("FLOAT_ARRAY", str) == 0)
            return MaterialParameter::FLOAT_ARRAY;
        else if (std::strcmp("METHOD", str) == 0)
            return MaterialParameter::METHOD;
    }
    return -1;
}

void MaterialParameter::cloneInto(MaterialParameter* materialParameter) const
{
    GP_ASSERT(materialParameter);
    materialParameter->_type = _type;
    materialParameter->_count = _count;
    materialParameter->_dynamic = _dynamic;
    materialParameter->_uniform = _uniform;
    switch (_type)
    {
    case NONE:
        break;
    case FLOAT:
        materialParameter->setValue(_value.floatValue);
        break;
    case FLOAT_ARRAY:
        materialParameter->setValue(_value.floatPtrValue, _count);
        break;
    case INT:
        materialParameter->setValue(_value.intValue);
        break;
    case INT_ARRAY:
        materialParameter->setValue(_value.intPtrValue, _count);
        break;
    case VECTOR2:
    {
        Vector2* value = reinterpret_cast<Vector2*>(_value.floatPtrValue);
        if (_count == 1)
        {
            GP_ASSERT(value);
            materialParameter->setValue(*value);
        }
        else
        {
            materialParameter->setValue(value, _count);
        }
        break;
    }   
    case VECTOR3:
    {
        Vector3* value = reinterpret_cast<Vector3*>(_value.floatPtrValue);
        if (_count == 1)
        {
            GP_ASSERT(value);
            materialParameter->setValue(*value);
        }
        else
        {
            materialParameter->setValue(value, _count);
        }
        break;
    }
    case VECTOR4:
    {
        Vector4* value = reinterpret_cast<Vector4*>(_value.floatPtrValue);
        if (_count == 1)
        {
            GP_ASSERT(value);
            materialParameter->setValue(*value);
        }
        else
        {
            materialParameter->setValue(value, _count);
        }
        break;
    }
    case MATRIX:
    {
        Matrix* value = reinterpret_cast<Matrix*>(_value.floatPtrValue);
        if (_count == 1)
        {
            GP_ASSERT(value);
            materialParameter->setValue(*value);
        }
        else
        {
            materialParameter->setValue(value, _count);
        }
        break;
    }
    case SAMPLER:
        materialParameter->setValue(_value.samplerValue);
        break;
    case SAMPLER_ARRAY:
        materialParameter->setValue((const Texture::Sampler**)_value.samplerArrayValue, _count);
        break;
    case METHOD:
        materialParameter->_value.method = _value.method;
        GP_ASSERT(materialParameter->_value.method);
        materialParameter->_value.method->addRef();
        break;
    default:
        GP_ERROR("Unsupported material parameter type(%d).", _type);
        break;
    }

    NodeCloneContext context;
    this->AnimationTarget::cloneInto(materialParameter, context);
}

MaterialParameter::MethodBinding::MethodBinding(MaterialParameter* param) :
    _parameter(param), _autoBinding(false)
{
}

}
