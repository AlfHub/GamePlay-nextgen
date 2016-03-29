#ifndef JOINT_H_
#define JOINT_H_

#include "Node.h"

namespace gameplay
{

class MeshSkin;
class Bundle;

/**
 * Defines a joint node.
 *
 * This represent a joint in a skeleton that is hierarchially part of
 * a MeshSkin. This allows the vertices in the mesh to be blended and 
 * animated using the sum of the blend weight that must add up to 1.0.
 */
class Joint : public Node
{
    friend class Serializer::Activator;
    friend class Node;
    friend class MeshSkin;
    friend class Bundle;

public:

    /**
     * @see Node::getScene()
     */
    Scene* getScene() const;

    /**
     * Returns the inverse bind pose matrix for this joint.
     * 
     * @return Inverse bind pose matrix.
     */
    const Matrix& getInverseBindPose() const;

protected:

    /**
     * Constructor.
     */
    Joint();
    
    /**
     * Constructor.
     */
    Joint(const char* id);
    
    /**
     * Destructor.
     */
    virtual ~Joint();
    
    /**
     * Creates a new joint with the given id.
     * 
     * @param id ID string.
     * 
     * @return Newly created joint.
     */
    static Joint* create(const char* id);

    /**
     * Clones a single node and its data but not its children.
     * This method returns a node pointer but actually creates a Joint.
     * 
     * @param context The clone context.
     * 
     * @return Pointer to the newly created joint.
     */
    virtual Node* cloneSingleNode(NodeCloneContext &context) const;

    /**
     * Sets the inverse bind pose matrix.
     * 
     * @param m Matrix representing the inverse bind pose for this Joint.
     */
    void setInverseBindPose(const Matrix& m);

    /**
     * Updates the joint matrix.
     * 
     * @param bindShape The bind shape matrix.
     * @param matrixPalette The matrix palette to update.
     */
    void updateJointMatrix(const Matrix& bindShape, Vector4* matrixPalette);

    /**
     * Called when this Joint's transform changes.
     */
    void transformChanged();

    /**
     * @see Serializeable::getSerializedClassName
     */
    const char* getSerializedClassName() const;
    
    /**
     * @see Serializeable::serialize
     */
    void serialize(Serializer* serializer);
    
    /**
     * @see Serializeable::deserialize
     */
    void deserialize(Serializer* serializer);

private:

    /**
     * Track mesh skins referencing a joint.
     */
    struct SkinReference
    {
        MeshSkin* skin;
        SkinReference* next;
        
        SkinReference();
        
        ~SkinReference();
    };

    /**
     * Constructor.
     */
    Joint(const Joint& copy);

    /**
     * Copy assignment operator.
     */
    Joint& operator=(const Joint&);

    /**
     * @see Serializer::Activator::CreateInstanceCallback
     */
    static Serializable* createInstance();

    /**
     * Adds a skin to the joint.
     */
    void addSkin(MeshSkin* skin);

    /**
     * Removes a skin to the joint.
     */
    void removeSkin(MeshSkin* skin);

    Matrix _bindPose;       // The Matrix representation of the Joint's bind pose.
    bool _jointMatrixDirty; // Flag used to mark if the Joint's matrix is dirty.
    SkinReference _skin;    //Linked list of mesh skins that are referenced by this joint.
};

}

#endif
