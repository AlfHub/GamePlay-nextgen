#include "SceneView.h"
#include "ui_SceneView.h"

SceneView::SceneView(QWidget* parent)
    : QWidget(parent), _ui(new Ui::SceneView),
      _editor(static_cast<EditorWindow*>(parent)),
      _scene(NULL)
{
    _ui->setupUi(this);
}

SceneView::~SceneView()
{
    delete _ui;
}

void SceneView::sceneChanged()
{
    _scene = _editor->scene();

    // Update the scene tree view
}
