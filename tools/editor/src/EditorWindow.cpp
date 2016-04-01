#include "EditorWindow.h"
#include "GameView.h"
#include "Project.h"
#include "ProjectWizard.h"
#include "ProjectTreeView.h"
#include "ProjectView.h"
#include "SceneView.h"
#include "PropertiesView.h"
#include "ui_EditorWindow.h"
#include <QtWidgets>

#define EDITOR_WINDOW_TITLE "GamePlay Editor"

EditorWindow::EditorWindow(QWidget* parent) :
    QMainWindow(parent), _ui(new Ui::EditorWindow),
    _projectWizard(0), _projectView(0),
    _gameView(0), _sceneView(0), _propertiesView(0),
    _scene(NULL)
{
    _ui->setupUi(this);

    // Initialize the stylesheet
    QFile file(":/gameplay-editor.css");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         QTextStream stream(&file);
         qApp->setStyleSheet(stream.readAll());
         file.close();
    }

    // Initialize some effects
    QApplication::setEffectEnabled(Qt::UI_FadeMenu);
    QApplication::setEffectEnabled(Qt::UI_FadeTooltip);

    // Initialize the status bar
    QStatusBar* statusBar = this->statusBar();
    statusBar->showMessage(tr("Input Mode: Move"));

    // Initialize the game view
    _gameView = new GameView(this);
    setCentralWidget(_gameView);

    // Initialize the menu bar
    QMenuBar* menuBar = this->menuBar();
    QMenu* projectMenu = menuBar->addMenu(tr("Project"));
    projectMenu->addAction(_ui->actionNew);
    projectMenu->addAction(_ui->actionOpen);
    projectMenu->addAction(_ui->actionSave);
    projectMenu->addSeparator();
    projectMenu->addAction(_ui->actionPlay);
    projectMenu->addAction(_ui->actionPackage);
    QMenu* editMenu = menuBar->addMenu(tr("Edit"));
    editMenu->addAction(_ui->actionPreferences);
    editMenu->addAction(_ui->actionUndo);
    editMenu->addAction(_ui->actionRedo);
    editMenu->addSeparator();
    editMenu->addAction(_ui->actionCut);
    editMenu->addAction(_ui->actionCopy);
    editMenu->addAction(_ui->actionPaste);
    editMenu->addSeparator();
    editMenu->addAction(_ui->actionDuplicate);
    editMenu->addAction(_ui->actionDelete);
    editMenu->addAction(_ui->actionFrame);
    editMenu->addAction(_ui->actionFind);
    editMenu->addSeparator();
    QMenu* helpMenu = menuBar->addMenu(tr("Help"));
    helpMenu->addAction(_ui->actionHelp);
    helpMenu->addAction(_ui->actionAbout);

    // Initialize the toolbar
    QToolBar* toolBar = this->addToolBar(tr("Top"));
    toolBar->addAction(_ui->actionNew);
    toolBar->addAction(_ui->actionOpen);
    toolBar->addAction(_ui->actionSave);
    toolBar->addSeparator();
    toolBar->addAction(_ui->actionImport);
    toolBar->addSeparator();
    toolBar->addAction(_ui->actionAdd);
    toolBar->addSeparator();
    toolBar->addAction(_ui->actionPlay);
    toolBar->addSeparator();
    toolBar->addAction(_ui->actionFullscreen);
    toolBar->addAction(_ui->actionGrid);

    // Initialize Add Xxxx toolbar buttons
    QMenu* addMenu = new QMenu();
    addMenu->setObjectName("addContextMenu");
    QToolButton* addButton = (QToolButton*)toolBar->widgetForAction(_ui->actionAdd);
    addButton->setPopupMode(QToolButton::InstantPopup);
    addButton->setMenu(addMenu);
    addMenu->addAction(_ui->actionAdd_Group);
    addMenu->addAction(_ui->actionAdd_Model);
    addMenu->addAction(_ui->actionAdd_Camera);
    addMenu->addAction(_ui->actionAdd_DirectionalLight);
    addMenu->addAction(_ui->actionAdd_PointLight);
    addMenu->addAction(_ui->actionAdd_SpotLight);

    // Initialize the left toolbar
    toolBar = this->addToolBar(tr("Left"));
    this->addToolBar(Qt::LeftToolBarArea, toolBar);
    QActionGroup* transformGroup = new QActionGroup(toolBar);
    toolBar->addAction(_ui->actionPan);
    _ui->actionPan->setActionGroup(transformGroup);
    toolBar->addAction(_ui->actionMove);
    _ui->actionMove->setActionGroup(transformGroup);
    toolBar->addAction(_ui->actionRotate);
    _ui->actionRotate->setActionGroup(transformGroup);
    toolBar->addAction(_ui->actionScale);
    _ui->actionScale->setActionGroup(transformGroup);
    toolBar->addAction(_ui->actionSelect);
    _ui->actionSelect->setActionGroup(transformGroup);

    // Initialize the docking corners
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    // Initialize the project view
    _projectView= new ProjectView(_ui->projectDockWidget);
    _ui->projectDockWidget->setWidget(_projectView);

    // Initialize the scene view
    _sceneView = new SceneView(_ui->sceneDockWidget);
    _ui->sceneDockWidget->setWidget(_sceneView);

    // Initialize the properties view
    _propertiesView = new PropertiesView(_ui->propertiesDockWidget);
    _ui->propertiesDockWidget->setWidget(_propertiesView);

    connect(_ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(actionOpenTriggered()));
    connect(_ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(actionNewTriggered()));
    connect(_projectView, SIGNAL(sceneOpened(QString)), this, SLOT(sceneOpened(QString)));
    connect(this, SIGNAL(sceneChanged()), _sceneView, SLOT(sceneChanged()));
    connect(this, SIGNAL(sceneChanged()), _gameView, SLOT(sceneChanged()));
}

EditorWindow::~EditorWindow()
{
    delete _ui;
}

const std::string& EditorWindow::getAssetPath() const
{
    return _assetPath;
}

void EditorWindow::setProjectWizard(ProjectWizard* projectWizard)
{
    _projectWizard = projectWizard;    
    connect(_projectWizard, SIGNAL(projectOpened(QString)), this, SLOT(projectOpened(QString)));
}

Project* EditorWindow::project() const
{
    return _projectView->project();
}

Scene* EditorWindow::scene() const
{
    return _scene;
}

void EditorWindow::projectOpened(const QString& path)
{
    _projectView->openProject(path);
    Project* project = _projectView->project();
    if (project)
    {
        setWindowTitle(QString(QLatin1String(EDITOR_WINDOW_TITLE)) + QString(QLatin1String(" - ")) +
                       _projectView->project()->name());
    }
}

void EditorWindow::sceneOpened(const QString& path)
{
    QByteArray pathByteArray = path.toLatin1();
    Serializer* serializer = Serializer::createReader(pathByteArray.data());
    _scene = static_cast<Scene*>(serializer->readObject(NULL));

    emit sceneChanged();
}

void EditorWindow::actionOpenTriggered()
{
    _projectWizard->readProjects();
    _projectWizard->show();
}

void EditorWindow::actionNewTriggered()
{
    _projectWizard->show();
    _projectWizard->newProjectPressed();
}

