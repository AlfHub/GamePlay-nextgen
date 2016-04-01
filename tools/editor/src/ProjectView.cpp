#include "ProjectView.h"
#include "Project.h"
#include "ui_ProjectView.h"
#include <QtWidgets>


ProjectView::ProjectView(QWidget* parent) :
    QWidget(parent), _ui(new Ui::ProjectView),
    _editor(static_cast<EditorWindow*>(parent)),
    _project(NULL)
{
    _ui->setupUi(this);

    connect(_ui->projectTreeView, SIGNAL(doubleClicked(QModelIndex)), _ui->projectTreeView, SLOT(itemDoubleClicked(QModelIndex)));
    connect(_ui->actionOpen_File, SIGNAL(triggered(bool)), _ui->projectTreeView, SLOT(openFileTriggered()));
}

ProjectView::~ProjectView()
{
    delete _ui;
    closeProject();
}

Ui::ProjectView* ProjectView::ui()
{
    return _ui;
}

void ProjectView::openProject(const QString& path)
{
    closeProject();
    _project = Project::open(path, _ui->projectTreeView);
    if (_project)
    {
        _project->setRootPath(path);

        // Project files that the editor supports
        QStringList filters;
        filters << "*.config" << "*.scene" <<
                   "*.material" << "*.vert"<< ".frag" <<
                   "*.physics" << "*.particles" <<
                   "*.form" << "*.theme" <<
                   "*.fbx" << "*.ttf" << "*.png" <<
                   "*.lua" << "*.cpp" << "*.h";
        _project->setNameFilters(filters);
        _project->setNameFilterDisables(false);

        _ui->projectTreeView->setModel(_project);
        _ui->projectTreeView->setRootIndex(_project->index(path));
        _ui->projectTreeView->hideColumn(2);
        _ui->projectTreeView->setColumnWidth(0, 760);
        _ui->projectTreeView->setColumnWidth(1, 120);
        _ui->projectTreeView->setColumnWidth(2, 120);

        QString resFolderPath = path + QString("/") + QString(QLatin1String("res"));
        _ui->projectTreeView->expand(_project->index(resFolderPath));

        // Open the last scene editor for the project.        
        openScene(path+ QString("/") + _project->scene());
    }
}

void ProjectView::closeProject()
{
    SAFE_DELETE(_project);
}

Project* ProjectView::project() const
{
    return _project;
}

void ProjectView::openScene(const QString& path)
{
    emit sceneOpened(path);
}

