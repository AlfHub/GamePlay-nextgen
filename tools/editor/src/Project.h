#ifndef PROJECT_H
#define PROJECT_H

#include "gameplay.h"
using namespace gameplay;

#include <QFileSystemModel>

/**
 * The model of the project for editor.
 */
class Project : public QFileSystemModel, Serializable
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param parent The parent widget.
     */
    explicit Project(QObject* parent = 0);

    /**
     * Destructor.
     */
    ~Project();

public:
    /**
     * Creates a new project at the specified path.
     * @param name The name of the project to be created.
     * @param path The path to create the project at.
     */
    static Project* create(const QString& path, const QString& name, QObject* parent = 0);

    /**
     * Opens a project from the specified path.
     * @param projectPath The path to open the project from.
     */
    static Project* open(const QString& path, QObject* parent = 0);

    /**
     * Gets the project name.
     * @return The project name.
     */
    QString name();

    /**
     * Gets the project path.
     * @return The project path.
     */
    QString path();

    /**
     * Gets the last scene opened in the project.
     * @return The last scene opened in the project.
     */
    QString scene();

    /**
     * Sets the path of the scene currently open in the project.
     * @param scene The path of the scene currently open in the project.
     */
    void setScene(const QString& scene);

    /**
     * @see QAbstractItemModel::data
     */
    QVariant data(const QModelIndex& index, int role) const;

    /**
     * @see Serializer::Activator::CreateInstanceCallback
     */
    static Serializable* createInstance();

    /**
     * @see Serializable::getSerializedClassName
     */
    const char* getSerializedClassName() const;

    /**
     * @see Serializable::serialize
     */
    void serialize(Serializer* serializer);

    /**
     * @see Serializable::deserialize
     */
    void deserialize(Serializer* serializer);

private:
    static bool _serializerActivated;
    std::string _path;
    std::string _name;
    std::string _scene;
};

#endif
