#include "ProjectWizard.h"
#include "ProjectWizardItemDelegate.h"
#include "Project.h"
#include "EditorWindow.h"
#include "ui_ProjectWizard.h"

#include <QtWidgets>
#define SETTINGS_ORGANIZATION "gameplay"
#define SETTINGS_APPLICATION "editor"
#define SETTINGS_RECENTS_PROECTS "recentProjects"
#define SETTINGS_LAST_PROJECT_PATH "lastProjectPath"
#define PROJECT_NEW "NewProject"
#define PROJECT_FILE "game.project"


ProjectWizard::ProjectWizard(QWidget* parent) :
    QDialog(parent), _ui(new Ui::ProjectWizard), _editor(NULL)
{
    _ui->setupUi(this);
}

ProjectWizard::~ProjectWizard()
{
    delete _ui;
}

bool ProjectWizard::initialize()
{
    QFile file(":/gameplay-editor.css");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         QTextStream stream(&file);
         qApp->setStyleSheet(stream.readAll());
         file.close();
    }

    _ui->listWidgetProjects->setItemDelegate(&_itemDelegate);

    connect(_ui->pushButtonNew, SIGNAL(pressed()), this, SLOT(newProjectPressed()));
    connect(_ui->pushButtonOpen, SIGNAL(pressed()), this, SLOT(openProjectPressed()));
    connect(_ui->pushButtonCreate, SIGNAL(pressed()), this, SLOT(createProjectPressed()));
    connect(_ui->pushButtonCancel, SIGNAL(pressed()), this, SLOT(cancelProjectPressed()));
    connect(_ui->lineEditProjectName, SIGNAL(textChanged(QString)), this, SLOT(projectNameTextChanged()));
    connect(_ui->pushButtonProjectDirectory, SIGNAL(pressed()), this, SLOT(projectDirectoryPressed()));
    connect(_ui->lineEditProjectDirectory, SIGNAL(textChanged(QString)), this, SLOT(projectDirectoryTextChanged()));

    readProjects();

    return true;
}

void ProjectWizard::setEditor(EditorWindow* editor)
{
    _editor = editor;
}

void ProjectWizard::readProjects()
{
    _ui->frameCreateProject->hide();
    _ui->pushButtonNew->show();
    _ui->pushButtonOpen->show();

    // Restore previously created projects
    QSettings settings(SETTINGS_ORGANIZATION, SETTINGS_APPLICATION);
    _recentProjects = settings.value(SETTINGS_RECENTS_PROECTS).toStringList();
    if (_recentProjects.size() > 0)
    {
        _ui->listWidgetProjects->clear();
        for (unsigned int i = 0; i < _recentProjects.size(); i++)
        {
            const QString path = _recentProjects.at(i);
            Project* project = Project::open(path);
            if (project)
            {
                QListWidgetItem* item = new QListWidgetItem();
                item->setData(Qt::DisplayRole, project->name());
                item->setData(Qt::UserRole, path);
                _ui->listWidgetProjects->addItem(item);
                SAFE_DELETE(project);
            }
            else
            {
                _recentProjects.removeAt(i);
                i--;
            }
        }
    }
    settings.setValue(QString(QLatin1String(SETTINGS_RECENTS_PROECTS)), _recentProjects);

    // Updte the ui frame components
    if (_recentProjects.size() == 0)
    {
        _ui->frameCreateProjectTip->show();
        _ui->listWidgetProjects->hide();
    }
    else
    {
        _ui->pushButtonOpen->setEnabled(true);
        _ui->frameCreateProjectTip->hide();
        _ui->listWidgetProjects->show();
    }
}

