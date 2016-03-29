#include "Base.h"
#include "RenderState.h"
#include "Node.h"
#include "Pass.h"
#include "Technique.h"
#include "Node.h"
#include "Scene.h"

// Render state override bits
#define RS_BLEND 1
#define RS_BLEND_FUNC 2
#define RS_CULL_FACE 4
#define RS_DEPTH_TEST 8
#define RS_DEPTH_WRITE 16
#define RS_DEPTH_FUNC 32
#define RS_CULL_FACE_SIDE 64
#define RS_STENCIL_TEST 128
#define RS_STENCIL_WRITE 256
#define RS_STENCIL_FUNC 512
#define RS_STENCIL_OP 1024
#define RS_FRONT_FACE 2048

#define RS_ALL_ONES 0xFFFFFFFF

namespace gameplay
{

RenderState::StateBlock* RenderState::StateBlock::_defaultState = NULL;
std::vector<RenderState::AutoBindingResolver*> RenderState::_customAutoBindingResolvers;

RenderState::RenderState()
    : _nodeBinding(NULL), _state(NULL), _parent(NULL)
{
}

RenderState::~RenderState()
{
    SAFE_RELEASE(_state);

    // Destroy all the material parameters
    for (size_t i = 0, count = _parameters.size(); i < count; ++i)
    {
        SAFE_RELEASE(_parameters[i]);
    }
}

void RenderState::initialize()
{
    if (StateBlock::_defaultState == NULL)
    {
        StateBlock::_defaultState = StateBlock::create();
    }
}

void RenderState::finalize()
{
    SAFE_RELEASE(StateBlock::_defaultState);
}

MaterialParameter* RenderState::getParameter(const char* name) const
{
    GP_ASSERT(name);

    // Search for an existing parameter with this name.
    MaterialParameter* param;
    for (size_t i = 0, count = _parameters.size(); i < count; ++i)
    {
        param = _parameters[i];
        GP_ASSERT(param);
        if (strcmp(param->getName(), name) == 0)
        {
            return param;
        }
    }

    // Create a new parameter and store it in our list.
    param = new MaterialParameter(name);
    _parameters.push_back(param);

    return param;
}

unsigned int RenderState::getParameterCount() const
{
    return _parameters.size();
}

MaterialParameter* RenderState::getParameterByIndex(unsigned int index)
{
    return _parameters[index];
}

void RenderState::addParameter(MaterialParameter* param)
{
    _parameters.push_back(param);
    param->addRef();
}

void RenderState::removeParameter(const char* name)
{
    for (size_t i = 0, count = _parameters.size(); i < count; ++i)
    {
        MaterialParameter* p = _parameters[i];
        if (p->_name == name)
        {
            _parameters.erase(_parameters.begin() + i);
            SAFE_RELEASE(p);
            break;
        }
    }
}

/**
 * @script{ignore}
 */
const char* autoBindingToString(RenderState::AutoBinding autoBinding)
{
    // NOTE: As new AutoBinding values are added, this switch statement must be updatd.
    switch (autoBinding)
    {
    case RenderState::NONE:
        return NULL;
    case RenderState::VIEW_MATRIX:
        return "VIEW_MATRIX";
    case RenderState::PROJECTION_MATRIX:
        return "PROJECTION_MATRIX";
    case RenderState::WORLD_VIEW_MATRIX:
        return "WORLD_VIEW_MATRIX";
    case RenderState::VIEW_PROJECTION_MATRIX:
        return "VIEW_PROJECTION_MATRIX";
    case RenderState::WORLD_VIEW_PROJECTION_MATRIX:
        return "WORLD_VIEW_PROJECTION_MATRIX";
    case RenderState::INVERSE_TRANSPOSE_WORLD_MATRIX:
        return "INVERSE_TRANSPOSE_WORLD_MATRIX";
    case RenderState::INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX:
        return "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX";
    case RenderState::CAMERA_WORLD_POSITION:
        return "CAMERA_WORLD_POSITION";
    case RenderState::CAMERA_VIEW_POSITION:
        return "CAMERA_VIEW_POSITION";
    case RenderState::MATRIX_PALETTE:
        return "MATRIX_PALETTE";
    case RenderState::SCENE_AMBIENT_COLOR:
        return "SCENE_AMBIENT_COLOR";
    default:
        return "";
    }
}

void RenderState::setParameterAutoBinding(const char* name, AutoBinding autoBinding)
{
    setParameterAutoBinding(name, autoBindingToString(autoBinding));
}

void RenderState::setParameterAutoBinding(const char* name, const char* autoBinding)
{
    GP_ASSERT(name);
    GP_ASSERT(autoBinding);

    if (autoBinding == NULL)
    {
        // Remove an existing auto-binding
        std::map<std::string, std::string>::iterator itr = _autoBindings.find(name);
        if (itr != _autoBindings.end())
            _autoBindings.erase(itr);
    }
    else
    {
        // Add/update an auto-binding
        _autoBindings[name] = autoBinding;
    }

    // If we already have a node binding set, pass it to our handler now
    if (_nodeBinding)
    {
        applyAutoBinding(name, autoBinding);
    }
}

void RenderState::setStateBlock(StateBlock* state)
{
    if (_state != state)
    {
        SAFE_RELEASE(_state);

        _state = state;

        if (_state)
        {
            _state->addRef();
        }
    }
}

RenderState::StateBlock* RenderState::getStateBlock() const
{
    if (_state == NULL)
    {
        _state = StateBlock::create();
    }

    return _state;
}

void RenderState::setNodeBinding(Node* node)
{
    if (_nodeBinding != node)
    {
        _nodeBinding = node;

        if (_nodeBinding)
        {
            // Apply all existing auto-bindings using this node.
            std::map<std::string, std::string>::const_iterator itr = _autoBindings.begin();
            while (itr != _autoBindings.end())
            {
                applyAutoBinding(itr->first.c_str(), itr->second.c_str());
                ++itr;
            }
        }
    }
}

void RenderState::applyAutoBinding(const char* uniformName, const char* autoBinding)
{
    GP_ASSERT(_nodeBinding);

    MaterialParameter* param = getParameter(uniformName);
    GP_ASSERT(param);

    bool bound = false;

    // First attempt to resolve the binding using custom registered resolvers.
    for (size_t i = 0, count = _customAutoBindingResolvers.size(); i < count; ++i)
    {
        if (_customAutoBindingResolvers[i]->resolveAutoBinding(autoBinding, _nodeBinding, param))
        {
            // Handled by custom auto binding resolver
            bound = true;
            break;
        }
    }
    // Perform built-in resolution
    if (!bound)
    {
        bound = true;

        if (strcmp(autoBinding, "WORLD_MATRIX") == 0)
        {
            param->bindValue(this, &RenderState::autoBindingGetWorldMatrix);
        }
        else if (strcmp(autoBinding, "VIEW_MATRIX") == 0)
        {
            param->bindValue(this, &RenderState::autoBindingGetViewMatrix);
        }
        else if (strcmp(autoBinding, "PROJECTION_MATRIX") == 0)
        {
            param->bindValue(this, &RenderState::autoBindingGetProjectionMatrix);
        }
        else if (strcmp(autoBinding, "WORLD_VIEW_MATRIX") == 0)
        {
            param->bindValue(this, &RenderState::autoBindingGetWorldViewMatrix);
        }
        else if (strcmp(autoBinding, "VIEW_PROJECTION_MATRIX") == 0)
        {
            param->bindValue(this, &RenderState::autoBindingGetViewProjectionMatrix);
        }
        else if (strcmp(autoBinding, "WORLD_VIEW_PROJECTION_MATRIX") == 0)
        {
            param->bindValue(this, &RenderState::autoBindingGetWorldViewProjectionMatrix);
        }
        else if (strcmp(autoBinding, "INVERSE_TRANSPOSE_WORLD_MATRIX") == 0)
        {
            param->bindValue(this, &RenderState::autoBindingGetInverseTransposeWorldMatrix);
        }
        else if (strcmp(autoBinding, "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX") == 0)
        {
            param->bindValue(this, &RenderState::autoBindingGetInverseTransposeWorldViewMatrix);
        }
        else if (strcmp(autoBinding, "CAMERA_WORLD_POSITION") == 0)
        {
            param->bindValue(this, &RenderState::autoBindingGetCameraWorldPosition);
        }
        else if (strcmp(autoBinding, "CAMERA_VIEW_POSITION") == 0)
        {
            param->bindValue(this, &RenderState::autoBindingGetCameraViewPosition);
        }
        else if (strcmp(autoBinding, "MATRIX_PALETTE") == 0)
        {
            param->bindValue(this, &RenderState::autoBindingGetMatrixPalette, &RenderState::autoBindingGetMatrixPaletteSize);
        }
        else if (strcmp(autoBinding, "SCENE_AMBIENT_COLOR") == 0)
        {
            param->bindValue(this, &RenderState::autoBindingGetAmbientColor);
        }
        else
        {
            bound = false;
            GP_WARN("Unsupported auto binding type (%s).", autoBinding);
        }
    }
    if (bound)
    {
        // Mark parameter as an auto binding
        if (param->_type == MaterialParameter::METHOD && param->_value.method)
            param->_value.method->_autoBinding = true;
    }
}

const Matrix& RenderState::autoBindingGetWorldMatrix() const
{
    return _nodeBinding ? _nodeBinding->getWorldMatrix() : Matrix::identity();
}

const Matrix& RenderState::autoBindingGetViewMatrix() const
{
    return _nodeBinding ? _nodeBinding->getViewMatrix() : Matrix::identity();
}

const Matrix& RenderState::autoBindingGetProjectionMatrix() const
{
    return _nodeBinding ? _nodeBinding->getProjectionMatrix() : Matrix::identity();
}

const Matrix& RenderState::autoBindingGetWorldViewMatrix() const
{
    return _nodeBinding ? _nodeBinding->getWorldViewMatrix() : Matrix::identity();
}

const Matrix& RenderState::autoBindingGetViewProjectionMatrix() const
{
    return _nodeBinding ? _nodeBinding->getViewProjectionMatrix() : Matrix::identity();
}

const Matrix& RenderState::autoBindingGetWorldViewProjectionMatrix() const
{
    return _nodeBinding ? _nodeBinding->getWorldViewProjectionMatrix() : Matrix::identity();
}

const Matrix& RenderState::autoBindingGetInverseTransposeWorldMatrix() const
{
    return _nodeBinding ? _nodeBinding->getInverseTransposeWorldMatrix() : Matrix::identity();
}

const Matrix& RenderState::autoBindingGetInverseTransposeWorldViewMatrix() const
{
    return _nodeBinding ? _nodeBinding->getInverseTransposeWorldViewMatrix() : Matrix::identity();
}

Vector3 RenderState::autoBindingGetCameraWorldPosition() const
{
    return _nodeBinding ? _nodeBinding->getActiveCameraTranslationWorld() : Vector3::zero();
}

Vector3 RenderState::autoBindingGetCameraViewPosition() const
{
    return _nodeBinding ? _nodeBinding->getActiveCameraTranslationView() : Vector3::zero();
}

const Vector4* RenderState::autoBindingGetMatrixPalette() const
{
    Model* model = dynamic_cast<Model*>(_nodeBinding->getDrawable());
    if (model)
    {
        MeshSkin* skin = model->getSkin();
        if (skin)
            return skin->getMatrixPalette();
    }
    return NULL;
}

unsigned int RenderState::autoBindingGetMatrixPaletteSize() const
{
    Model* model = dynamic_cast<Model*>(_nodeBinding->getDrawable());
    if (model)
    {
        MeshSkin* skin = model->getSkin();
        if (skin)
            return skin->getMatrixPaletteSize();
    }
    return 0;
}

const Vector3& RenderState::autoBindingGetAmbientColor() const
{
    Scene* scene = _nodeBinding ? _nodeBinding->getScene() : NULL;
    return scene ? scene->getAmbientColor() : Vector3::zero();
}

void RenderState::bind(Pass* pass)
{
    GP_ASSERT(pass);

    // Get the combined modified state bits for our RenderState hierarchy.
    long stateOverrideBits = _state ? _state->_bits : 0;
    RenderState* rs = _parent;
    while (rs)
    {
        if (rs->_state)
        {
            stateOverrideBits |= rs->_state->_bits;
        }
        rs = rs->_parent;
    }

    // Restore renderer state to its default, except for explicitly specified states
    StateBlock::restore(stateOverrideBits);

    // Apply parameter bindings and renderer state for the entire hierarchy, top-down.
    rs = NULL;
    Effect* effect = pass->getEffect();
    while ((rs = getTopmost(rs)))
    {
        for (size_t i = 0, count = rs->_parameters.size(); i < count; ++i)
        {
            GP_ASSERT(rs->_parameters[i]);
            rs->_parameters[i]->bind(effect);
        }

        if (rs->_state)
        {
            rs->_state->bindNoRestore();
        }
    }
}

RenderState* RenderState::getTopmost(RenderState* below)
{
    RenderState* rs = this;
    if (rs == below)
    {
        // Nothing below ourself.
        return NULL;
    }

    while (rs)
    {
        if (rs->_parent == below || rs->_parent == NULL)
        {
            // Stop traversing up here.
            return rs;
        }
        rs = rs->_parent;
    }

    return NULL;
}

void RenderState::cloneInto(RenderState* renderState, NodeCloneContext& context) const
{
    GP_ASSERT(renderState);

    // Clone parameters
    for (std::map<std::string, std::string>::const_iterator it = _autoBindings.begin(); it != _autoBindings.end(); ++it)
    {
        renderState->setParameterAutoBinding(it->first.c_str(), it->second.c_str());
    }
    for (std::vector<MaterialParameter*>::const_iterator it = _parameters.begin(); it != _parameters.end(); ++it)
    {
        const MaterialParameter* param = *it;
        GP_ASSERT(param);

        // If this parameter is a method binding auto binding, don't clone it - it will get setup automatically
        // via the cloned auto bindings instead.
        if (param->_type == MaterialParameter::METHOD && param->_value.method && param->_value.method->_autoBinding)
            continue;

        MaterialParameter* paramCopy = new MaterialParameter(param->getName());
        param->cloneInto(paramCopy);

        renderState->_parameters.push_back(paramCopy);
    }

    // Clone our state block
    if (_state)
    {
        _state->cloneInto(renderState->getStateBlock());
    }

    // Notes:
    // 1. _nodeBinding should not be set here, it should be set by the caller.
    // 2. _parent should not be set here, since it's set in the constructor of Technique and Pass.
}

RenderState::StateBlock::StateBlock() :
    _blendEnabled(false), _blendSrc(RenderState::BLEND_ONE), _blendDst(RenderState::BLEND_ZERO),
    _cullFaceEnabled(false), _cullFaceSide(CULL_FACE_SIDE_BACK), _frontFace(FRONT_FACE_CCW),
    _depthTestEnabled(false), _depthWriteEnabled(true), _depthFunc(RenderState::DEPTH_LESS),
    _stencilTestEnabled(false), _stencilWrite(RS_ALL_ONES),
    _stencilFunc(RenderState::STENCIL_ALWAYS), _stencilFuncRef(0), _stencilFuncMask(RS_ALL_ONES),
    _stencilOpSfail(RenderState::STENCIL_OP_KEEP),
    _stencilOpDpfail(RenderState::STENCIL_OP_KEEP),
    _stencilOpDppass(RenderState::STENCIL_OP_KEEP),
    _bits(0L)
{
}

RenderState::StateBlock::StateBlock(const StateBlock& copy)
{
}

RenderState::StateBlock::~StateBlock()
{
}

RenderState::StateBlock* RenderState::StateBlock::create()
{
    return new RenderState::StateBlock();
}

void RenderState::StateBlock::bind()
{
    // When the public bind() is called with no RenderState object passed in,
    // we assume we are being called to bind the state of a single StateBlock,
    // irrespective of whether it belongs to a hierarchy of RenderStates.
    // Therefore, we call restore() here with only this StateBlock's override
    // bits to restore state before applying the new state.
    StateBlock::restore(_bits);

    bindNoRestore();
}

void RenderState::StateBlock::bindNoRestore()
{
    GP_ASSERT(_defaultState);
    
    // Update any state that differs from _defaultState and flip _defaultState bits
    if ((_bits & RS_BLEND) && (_blendEnabled != _defaultState->_blendEnabled))
    {
        if (_blendEnabled)
            GL_ASSERT( glEnable(GL_BLEND) );
        else
            GL_ASSERT( glDisable(GL_BLEND) );
        _defaultState->_blendEnabled = _blendEnabled;
    }
    if ((_bits & RS_BLEND_FUNC) && (_blendSrc != _defaultState->_blendSrc || _blendDst != _defaultState->_blendDst))
    {
        GL_ASSERT( glBlendFunc((GLenum)_blendSrc, (GLenum)_blendDst) );
        _defaultState->_blendSrc = _blendSrc;
        _defaultState->_blendDst = _blendDst;
    }
    if ((_bits & RS_CULL_FACE) && (_cullFaceEnabled != _defaultState->_cullFaceEnabled))
    {
        if (_cullFaceEnabled)
            GL_ASSERT( glEnable(GL_CULL_FACE) );
        else
            GL_ASSERT( glDisable(GL_CULL_FACE) );
        _defaultState->_cullFaceEnabled = _cullFaceEnabled;
    }
    if ((_bits & RS_CULL_FACE_SIDE) && (_cullFaceSide != _defaultState->_cullFaceSide))
    {
        GL_ASSERT( glCullFace((GLenum)_cullFaceSide) );
        _defaultState->_cullFaceSide = _cullFaceSide;
    }
    if ((_bits & RS_FRONT_FACE) && (_frontFace != _defaultState->_frontFace))
    {
        GL_ASSERT( glFrontFace((GLenum)_frontFace) );
        _defaultState->_frontFace = _frontFace;
    }
    if ((_bits & RS_DEPTH_TEST) && (_depthTestEnabled != _defaultState->_depthTestEnabled))
    {
        if (_depthTestEnabled)
            GL_ASSERT( glEnable(GL_DEPTH_TEST) );
        else
            GL_ASSERT( glDisable(GL_DEPTH_TEST) );
        _defaultState->_depthTestEnabled = _depthTestEnabled;
    }
    if ((_bits & RS_DEPTH_WRITE) && (_depthWriteEnabled != _defaultState->_depthWriteEnabled))
    {
        GL_ASSERT( glDepthMask(_depthWriteEnabled ? GL_TRUE : GL_FALSE) );
        _defaultState->_depthWriteEnabled = _depthWriteEnabled;
    }
    if ((_bits & RS_DEPTH_FUNC) && (_depthFunc != _defaultState->_depthFunc))
    {
        GL_ASSERT( glDepthFunc((GLenum)_depthFunc) );
        _defaultState->_depthFunc = _depthFunc;
    }
    if ((_bits & RS_STENCIL_TEST) && (_stencilTestEnabled != _defaultState->_stencilTestEnabled))
    {
        if (_stencilTestEnabled)
            GL_ASSERT( glEnable(GL_STENCIL_TEST) );
        else
            GL_ASSERT( glDisable(GL_STENCIL_TEST) );
        _defaultState->_stencilTestEnabled = _stencilTestEnabled;
    }
    if ((_bits & RS_STENCIL_WRITE) && (_stencilWrite != _defaultState->_stencilWrite))
    {
        GL_ASSERT( glStencilMask(_stencilWrite) );
        _defaultState->_stencilWrite = _stencilWrite;
    }
    if ((_bits & RS_STENCIL_FUNC) && (_stencilFunc != _defaultState->_stencilFunc ||
                                      _stencilFuncRef != _defaultState->_stencilFuncRef ||
                                      _stencilFuncMask != _defaultState->_stencilFuncMask))
    {
        GL_ASSERT( glStencilFunc((GLenum)_stencilFunc, _stencilFuncRef, _stencilFuncMask) );
        _defaultState->_stencilFunc = _stencilFunc;
        _defaultState->_stencilFuncRef = _stencilFuncRef;
        _defaultState->_stencilFuncMask = _stencilFuncMask;
    }
    if ((_bits & RS_STENCIL_OP) && (_stencilOpSfail != _defaultState->_stencilOpSfail ||
                                    _stencilOpDpfail != _defaultState->_stencilOpDpfail ||
                                    _stencilOpDppass != _defaultState->_stencilOpDppass))
    {
        GL_ASSERT( glStencilOp((GLenum)_stencilOpSfail, (GLenum)_stencilOpDpfail, (GLenum)_stencilOpDppass) );
        _defaultState->_stencilOpSfail = _stencilOpSfail;
        _defaultState->_stencilOpDpfail = _stencilOpDpfail;
        _defaultState->_stencilOpDppass = _stencilOpDppass;
    }
    _defaultState->_bits |= _bits;
}

void RenderState::StateBlock::restore(long stateOverrideBits)
{
    GP_ASSERT(_defaultState);

    // If there is no state to restore (i.e. no non-default state), do nothing.
    if (_defaultState->_bits == 0)
        return;
    
    // Restore any state that is not overridden and is not default
    if (!(stateOverrideBits & RS_BLEND) && (_defaultState->_bits & RS_BLEND))
    {
        GL_ASSERT( glDisable(GL_BLEND) );
        _defaultState->_bits &= ~RS_BLEND;
        _defaultState->_blendEnabled = false;
    }
    if (!(stateOverrideBits & RS_BLEND_FUNC) && (_defaultState->_bits & RS_BLEND_FUNC))
    {
        GL_ASSERT( glBlendFunc(GL_ONE, GL_ZERO) );
        _defaultState->_bits &= ~RS_BLEND_FUNC;
        _defaultState->_blendSrc = RenderState::BLEND_ONE;
        _defaultState->_blendDst = RenderState::BLEND_ZERO;
    }
    if (!(stateOverrideBits & RS_CULL_FACE) && (_defaultState->_bits & RS_CULL_FACE))
    {
        GL_ASSERT( glDisable(GL_CULL_FACE) );
        _defaultState->_bits &= ~RS_CULL_FACE;
        _defaultState->_cullFaceEnabled = false;
    }
    if (!(stateOverrideBits & RS_CULL_FACE_SIDE) && (_defaultState->_bits & RS_CULL_FACE_SIDE))
    {
        GL_ASSERT( glCullFace((GLenum)GL_BACK) );
        _defaultState->_bits &= ~RS_CULL_FACE_SIDE;
        _defaultState->_cullFaceSide = RenderState::CULL_FACE_SIDE_BACK;
    }
    if (!(stateOverrideBits & RS_FRONT_FACE) && (_defaultState->_bits & RS_FRONT_FACE))
    {
        GL_ASSERT( glFrontFace((GLenum)GL_CCW) );
        _defaultState->_bits &= ~RS_FRONT_FACE;
        _defaultState->_frontFace = RenderState::FRONT_FACE_CCW;
    }
    if (!(stateOverrideBits & RS_DEPTH_TEST) && (_defaultState->_bits & RS_DEPTH_TEST))
    {
        GL_ASSERT( glDisable(GL_DEPTH_TEST) );
        _defaultState->_bits &= ~RS_DEPTH_TEST;
        _defaultState->_depthTestEnabled = false;
    }
    if (!(stateOverrideBits & RS_DEPTH_WRITE) && (_defaultState->_bits & RS_DEPTH_WRITE))
    {
        GL_ASSERT( glDepthMask(GL_TRUE) );
        _defaultState->_bits &= ~RS_DEPTH_WRITE;
        _defaultState->_depthWriteEnabled = true;
    }
    if (!(stateOverrideBits & RS_DEPTH_FUNC) && (_defaultState->_bits & RS_DEPTH_FUNC))
    {
        GL_ASSERT( glDepthFunc((GLenum)GL_LESS) );
        _defaultState->_bits &= ~RS_DEPTH_FUNC;
        _defaultState->_depthFunc = RenderState::DEPTH_LESS;
    }
	if (!(stateOverrideBits & RS_STENCIL_TEST) && (_defaultState->_bits & RS_STENCIL_TEST))
    {
        GL_ASSERT( glDisable(GL_STENCIL_TEST) );
        _defaultState->_bits &= ~RS_STENCIL_TEST;
        _defaultState->_stencilTestEnabled = false;
    }
	if (!(stateOverrideBits & RS_STENCIL_WRITE) && (_defaultState->_bits & RS_STENCIL_WRITE))
    {
		GL_ASSERT( glStencilMask(RS_ALL_ONES) );
        _defaultState->_bits &= ~RS_STENCIL_WRITE;
		_defaultState->_stencilWrite = RS_ALL_ONES;
    }
	if (!(stateOverrideBits & RS_STENCIL_FUNC) && (_defaultState->_bits & RS_STENCIL_FUNC))
    {
		GL_ASSERT( glStencilFunc((GLenum)RenderState::STENCIL_ALWAYS, 0, RS_ALL_ONES) );
        _defaultState->_bits &= ~RS_STENCIL_FUNC;
        _defaultState->_stencilFunc = RenderState::STENCIL_ALWAYS;
		_defaultState->_stencilFuncRef = 0;
		_defaultState->_stencilFuncMask = RS_ALL_ONES;
    }
	if (!(stateOverrideBits & RS_STENCIL_OP) && (_defaultState->_bits & RS_STENCIL_OP))
    {
		GL_ASSERT( glStencilOp((GLenum)RenderState::STENCIL_OP_KEEP, (GLenum)RenderState::STENCIL_OP_KEEP, (GLenum)RenderState::STENCIL_OP_KEEP) );
        _defaultState->_bits &= ~RS_STENCIL_OP;
        _defaultState->_stencilOpSfail = RenderState::STENCIL_OP_KEEP;
		_defaultState->_stencilOpDpfail = RenderState::STENCIL_OP_KEEP;
		_defaultState->_stencilOpDppass = RenderState::STENCIL_OP_KEEP;
    }
}

void RenderState::StateBlock::enableDepthWrite()
{
    GP_ASSERT(_defaultState);

    // Internal method used by Game::clear() to restore depth writing before a
    // clear operation. This is necessary if the last code to draw before the
    // next frame leaves depth writing disabled.
    if (!_defaultState->_depthWriteEnabled)
    {
        GL_ASSERT( glDepthMask(GL_TRUE) );
        _defaultState->_bits &= ~RS_DEPTH_WRITE;
        _defaultState->_depthWriteEnabled = true;
    }
}

void RenderState::StateBlock::cloneInto(StateBlock* state)
{
    GP_ASSERT(state);

    state->_blendEnabled = _blendEnabled;
    state->_blendSrc = _blendSrc;
    state->_blendDst = _blendDst;
    state->_cullFaceEnabled = _cullFaceEnabled;
    state->_cullFaceSide = _cullFaceSide;
    state->_frontFace = _frontFace;
    state->_depthTestEnabled = _depthTestEnabled;
    state->_depthWriteEnabled = _depthWriteEnabled;
    state->_depthFunc = _depthFunc;
    state->_stencilTestEnabled = _stencilTestEnabled;
    state->_stencilWrite = _stencilWrite;
    state->_stencilFunc = _stencilFunc;
    state->_stencilFuncRef = _stencilFuncRef;
    state->_stencilFuncMask = _stencilFuncMask;
    state->_stencilOpSfail = _stencilOpSfail;
    state->_stencilOpDpfail = _stencilOpDpfail;
    state->_stencilOpDppass = _stencilOpDppass;
    state->_bits = _bits;
}

static bool parseBool(const char* value)
{
    GP_ASSERT(value);

    if (strlen(value) == 4)
    {
        return (
            tolower(value[0]) == 't' &&
            tolower(value[1]) == 'r' &&
            tolower(value[2]) == 'u' &&
            tolower(value[3]) == 'e' );
    }

    return false;
}

static int parseInt(const char* value)
{
	GP_ASSERT(value);

	int rValue;
    int scanned = sscanf(value, "%d", &rValue);
    if (scanned != 1)
    {
        GP_ERROR("Error attempting to parse int '%s'. (Will default to 0 if errors are treated as warnings)", value);
        return 0;
    }
    return rValue;
}

static unsigned int parseUInt(const char* value)
{
	GP_ASSERT(value);

	unsigned int rValue;
    int scanned = sscanf(value, "%u", &rValue);
    if (scanned != 1)
    {
        GP_ERROR("Error attempting to parse unsigned int '%s'. (Will default to 0 if errors are treated as warnings)", value);
        return 0;
    }
    return rValue;
}

static RenderState::BlendMode parseBlendMode(const char* value)
{
    GP_ASSERT(value);

    // Convert the string to uppercase for comparison.
    std::string upper(value);
    std::transform(upper.begin(), upper.end(), upper.begin(), (int(*)(int))toupper);
    if (upper == "ZERO")
        return RenderState::BLEND_ZERO;
    else if (upper == "ONE")
        return RenderState::BLEND_ONE;
    else if (upper == "SRC_COLOR")
        return RenderState::BLEND_SRC_COLOR;
    else if (upper == "ONE_MINUS_SRC_COLOR")
        return RenderState::BLEND_ONE_MINUS_SRC_COLOR;
    else if (upper == "DST_COLOR")
        return RenderState::BLEND_DST_COLOR;
    else if (upper == "ONE_MINUS_DST_COLOR")
        return RenderState::BLEND_ONE_MINUS_DST_COLOR;
    else if (upper == "SRC_ALPHA")
        return RenderState::BLEND_SRC_ALPHA;
    else if (upper == "ONE_MINUS_SRC_ALPHA")
        return RenderState::BLEND_ONE_MINUS_SRC_ALPHA;
    else if (upper == "DST_ALPHA")
        return RenderState::BLEND_DST_ALPHA;
    else if (upper == "ONE_MINUS_DST_ALPHA")
        return RenderState::BLEND_ONE_MINUS_DST_ALPHA;
    else if (upper == "CONSTANT_ALPHA")
        return RenderState::BLEND_CONSTANT_ALPHA;
    else if (upper == "ONE_MINUS_CONSTANT_ALPHA")
        return RenderState::BLEND_ONE_MINUS_CONSTANT_ALPHA;
    else if (upper == "SRC_ALPHA_SATURATE")
        return RenderState::BLEND_SRC_ALPHA_SATURATE;
    else
    {
        GP_ERROR("Unsupported blend value (%s). (Will default to BLEND_ONE if errors are treated as warnings)", value);
        return RenderState::BLEND_ONE;
    }
}

static RenderState::DepthFunc parseDepthFunc(const char* value)
{
    GP_ASSERT(value);

    // Convert string to uppercase for comparison
    std::string upper(value);
    std::transform(upper.begin(), upper.end(), upper.begin(), (int(*)(int))toupper);
    if (upper == "NEVER")
        return RenderState::DEPTH_NEVER;
    else if (upper == "LESS")
        return RenderState::DEPTH_LESS;
    else if (upper == "EQUAL")
        return RenderState::DEPTH_EQUAL;
    else if (upper == "LEQUAL")
        return RenderState::DEPTH_LEQUAL;
    else if (upper == "GREATER")
        return RenderState::DEPTH_GREATER;
    else if (upper == "NOTEQUAL")
        return RenderState::DEPTH_NOTEQUAL;
    else if (upper == "GEQUAL")
        return RenderState::DEPTH_GEQUAL;
    else if (upper == "ALWAYS")
        return RenderState::DEPTH_ALWAYS;
    else
    {
        GP_ERROR("Unsupported depth function value (%s). Will default to DEPTH_LESS if errors are treated as warnings)", value);
        return RenderState::DEPTH_LESS;
    }
}

static RenderState::CullFaceSide parseCullFaceSide(const char* value)
{
    GP_ASSERT(value);

    // Convert string to uppercase for comparison
    std::string upper(value);
    std::transform(upper.begin(), upper.end(), upper.begin(), (int(*)(int))toupper);
    if (upper == "BACK")
        return RenderState::CULL_FACE_SIDE_BACK;
    else if (upper == "FRONT")
        return RenderState::CULL_FACE_SIDE_FRONT;
    else if (upper == "FRONT_AND_BACK")
        return RenderState::CULL_FACE_SIDE_FRONT_AND_BACK;
    else
    {
        GP_ERROR("Unsupported cull face side value (%s). Will default to BACK if errors are treated as warnings.", value);
        return RenderState::CULL_FACE_SIDE_BACK;
    }
}

static RenderState::FrontFace parseFrontFace(const char* value)
{
    GP_ASSERT(value);

    // Convert string to uppercase for comparison
    std::string upper(value);
    std::transform(upper.begin(), upper.end(), upper.begin(), (int(*)(int))toupper);
    if (upper == "CCW")
        return RenderState::FRONT_FACE_CCW;
    else if (upper == "CW")
        return RenderState::FRONT_FACE_CW;
    else
    {
        GP_ERROR("Unsupported front face side value (%s). Will default to CCW if errors are treated as warnings.", value);
        return RenderState::FRONT_FACE_CCW;
    }
}

static RenderState::StencilFunc parseStencilFunc(const char* value)
{
    GP_ASSERT(value);

    // Convert string to uppercase for comparison
    std::string upper(value);
    std::transform(upper.begin(), upper.end(), upper.begin(), (int(*)(int))toupper);
    if (upper == "NEVER")
        return RenderState::STENCIL_NEVER;
    else if (upper == "LESS")
        return RenderState::STENCIL_LESS;
    else if (upper == "EQUAL")
        return RenderState::STENCIL_EQUAL;
    else if (upper == "LEQUAL")
        return RenderState::STENCIL_LEQUAL;
    else if (upper == "GREATER")
        return RenderState::STENCIL_GREATER;
    else if (upper == "NOTEQUAL")
        return RenderState::STENCIL_NOTEQUAL;
    else if (upper == "GEQUAL")
        return RenderState::STENCIL_GEQUAL;
    else if (upper == "ALWAYS")
        return RenderState::STENCIL_ALWAYS;
    else
    {
        GP_ERROR("Unsupported stencil function value (%s). Will default to STENCIL_ALWAYS if errors are treated as warnings)", value);
        return RenderState::STENCIL_ALWAYS;
    }
}

static RenderState::StencilOp parseStencilOp(const char* value)
{
    GP_ASSERT(value);

    // Convert string to uppercase for comparison
    std::string upper(value);
    std::transform(upper.begin(), upper.end(), upper.begin(), (int(*)(int))toupper);
    if (upper == "KEEP")
        return RenderState::STENCIL_OP_KEEP;
    else if (upper == "ZERO")
        return RenderState::STENCIL_OP_ZERO;
	else if (upper == "REPLACE")
        return RenderState::STENCIL_OP_REPLACE;
	else if (upper == "INCR")
        return RenderState::STENCIL_OP_INCR;
	else if (upper == "DECR")
        return RenderState::STENCIL_OP_DECR;
	else if (upper == "INVERT")
        return RenderState::STENCIL_OP_INVERT;
	else if (upper == "INCR_WRAP")
        return RenderState::STENCIL_OP_INCR_WRAP;
	else if (upper == "DECR_WRAP")
        return RenderState::STENCIL_OP_DECR_WRAP;
    else
    {
        GP_ERROR("Unsupported stencil operation value (%s). Will default to STENCIL_OP_KEEP if errors are treated as warnings)", value);
		return RenderState::STENCIL_OP_KEEP;
    }
}

const char* RenderState::getSerializedClassName() const
{
    return "gameplay::RenderState";
}

void RenderState::serialize(Serializer* serializer)
{
    // MaterialParameters
    serializer->writeObjectList("parameters", _parameters.size());
    for (size_t i = 0, count = _parameters.size(); i < count; ++i)
    {
        serializer->writeObject(NULL, _parameters[i]);
    }
    // RenderState::StateBlock
}

void RenderState::deserialize(Serializer* serializer)
{
}

const char* RenderState::enumToString(const char* enumName, int value)
{
    if (std::strcmp("gameplay::RenderState::AutoBinding", enumName) == 0)
    {
        switch (value)
        {
            case RenderState::NONE:
                return "NONE";
            case RenderState::WORLD_MATRIX:
                return "WORLD_MATRIX";
            case RenderState::VIEW_MATRIX:
                return "VIEW_MATRIX";
            case RenderState::PROJECTION_MATRIX:
                return "PROJECTION_MATRIX";
            case RenderState::WORLD_VIEW_MATRIX:
                return "WORLD_VIEW_MATRIX";
            case RenderState::VIEW_PROJECTION_MATRIX:
                return "VIEW_PROJECTION_MATRIX";
            case RenderState::WORLD_VIEW_PROJECTION_MATRIX:
                return "WORLD_VIEW_PROJECTION_MATRIX";
            case RenderState::INVERSE_TRANSPOSE_WORLD_MATRIX:
                return "INVERSE_TRANSPOSE_WORLD_MATRIX";
            case RenderState::INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX:
                return "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX";
            case RenderState::CAMERA_WORLD_POSITION:
                return "CAMERA_WORLD_POSITION";
            case RenderState::CAMERA_VIEW_POSITION:
                return "CAMERA_VIEW_POSITION";
            case RenderState::MATRIX_PALETTE:
                return "MATRIX_PALETTE";
            case RenderState::SCENE_AMBIENT_COLOR:
                return "SCENE_AMBIENT_COLOR";
            default:
                return NULL;
        }
    }
    else if (std::strcmp("gameplay::RenderState::BlendMode", enumName) == 0)
    {
        switch (value)
        {
            case RenderState::BLEND_ZERO:
                return "BLEND_ZERO";
            case RenderState::BLEND_ONE:
                return "BLEND_ONE";
            case RenderState::BLEND_SRC_COLOR:
                return "BLEND_SRC_COLOR";
            case RenderState::BLEND_ONE_MINUS_SRC_COLOR:
                return "BLEND_ONE_MINUS_SRC_COLOR";
            case RenderState::BLEND_DST_COLOR:
                return "BLEND_DST_COLOR";
            case RenderState::BLEND_ONE_MINUS_DST_COLOR:
                return "BLEND_ONE_MINUS_DST_COLOR";
            case RenderState::BLEND_SRC_ALPHA:
                return "BLEND_SRC_ALPHA";
            case RenderState::BLEND_ONE_MINUS_SRC_ALPHA:
                return "BLEND_ONE_MINUS_SRC_ALPHA";
            case RenderState::BLEND_DST_ALPHA:
                return "BLEND_DST_ALPHA";
            case RenderState::BLEND_ONE_MINUS_DST_ALPHA:
                return "BLEND_ONE_MINUS_DST_ALPHA";
            case RenderState::BLEND_CONSTANT_ALPHA:
                return "BLEND_CONSTANT_ALPHA";
            case RenderState::BLEND_ONE_MINUS_CONSTANT_ALPHA:
                return "BLEND_ONE_MINUS_CONSTANT_ALPHA";
            case RenderState::BLEND_SRC_ALPHA_SATURATE:
                return "BLEND_SRC_ALPHA_SATURATE";
            default:
                return NULL;
        }
    }
    else if (std::strcmp("gameplay::RenderState::DepthFunc", enumName) == 0)
    {
        switch (value)
        {
            case RenderState::DEPTH_NEVER:
                return "DEPTH_NEVER";
            case RenderState::DEPTH_LESS:
                return "DEPTH_LESS";
            case RenderState::DEPTH_EQUAL:
                return "DEPTH_EQUAL";
            case RenderState::DEPTH_LEQUAL:
                return "DEPTH_LEQUAL";
            case RenderState::DEPTH_GREATER:
                return "DEPTH_GREATER";
            case RenderState::DEPTH_NOTEQUAL:
                return "DEPTH_NOTEQUAL";
            case RenderState::DEPTH_GEQUAL:
                return "DEPTH_GEQUAL";
            case RenderState::DEPTH_ALWAYS:
                return "DEPTH_ALWAYS";
        }
    }
    else if (std::strcmp("gameplay::RenderState::CullFaceSide", enumName) == 0)
    {
        switch (value)
        {
            case RenderState::CULL_FACE_SIDE_BACK:
                return "CULL_FACE_SIDE_BACK";
            case RenderState::CULL_FACE_SIDE_FRONT:
                return "CULL_FACE_SIDE_FRONT";
            case RenderState::CULL_FACE_SIDE_FRONT_AND_BACK:
                return "CULL_FACE_SIDE_FRONT_AND_BACK";
        }
    }
    
    else if (std::strcmp("gameplay::RenderState::FrontFace", enumName) == 0)
    {
        switch (value)
        {
            case RenderState::FRONT_FACE_CW:
                return "FRONT_FACE_CW";
            case RenderState::FRONT_FACE_CCW:
                return "FRONT_FACE_CCW";
        }
    }
    else if (std::strcmp("gameplay::RenderState::StencilFunc", enumName) == 0)
    {
        switch (value)
        {
            case RenderState::STENCIL_NEVER:
                return "STENCIL_NEVER";
            case RenderState::STENCIL_ALWAYS:
                return "STENCIL_ALWAYS";
            case RenderState::STENCIL_LESS:
                return "STENCIL_LESS";
            case RenderState::STENCIL_LEQUAL:
                return "STENCIL_LEQUAL";
            case RenderState::STENCIL_EQUAL:
                return "STENCIL_EQUAL";
            case RenderState::STENCIL_GREATER:
                return "STENCIL_GREATER";
            case RenderState::STENCIL_GEQUAL:
                return "STENCIL_GEQUAL";
            case RenderState::STENCIL_NOTEQUAL:
                return "STENCIL_NOTEQUAL";
        }
    }
    else if (std::strcmp("gameplay::RenderState::StencilOp", enumName) == 0)
    {
        switch (value)
        {
            case RenderState::STENCIL_OP_KEEP:
                return "STENCIL_OP_KEEP";
            case RenderState::STENCIL_OP_ZERO:
                return "STENCIL_OP_ZERO";
            case RenderState::STENCIL_OP_REPLACE:
                return "STENCIL_OP_REPLACE";
            case RenderState::STENCIL_OP_INCR:
                return "STENCIL_OP_INCR";
            case RenderState::STENCIL_OP_DECR:
                return "STENCIL_OP_DECR";
            case RenderState::STENCIL_OP_INVERT:
                return "STENCIL_OP_INVERT";
            case RenderState::STENCIL_OP_INCR_WRAP:
                return "STENCIL_OP_INCR_WRAP";
            case RenderState::STENCIL_OP_DECR_WRAP:
                return "STENCIL_OP_DECR_WRAP";
        }
    }
    
    return NULL;
}

int RenderState::enumParse(const char* enumName, const char* str)
{
    if (std::strcmp("gameplay::RenderState::AutoBinding", enumName) == 0)
    {
        if (std::strcmp("NONE", str) == 0)
            return RenderState::NONE;
        else if (std::strcmp("WORLD_MATRIX", str) == 0)
            return RenderState::WORLD_MATRIX;
        else if (std::strcmp("VIEW_MATRIX", str) == 0)
            return RenderState::VIEW_MATRIX;
        else if (std::strcmp("PROJECTION_MATRIX", str) == 0)
            return RenderState::PROJECTION_MATRIX;
        else if (std::strcmp("WORLD_VIEW_MATRIX", str) == 0)
            return RenderState::WORLD_VIEW_MATRIX;
        else if (std::strcmp("VIEW_PROJECTION_MATRIX", str) == 0)
            return RenderState::VIEW_PROJECTION_MATRIX;
        else if (std::strcmp("WORLD_VIEW_PROJECTION_MATRIX", str) == 0)
            return RenderState::WORLD_VIEW_PROJECTION_MATRIX;
        else if (std::strcmp("INVERSE_TRANSPOSE_WORLD_MATRIX", str) == 0)
            return RenderState::INVERSE_TRANSPOSE_WORLD_MATRIX;
        else if (std::strcmp("INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX", str) == 0)
            return RenderState::INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX;
        else if (std::strcmp("CAMERA_WORLD_POSITION", str) == 0)
            return RenderState::CAMERA_WORLD_POSITION;
        else if (std::strcmp("CAMERA_VIEW_POSITION", str) == 0)
            return RenderState::CAMERA_VIEW_POSITION;
        else if (std::strcmp("MATRIX_PALETTE", str) == 0)
            return RenderState::MATRIX_PALETTE;
        else if (std::strcmp("SCENE_AMBIENT_COLOR", str) == 0)
            return RenderState::SCENE_AMBIENT_COLOR;
    }
    else if (std::strcmp("gameplay::RenderState::BlendMode", enumName) == 0)
    {
        if (std::strcmp("BLEND_ZERO", str) == 0)
            return RenderState::BLEND_ZERO;
        else if (std::strcmp("BLEND_ONE", str) == 0)
            return RenderState::BLEND_ONE;
        else if (std::strcmp("BLEND_SRC_COLOR", str) == 0)
            return RenderState::BLEND_SRC_COLOR;
        else if (std::strcmp("BLEND_ONE_MINUS_SRC_COLOR", str) == 0)
            return RenderState::BLEND_ONE_MINUS_SRC_COLOR;
        else if (std::strcmp("BLEND_DST_COLOR", str) == 0)
            return RenderState::BLEND_DST_COLOR;
        else if (std::strcmp("BLEND_ONE_MINUS_DST_COLOR", str) == 0)
            return RenderState::BLEND_ONE_MINUS_DST_COLOR;
        else if (std::strcmp("BLEND_SRC_ALPHA", str) == 0)
            return RenderState::BLEND_SRC_ALPHA;
        else if (std::strcmp("BLEND_ONE_MINUS_SRC_ALPHA", str) == 0)
            return RenderState::BLEND_ONE_MINUS_SRC_ALPHA;
        else if (std::strcmp("BLEND_DST_ALPHA", str) == 0)
            return RenderState::BLEND_DST_ALPHA;
        else if (std::strcmp("BLEND_ONE_MINUS_DST_ALPHA", str) == 0)
            return RenderState::BLEND_ONE_MINUS_DST_ALPHA;
        else if (std::strcmp("BLEND_CONSTANT_ALPHA", str) == 0)
            return RenderState::BLEND_CONSTANT_ALPHA;
        else if (std::strcmp("BLEND_ONE_MINUS_CONSTANT_ALPHA", str) == 0)
            return RenderState::BLEND_ONE_MINUS_CONSTANT_ALPHA;
        else if (std::strcmp("BLEND_SRC_ALPHA_SATURATE", str) == 0)
            return RenderState::BLEND_SRC_ALPHA_SATURATE;
    }
    else if (std::strcmp("gameplay::RenderState::DepthFunc", enumName) == 0)
    {
        if (std::strcmp("DEPTH_NEVER", str) == 0)
            return RenderState::DEPTH_NEVER;
        else if (std::strcmp("DEPTH_LESS", str) == 0)
            return RenderState::DEPTH_LESS;
        else if (std::strcmp("DEPTH_EQUAL", str) == 0)
            return RenderState::DEPTH_EQUAL;
        else if (std::strcmp("DEPTH_LEQUAL", str) == 0)
            return RenderState::DEPTH_LEQUAL;
        else if (std::strcmp("DEPTH_GREATER", str) == 0)
            return RenderState::DEPTH_GREATER;
        else if (std::strcmp("DEPTH_NOTEQUAL", str) == 0)
            return RenderState::DEPTH_NOTEQUAL;
        else if (std::strcmp("DEPTH_GEQUAL", str) == 0)
            return RenderState::DEPTH_GEQUAL;
        else if (std::strcmp("DEPTH_ALWAYS", str) == 0)
            return RenderState::DEPTH_ALWAYS;
    }
    else if (std::strcmp("gameplay::RenderState::CullFaceSide", enumName) == 0)
    {
        if (std::strcmp("CULL_FACE_SIDE_BACK", str) == 0)
            return RenderState::CULL_FACE_SIDE_BACK;
        else if (std::strcmp("CULL_FACE_SIDE_FRONT", str) == 0)
            return RenderState::CULL_FACE_SIDE_FRONT;
        else if (std::strcmp("CULL_FACE_SIDE_FRONT_AND_BACK", str) == 0)
            return RenderState::CULL_FACE_SIDE_FRONT_AND_BACK;
    }
    else if (std::strcmp("gameplay::RenderState::FontFace", enumName) == 0)
    {
        if (std::strcmp("FRONT_FACE_CW", str) == 0)
            return RenderState::FRONT_FACE_CW;
        else if (std::strcmp("FRONT_FACE_CCW", str) == 0)
            return RenderState::FRONT_FACE_CCW;
    }
    else if (std::strcmp("gameplay::RenderState::StencilFunc", enumName) == 0)
    {
        if (std::strcmp("STENCIL_NEVER", str) == 0)
            return RenderState::STENCIL_NEVER;
        else if (std::strcmp("STENCIL_ALWAYS", str) == 0)
            return RenderState::STENCIL_ALWAYS;
        else if (std::strcmp("STENCIL_LESS", str) == 0)
            return RenderState::STENCIL_LESS;
        else if (std::strcmp("STENCIL_LEQUAL", str) == 0)
            return RenderState::STENCIL_LEQUAL;
        else if (std::strcmp("STENCIL_EQUAL", str) == 0)
            return RenderState::STENCIL_EQUAL;
        else if (std::strcmp("STENCIL_GREATER", str) == 0)
            return RenderState::STENCIL_GREATER;
        else if (std::strcmp("STENCIL_GEQUAL", str) == 0)
            return RenderState::STENCIL_GEQUAL;
        else if (std::strcmp("STENCIL_NOTEQUAL", str) == 0)
            return RenderState::STENCIL_NOTEQUAL;
    }
    
    else if (std::strcmp("gameplay::RenderState::StencilOp", enumName) == 0)
    {
        if (std::strcmp("STENCIL_OP_KEEP", str) == 0)
            return RenderState::STENCIL_OP_KEEP;
        else if (std::strcmp("STENCIL_OP_ZERO", str) == 0)
            return RenderState::STENCIL_OP_ZERO;
        else if (std::strcmp("STENCIL_OP_REPLACE", str) == 0)
            return RenderState::STENCIL_OP_REPLACE;
        else if (std::strcmp("STENCIL_OP_INCR", str) == 0)
            return RenderState::STENCIL_OP_INCR;
        else if (std::strcmp("STENCIL_OP_DECR", str) == 0)
            return RenderState::STENCIL_OP_DECR;
        else if (std::strcmp("STENCIL_OP_INVERT", str) == 0)
            return RenderState::STENCIL_OP_INVERT;
        else if (std::strcmp("STENCIL_OP_INCR_WRAP", str) == 0)
            return RenderState::STENCIL_OP_INCR_WRAP;
        else if (std::strcmp("STENCIL_OP_DECR_WRAP", str) == 0)
            return RenderState::STENCIL_OP_DECR_WRAP;
    }
    
    return -1;
}

void RenderState::StateBlock::setState(const char* name, const char* value)
{
    GP_ASSERT(name);

    if (strcmp(name, "blend") == 0)
    {
        setBlend(parseBool(value));
    }
    else if (strcmp(name, "blendSrc") == 0)
    {
        setBlendSrc(parseBlendMode(value));
    }
    else if (strcmp(name, "blendDst") == 0)
    {
        setBlendDst(parseBlendMode(value));
    }
    else if (strcmp(name, "cullFace") == 0)
    {
        setCullFace(parseBool(value));
    }
    else if (strcmp(name, "cullFaceSide") == 0)
    {
        setCullFaceSide(parseCullFaceSide(value));
    }
    else if (strcmp(name, "frontFace") == 0)
    {
        setFrontFace(parseFrontFace(value));
    }
    else if (strcmp(name, "depthTest") == 0)
    {
        setDepthTest(parseBool(value));
    }
    else if (strcmp(name, "depthWrite") == 0)
    {
        setDepthWrite(parseBool(value));
    }
    else if (strcmp(name, "depthFunc") == 0)
    {
        setDepthFunc(parseDepthFunc(value));
    }
	else if (strcmp(name, "stencilTest") == 0)
    {
		setStencilTest(parseBool(value));
    }
	else if (strcmp(name, "stencilWrite") == 0)
    {
		setStencilWrite(parseUInt(value));
    }
	else if (strcmp(name, "stencilFunc") == 0)
    {
		setStencilFunc(parseStencilFunc(value), _stencilFuncRef, _stencilFuncMask);
    }
	else if (strcmp(name, "stencilFuncRef") == 0)
    {
		setStencilFunc(_stencilFunc, parseInt(value), _stencilFuncMask);
    }
	else if (strcmp(name, "stencilFuncMask") == 0)
    {
		setStencilFunc(_stencilFunc, _stencilFuncRef, parseUInt(value));
    }
	else if (strcmp(name, "stencilOpSfail") == 0)
    {
		setStencilOp(parseStencilOp(value), _stencilOpDpfail, _stencilOpDppass);
    }
	else if (strcmp(name, "stencilOpDpfail") == 0)
    {
		setStencilOp(_stencilOpSfail, parseStencilOp(value), _stencilOpDppass);
    }
	else if (strcmp(name, "stencilOpDppass") == 0)
    {
		setStencilOp(_stencilOpSfail, _stencilOpDpfail, parseStencilOp(value));
    }
    else
    {
        GP_ERROR("Unsupported render state string '%s'.", name);
    }
}

void RenderState::StateBlock::setBlend(bool enabled)
{
    _blendEnabled = enabled;
    if (!enabled)
    {
        _bits &= ~RS_BLEND;
    }
    else
    {
        _bits |= RS_BLEND;
    }
}

void RenderState::StateBlock::setBlendSrc(BlendMode blend)
{
    _blendSrc = blend;
    if (_blendSrc == BLEND_ONE && _blendDst == BLEND_ZERO)
    {
        // Default blend func
        _bits &= ~RS_BLEND_FUNC;
    }
    else
    {
        _bits |= RS_BLEND_FUNC;
    }
}

void RenderState::StateBlock::setBlendDst(BlendMode blend)
{
    _blendDst = blend;
    if (_blendSrc == BLEND_ONE && _blendDst == BLEND_ZERO)
    {
        // Default blend func
        _bits &= ~RS_BLEND_FUNC;
    }
    else
    {
        _bits |= RS_BLEND_FUNC;
    }
}

void RenderState::StateBlock::setCullFace(bool enabled)
{
    _cullFaceEnabled = enabled;
    if (!enabled)
    {
        _bits &= ~RS_CULL_FACE;
    }
    else
    {
        _bits |= RS_CULL_FACE;
    }
}

void RenderState::StateBlock::setCullFaceSide(CullFaceSide side)
{
    _cullFaceSide = side;
    if (_cullFaceSide == CULL_FACE_SIDE_BACK)
    {
        // Default cull side
        _bits &= ~RS_CULL_FACE_SIDE;
    }
    else
    {
        _bits |= RS_CULL_FACE_SIDE;
    }
}

void RenderState::StateBlock::setFrontFace(FrontFace winding)
{
    _frontFace = winding;
    if (_frontFace == FRONT_FACE_CCW)
    {
        // Default front face
        _bits &= ~RS_FRONT_FACE;
    }
    else
    {
        _bits |= RS_FRONT_FACE;
    }
}

void RenderState::StateBlock::setDepthTest(bool enabled)
{
    _depthTestEnabled = enabled;
    if (!enabled)
    {
        _bits &= ~RS_DEPTH_TEST;
    }
    else
    {
        _bits |= RS_DEPTH_TEST;
    }
}

void RenderState::StateBlock::setDepthWrite(bool enabled)
{
    _depthWriteEnabled = enabled;
    if (enabled)
    {
        _bits &= ~RS_DEPTH_WRITE;
    }
    else
    {
        _bits |= RS_DEPTH_WRITE;
    }
}

void RenderState::StateBlock::setDepthFunc(DepthFunc func)
{
    _depthFunc = func;
    if (_depthFunc == DEPTH_LESS)
    {
        // Default depth function
        _bits &= ~RS_DEPTH_FUNC;
    }
    else
    {
        _bits |= RS_DEPTH_FUNC;
    }
}

void RenderState::StateBlock::setStencilTest(bool enabled)
{
	_stencilTestEnabled = enabled;
	if (!enabled)
	{
		_bits &= ~RS_STENCIL_TEST;
	}
	else
	{
		_bits |= RS_STENCIL_TEST;
	}
}

void RenderState::StateBlock::setStencilWrite(unsigned int mask)
{
	_stencilWrite = mask;
	if (mask == RS_ALL_ONES)
	{
		// Default stencil write
		_bits &= ~RS_STENCIL_WRITE;
	}
	else
	{
		_bits |= RS_STENCIL_WRITE;
	}
}

void RenderState::StateBlock::setStencilFunc(StencilFunc func, int ref, unsigned int mask)
{
	_stencilFunc = func;
	_stencilFuncRef = ref;
	_stencilFuncMask = mask;
	if (func == STENCIL_ALWAYS && ref == 0 && mask == RS_ALL_ONES)
	{
		// Default stencil function
		_bits &= ~RS_STENCIL_FUNC;
	}
	else
	{
		_bits |= RS_STENCIL_FUNC;
	}
}

void RenderState::StateBlock::setStencilOp(StencilOp sfail, StencilOp dpfail, StencilOp dppass)
{
	_stencilOpSfail = sfail;
	_stencilOpDpfail = dpfail;
	_stencilOpDppass = dppass;
	if (sfail == STENCIL_OP_KEEP && dpfail == STENCIL_OP_KEEP && dppass == STENCIL_OP_KEEP)
	{
		// Default stencil operation
		_bits &= ~RS_STENCIL_OP;
	}
	else
	{
		_bits |= RS_STENCIL_OP;
	}
}

const char* RenderState::StateBlock::getSerializedClassName() const
{
    return "gameplay::RenderState::StateBlock";
}

void RenderState::StateBlock::serialize(Serializer* serializer)
{
    serializer->writeBool("blend", _blendEnabled, false);
    serializer->writeEnum("blendSrc", "gameplay::RenderState::BlendMode", _blendSrc, BLEND_ONE);
    serializer->writeEnum("blendDst", "gameplay::RenderState::BlendMode", _blendDst, BLEND_ZERO);
    serializer->writeBool("cullFace", _cullFaceEnabled, false);
    serializer->writeEnum("cullFaceSize", "gameplay::RenderState::CullFaceSide", _cullFaceSide, CULL_FACE_SIDE_BACK);
    serializer->writeEnum("frontFace", "gameplay::RenderState::FrontFace", _frontFace, FRONT_FACE_CCW);
    serializer->writeBool("depthTest", _depthTestEnabled, false);
    serializer->writeBool("depthWrite", _depthWriteEnabled, true);
    serializer->writeEnum("depthFunc", "gameplay::RenderState::DepthFunc", _depthFunc, DEPTH_LESS);
    serializer->writeBool("stencilTest", _stencilTestEnabled, false);
    serializer->writeInt("stencilWrite", _stencilWrite, RS_ALL_ONES);
    serializer->writeEnum("stencilFunc", "gameplay::RenderState::StencilFunc", _stencilFunc, STENCIL_ALWAYS);
    serializer->writeInt("stencilFuncRef", _stencilFuncRef, 0);
    serializer->writeInt("stencilFuncMask", _stencilFuncMask, RS_ALL_ONES);
    serializer->writeEnum("stencilOpSfail", "gameplay::RenderState::StencilOp", _stencilOpSfail, STENCIL_OP_KEEP);
    serializer->writeEnum("stencilOpDpfail", "gameplay::RenderState::StencilOp", _stencilOpDpfail, STENCIL_OP_KEEP);
    serializer->writeEnum("stencilOpDppass", "gameplay::RenderState::StencilOp", _stencilOpDppass, STENCIL_OP_KEEP);
}

void RenderState::StateBlock::deserialize(Serializer* serializer)
{
    _blendEnabled = serializer->readBool("blend", false);
    _blendSrc = static_cast<gameplay::RenderState::BlendMode>(serializer->readEnum("blendSrc", "gameplay::RenderState::BlendMode", BLEND_ONE));
    _blendDst = static_cast<gameplay::RenderState::BlendMode>(serializer->readEnum("blendDst", "gameplay::RenderState::BlendMode", BLEND_ZERO));
    _cullFaceEnabled = serializer->readBool("cullFace", false);
    _cullFaceSide = static_cast<gameplay::RenderState::CullFaceSide>(serializer->readEnum("cullFaceSize", "gameplay::RenderState::CullFaceSide", CULL_FACE_SIDE_BACK));
    _frontFace = static_cast<gameplay::RenderState::FrontFace>(serializer->readEnum("frontFace", "gameplay::RenderState::FrontFace", FRONT_FACE_CCW));
    _depthTestEnabled = serializer->readBool("depthTest", false);
    _depthWriteEnabled = serializer->readBool("depthWrite", true);
    _depthFunc = static_cast<gameplay::RenderState::DepthFunc>(serializer->readEnum("depthFunction", "gameplay::RenderState::DepthFunc", DEPTH_LESS));
    _stencilTestEnabled = serializer->readBool("stencilTest", false);
    _stencilWrite = serializer->readInt("stencilWrite", RS_ALL_ONES);
    _stencilFunc = static_cast<gameplay::RenderState::StencilFunc>(serializer->readEnum("stencilFunc", "gameplay::RenderState::StencilFunction", STENCIL_ALWAYS));
    _stencilFuncRef = serializer->readInt("stencilFuncRef", 0);
    _stencilFuncMask = serializer->readInt("stencilFuncMask", RS_ALL_ONES);
    _stencilOpSfail = static_cast<gameplay::RenderState::StencilOp>(serializer->readEnum("stencilOpSfail", "gameplay::RenderState::StencilOp", STENCIL_OP_KEEP));
    _stencilOpDpfail = static_cast<gameplay::RenderState::StencilOp>(serializer->readEnum("stencilOpDpfail", "gameplay::RenderState::StencilOp", STENCIL_OP_KEEP));
    _stencilOpDppass = static_cast<gameplay::RenderState::StencilOp>(serializer->readEnum("stencilOpDppass", "gameplay::RenderState::StencilOp", STENCIL_OP_KEEP));
    _bits = 0L;
}

RenderState::AutoBindingResolver::AutoBindingResolver()
{
    _customAutoBindingResolvers.push_back(this);
}

RenderState::AutoBindingResolver::~AutoBindingResolver()
{
    std::vector<RenderState::AutoBindingResolver*>::iterator itr = std::find(_customAutoBindingResolvers.begin(), _customAutoBindingResolvers.end(), this);
    if (itr != _customAutoBindingResolvers.end())
        _customAutoBindingResolvers.erase(itr);
}

}
