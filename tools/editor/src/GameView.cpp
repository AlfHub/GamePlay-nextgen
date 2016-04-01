#include "GameView.h"
#include <QtWidgets>

GameView::GameView(QWidget* parent) :
    QOpenGLWidget(parent),
    _editor(static_cast<EditorWindow*>(parent)),
    _scene(NULL), _wireframe(false)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);
}

GameView::~GameView()
{
    Game::getInstance()->exit();
}

void GameView::resourcePathChanged(const QString& path)
{
    QString fixedPath(path);
    fixedPath.append("/");
    QByteArray ba = fixedPath.toLatin1();
    const char* str = ba.data();
    FileSystem::setResourcePath(str);
}

void GameView::sceneChanged()
{
    _scene = _editor->scene();
}

void GameView::initialize()
{
    // Set the resource path for the editor specific resources.
    // We will copy the this projects res folder and
    // All resource use will be Ex. "res/foo.png"
    FileSystem::setResourcePath("./");

    // Resolve the platform specific absolute asset path
    // This should be the game project path.
    // These resouces are not pat
    //FileSystem::setAssetPath(_editorWindow->getAssetPath());

    Logger::log(Logger::LEVEL_INFO, "GameView initialized.\n");
}

void GameView::finalize()
{
    Logger::log(Logger::LEVEL_INFO, "GameView finalized.\n");
}

void GameView::update(float elapsedTime)
{
    if (_scene)
        _scene->update(elapsedTime);
}

void GameView::render(float elapsedTime)
{
    clear(CLEAR_COLOR_DEPTH, Vector4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

    if (_scene)
        _scene->visit(this, &GameView::drawScene);
}

void GameView::initializeGL()
{
    makeCurrent();
    initializeOpenGLFunctions();
#ifdef GP_USE_GLEW
    glewInit();
#endif
    Game::getInstance()->run();
}

void GameView::resizeGL(int width, int height)
{
    // Update the viewport
    Game::getInstance()->setViewport(gameplay::Rectangle(0, 0, width, height));

    // TODO: Update the camera
}

void GameView::paintGL()
{
    Game::getInstance()->frame();
}

void GameView::mousePressEvent(QMouseEvent* event)
{
    // TODO: Handler here...
}

void GameView::mouseReleaseEvent(QMouseEvent* event)
{
    // TODO: Handler here...
}

void GameView::keyPressEvent(QKeyEvent* event)
{
    // TODO: Handler here...
}

void GameView::keyReleaseEvent(QKeyEvent* event)
{
    // TODO: Handler here...
}

bool GameView::drawScene(Node* node)
{
    Drawable* drawable = node->getDrawable();
    if (drawable)
        drawable->draw(_wireframe);

    return true;
}

