#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include "ProjectWizardItemDelegate.h"
#include <QDialog>
#include <QStyledItemDelegate>

namespace Ui {
class ProjectWizard;
}

class EditorWindow;
class Project;

/**
 * Defines a class used for selecting an existing
 * project/sample to open or a new project
 * to be created.
 */
class ProjectWizard : public QDialog
{
    Q_OBJECT

public:

    /**
     * Constructor.
     * @param parent The parent widget.
     */
    explicit ProjectWizard(QWidget* parent = 0);

    /**
     * Destructor.
     */
    ~ProjectWizard();

    /**
     * Initialize the project wizard and ui.
     */
    bool initialize();

    /**
     * Sets the editor to interface with.
     * @param editor The editor to interface with.
     */
    void setEditor(EditorWindow* editor);

     void readProjects();


public slots:

    /**
     * Handles the new project pressed.
     */
    void newProjectPressed();

    /**
     * Handles the open project pressed.
     */
    void openProjectPressed();

    /**
     * Handles the cancel project pressed.
     */
    void cancelProjectPressed();

    /**
     * Handles the create project pressed.
     */
    void createProjectPressed();

    /**
     * Handles the project name text being changed.
     */
    void projectNameTextChanged();

    /**
     * Handles the project directory being pressed.
     */
    void projectDirectoryPressed();

    /**
     * Handles the project directory text being changed.
     */
    void projectDirectoryTextChanged();

signals:

    void projectOpened(const QString& path);

private:

    Ui::ProjectWizard* _ui;
    EditorWindow* _editor;
    QStringList _recentProjects;
    QString _projectDirectoryRoot;
    QString _projectDirectory;
    ProjectWizardItemDelegate _itemDelegate;
};

#endif
