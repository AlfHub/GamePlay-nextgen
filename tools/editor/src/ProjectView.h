#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include "EditorWindow.h"
#include <QWidget>

namespace Ui {
class ProjectView;
}

class Project;

/**
 * The project view showing files visible in the project directory.
 */
class ProjectView : public QWidget
{
    Q_OBJECT
    friend class ProjectTreeView;
public:
    /**
     * Constructor.
     * @param parent The parent widget.
     */
    explicit ProjectView(QWidget* parent = 0);

    /**
     * Destructor.
     */
    ~ProjectView();

    /**
     * Opens a project at the specified path.
     * @param path The project path.
     */
    void openProject(const QString& path);

    /**
     * Closes the currently open project.
     */
    void closeProject();

    /**
     * Gets the project that is open.
     * @return The open project.
     */
    Project* project() const;

protected:
    /**
     * Gets the ui associated with this view.
     * @return The view ui.
     */
    Ui::ProjectView* ui();

    /**
     * Opens a scene at the specified path.
     * @param path The path of the scene to be opened.
     */
    void openScene(const QString& path);

signals:
    /**
     * Signal emmited with a new scene has been opened.
     * @param path The path of the scene being opened.
     */
    void sceneOpened(const QString& path);

private:
    Ui::ProjectView* _ui;
    EditorWindow* _editor;
    Project* _project;
};

#endif
