#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include "gameplay.h"
using namespace gameplay;

#include "EditorWindow.h"
#include <QWidget>

namespace Ui {
class SceneView;
}

class SceneView : public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param parent The parent widget.
     */
    explicit SceneView(QWidget* parent = 0);

    /**
     * Destructor.
     */
    ~SceneView();

public slots:
    /**
     * Handler for scene changing.
     */
    void sceneChanged();
    
private:
    Ui::SceneView* _ui;
    EditorWindow* _editor;
    Scene* _scene;
};

#endif