void ProjectWizard::newProjectPressed()
{
    _ui->listWidgetProjects->hide();
    _ui->pushButtonNew->hide();
    _ui->pushButtonOpen->hide();
    _ui->frameCreateProjectTip->hide();
    _ui->frameCreateProject->show();
    _ui->lineEditProjectName->setText(tr(PROJECT_NEW));
    _ui->lineEditProjectName->setSelection(0, _ui->lineEditProjectName->text().length());
    _ui->lineEditProjectName->setFocus();
    _ui->lineEditProjectDirectory->setText(QDir::homePath() + QString("/") + _ui->lineEditProjectName->text());
    _projectDirectoryRoot = QDir::homePath();
}

void ProjectWizard::openProjectPressed()
{
    QList<QListWidgetItem*> selected = _ui->listWidgetProjects->selectedItems();
    if (selected.size() == 0)
    {
        QString projectFilePath = QFileDialog::getOpenFileName(this, tr("Open Project"), ".", tr("Project Files (*.project)"));
        if (!projectFilePath.isEmpty())
        {
            QDir projectPath = QFileInfo(projectFilePath).absoluteDir();
            _editor->show();
            emit projectOpened(projectPath.path());
            this->hide();
        }
        return;
    }

    QListWidgetItem* item = selected.at(0);
    if (!item)
        return;

    QVariant data = item->data(Qt::UserRole);
    QString path = data.toString();
    if (!QDir(path).exists())
    {
        QMessageBox::warning(this,
                             tr("Open Project"),
                             tr("Project directory no longer exists. Removing project from recents."),
                             QMessageBox::Ok);
        _ui->listWidgetProjects->removeItemWidget(item);
    }

    _editor->show();
    emit projectOpened(path);
    this->hide();
}

void ProjectWizard::cancelProjectPressed()
{
    readProjects();
}

void ProjectWizard::createProjectPressed()
{
    if (QDir(_ui->lineEditProjectDirectory->text()).exists())
    {
        QMessageBox::warning(this,
                             tr("Create Project"),
                             tr("Directory already exists.\nDelete the directory or pick a different folder name."),
                             QMessageBox::Ok);
        return;
    }

    if (QDir().mkdir(_ui->lineEditProjectDirectory->text()))
    {
        Project* project = Project::create(_ui->lineEditProjectDirectory->text(),
                                           _ui->lineEditProjectName->text());
        if (project)
        {
            // Save our project to recent system project
            _recentProjects.push_back(_ui->lineEditProjectDirectory->text());
            QSettings settings(SETTINGS_ORGANIZATION, SETTINGS_APPLICATION);
            settings.setValue(QString(QLatin1String(SETTINGS_RECENTS_PROECTS)), _recentProjects);

            SAFE_DELETE(project);

            // Show the editor and hide the wizard
            _editor->show();            
            emit projectOpened(_ui->lineEditProjectDirectory->text());
            this->hide();
            _ui->listWidgetProjects->show();
            _ui->pushButtonNew->show();
            _ui->pushButtonOpen->show();
        }
        else
        {
            QMessageBox::warning(this,
                                 tr("Create Project"),
                                 tr("Unable to create project contents."),
                                 QMessageBox::Ok);
            return;
        }
    }
    else
    {
        QMessageBox::warning(this,
                             tr("Create Project"),
                             tr("Unable to create project directory."),
                             QMessageBox::Ok);
        return;
    }
}

void ProjectWizard::projectNameTextChanged()
{
    _ui->lineEditProjectDirectory->setText(_projectDirectoryRoot +
                                           QString("/") +
                                           _ui->lineEditProjectName->text());
}

void ProjectWizard::projectDirectoryPressed()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("Open Directory"),
                                                          _projectDirectoryRoot,
                                                          QFileDialog::ShowDirsOnly |
                                                          QFileDialog::DontResolveSymlinks);
    if (!directory.isEmpty())
    {
        _projectDirectoryRoot = directory;
        _ui->lineEditProjectDirectory->setText(_projectDirectoryRoot +
                                               QString("/") +
                                               _ui->lineEditProjectName->text());
    }
}

void ProjectWizard::projectDirectoryTextChanged()
{
}
