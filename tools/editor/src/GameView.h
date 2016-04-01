#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "gameplay.h"
using namespace gameplay;

#include "EditorWindow.h"
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QKeyEvent>

/**
 * The main game view use to intialize, update and render game scenes.
 */
class GameView : public QOpenGLWidget, protected QOpenGLFunctions, public Game
{
    Q_OBJECT
public:
    /**
     * Constructor
     * @param parent The parent widget.
     */
    explicit GameView(QWidget* parent = 0);

    /**
     * Destructor.
     */
    ~GameView();

public slots:
    /**
     * Slots handler for resource path changing.
     * @param path The new resource path.
     */
    void resourcePathChanged(const QString& path);

    /**
     * Slot handler for scene changing.
     */
    void sceneChanged();

protected:
    /**
     * @see Game::initialize
     */
    void initialize();

    /**
     * @see Game::finalize
     */
    void finalize();

    /**
     * @see Game::update
     */
    void update(float elapsedTime);

    /**
     * @see Game::render
     */
    void render(float elapsedTime);
    
    /**
     * @see QOpenGLWidget::initializeGL
     */
    void initializeGL() Q_DECL_OVERRIDE;

    /**
     * @see QOpenGLWidget::resizeGL
     */
    void resizeGL(int width, int height) Q_DECL_OVERRIDE;

    /**
     * @see QOpenGLWidget::paintGL
     */
    void paintGL() Q_DECL_OVERRIDE;

    /**
     * @see QWidget::mousePressEvent
     */
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    /**
     * @see QWidget::mouseReleaseEvent
     */
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

    /**
     * @see QWidget::keyPressEvent
     */
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

    /**
     * @see QWidget::keyReleaseEvent
     */
    void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

    /**
     * Draws the active scene.
     */
    bool drawScene(Node* node);

private:
    EditorWindow* _editor;
    Scene* _scene;
    bool _wireframe;
};

#endif
