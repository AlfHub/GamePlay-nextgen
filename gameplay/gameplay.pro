#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------
QT -= core gui
TARGET = gameplay
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
CONFIG -= qt
CONFIG(debug, debug|release): DEFINES += _DEBUG

SOURCES += src/AbsoluteLayout.cpp \
    src/AIAgent.cpp \
    src/AIController.cpp \
    src/AIMessage.cpp \
    src/AIState.cpp \
    src/AIStateMachine.cpp \
    src/Animation.cpp \
    src/AnimationClip.cpp \
    src/AnimationController.cpp \
    src/AnimationTarget.cpp \
    src/AnimationValue.cpp \
    src/AudioBuffer.cpp \
    src/AudioController.cpp \
    src/AudioListener.cpp \
    src/AudioSource.cpp \
    src/BoundingBox.cpp \
    src/BoundingBox.inl \
    src/BoundingSphere.cpp \
    src/BoundingSphere.inl \
    src/Bundle.cpp \
    src/Button.cpp \
    src/Camera.cpp \
    src/CheckBox.cpp \
    src/Container.cpp \
    src/Control.cpp \
    src/ControlFactory.cpp \
    src/Curve.cpp \
    src/DepthStencilTarget.cpp \
    src/Drawable.cpp \
    src/Effect.cpp \
    src/FileSystem.cpp \
    src/FlowLayout.cpp \
    src/Font.cpp \
    src/Form.cpp \
    src/FrameBuffer.cpp \
    src/Frustum.cpp \
    src/Game.cpp \
    src/Game.inl \
    src/Gamepad.cpp \
    src/HeightField.cpp \
    src/Image.cpp \
    src/Image.inl \
    src/ImageControl.cpp \
    src/Joint.cpp \
    src/JoystickControl.cpp \
    src/Label.cpp \
    src/Layout.cpp \
    src/Light.cpp \
    src/Logger.cpp \
    src/Material.cpp \
    src/MaterialParameter.cpp \
    src/MathUtil.cpp \
    src/MathUtil.inl \
    src/MathUtilNeon.inl \
    src/Matrix.cpp \
    src/Matrix.inl \
    src/Mesh.cpp \
    src/MeshBatch.cpp \
    src/MeshBatch.inl \
    src/MeshPart.cpp \
    src/MeshSkin.cpp \
    src/Model.cpp \
    src/Node.cpp \
    src/ParticleEmitter.cpp \
    src/Pass.cpp \
    src/PhysicsCharacter.cpp \
    src/PhysicsCollisionObject.cpp \
    src/PhysicsCollisionShape.cpp \
    src/PhysicsConstraint.cpp \
    src/PhysicsConstraint.inl \
    src/PhysicsController.cpp \
    src/PhysicsFixedConstraint.cpp \
    src/PhysicsFixedConstraint.inl \
    src/PhysicsGenericConstraint.cpp \
    src/PhysicsGenericConstraint.inl \
    src/PhysicsGhostObject.cpp \
    src/PhysicsHingeConstraint.cpp \
    src/PhysicsRigidBody.cpp \
    src/PhysicsRigidBody.inl \
    src/PhysicsSocketConstraint.cpp \
    src/PhysicsSpringConstraint.cpp \
    src/PhysicsSpringConstraint.inl \
    src/PhysicsVehicle.cpp \
    src/PhysicsVehicleWheel.cpp \
    src/Plane.cpp \
    src/Plane.inl \
    src/Platform.cpp \
    src/Properties.cpp \
    src/Quaternion.cpp \
    src/Quaternion.inl \
    src/RadioButton.cpp \
    src/Ray.cpp \
    src/Ray.inl \
    src/Rectangle.cpp \
    src/Ref.cpp \
    src/RenderState.cpp \
    src/RenderTarget.cpp \
    src/Scene.cpp \
    src/SceneLoader.cpp \
    src/ScreenDisplayer.cpp \
    src/Script.cpp \
    src/ScriptController.cpp \
    src/ScriptController.inl \
    src/ScriptTarget.cpp \
    src/Slider.cpp \
    src/Sprite.cpp \
    src/SpriteBatch.cpp \
    src/Technique.cpp \
    src/Terrain.cpp \
    src/TerrainPatch.cpp \
    src/Text.cpp \
    src/TextBox.cpp \
    src/Texture.cpp \
    src/Theme.cpp \
    src/ThemeStyle.cpp \
    src/TileSet.cpp \
    src/Transform.cpp \
    src/Vector2.cpp \
    src/Vector2.inl \
    src/Vector3.cpp \
    src/Vector3.inl \
    src/Vector4.cpp \
    src/Vector4.inl \
    src/VertexAttributeBinding.cpp \
    src/VertexFormat.cpp \
    src/VerticalLayout.cpp

HEADERS += src/AbsoluteLayout.h \
    src/AIAgent.h \
    src/AIController.h \
    src/AIMessage.h \
    src/AIState.h \
    src/AIStateMachine.h \
    src/Animation.h \
    src/AnimationClip.h \
    src/AnimationController.h \
    src/AnimationTarget.h \
    src/AnimationValue.h \
    src/AudioBuffer.h \
    src/AudioController.h \
    src/AudioListener.h \
    src/AudioSource.h \
    src/Base.h \
    src/BoundingBox.h \
    src/BoundingSphere.h \
    src/Bundle.h \
    src/Button.h \
    src/Camera.h \
    src/CheckBox.h \
    src/Container.h \
    src/Control.h \
    src/ControlFactory.h \
    src/Curve.h \
    src/DepthStencilTarget.h \
    src/Drawable.h \
    src/Effect.h \
    src/FileSystem.h \
    src/FlowLayout.h \
    src/Font.h \
    src/Form.h \
    src/FrameBuffer.h \
    src/Frustum.h \
    src/Game.h \
    src/Gamepad.h \
    src/gameplay.h \
    src/Gesture.h \
    src/HeightField.h \
    src/Image.h \
    src/ImageControl.h \
    src/Joint.h \
    src/JoystickControl.h \
    src/Keyboard.h \
    src/Label.h \
    src/Layout.h \
    src/Light.h \
    src/Logger.h \
    src/Material.h \
    src/MaterialParameter.h \
    src/MathUtil.h \
    src/Matrix.h \
    src/Mesh.h \
    src/MeshBatch.h \
    src/MeshPart.h \
    src/MeshSkin.h \
    src/Model.h \
    src/Mouse.h \
    src/Node.h \
    src/ParticleEmitter.h \
    src/Pass.h \
    src/PhysicsCharacter.h \
    src/PhysicsCollisionObject.h \
    src/PhysicsCollisionShape.h \
    src/PhysicsConstraint.h \
    src/PhysicsController.h \
    src/PhysicsFixedConstraint.h \
    src/PhysicsGenericConstraint.h \
    src/PhysicsGhostObject.h \
    src/PhysicsHingeConstraint.h \
    src/PhysicsRigidBody.h \
    src/PhysicsSocketConstraint.h \
    src/PhysicsSpringConstraint.h \
    src/PhysicsVehicle.h \
    src/PhysicsVehicleWheel.h \
    src/Plane.h \
    src/Platform.h \
    src/Properties.h \
    src/Quaternion.h \
    src/RadioButton.h \
    src/Ray.h \
    src/Rectangle.h \
    src/Ref.h \
    src/RenderState.h \
    src/RenderTarget.h \
    src/Scene.h \
    src/SceneLoader.h \
    src/ScreenDisplayer.h \
    src/Script.h \
    src/ScriptController.h \
    src/ScriptTarget.h \
    src/Slider.h \
    src/Sprite.h \
    src/SpriteBatch.h \
    src/Stream.h \
    src/Technique.h \
    src/Terrain.h \
    src/TerrainPatch.h \
    src/Text.h \
    src/TextBox.h \
    src/Texture.h \
    src/Theme.h \
    src/ThemeStyle.h \
    src/TileSet.h \
    src/TimeListener.h \
    src/Touch.h \
    src/Transform.h \
    src/Vector2.h \
    src/Vector3.h \
    src/Vector4.h \
    src/VertexAttributeBinding.h \
    src/VertexFormat.h \
    src/VerticalLayout.h

INCLUDEPATH += $$PWD/../gameplay/src
INCLUDEPATH += $$PWD/../external-deps/include
DEFINES += GP_USE_GAMEPAD

linux: SOURCES += src/PlatformLinux.cpp
linux: SOURCES += src/main-linux.cpp
linux: QMAKE_CXXFLAGS += -lstdc++ -pthread -w
linux: DEFINES += __linux__
linux: INCLUDEPATH += /usr/include/gtk-2.0
linux: INCLUDEPATH += /usr/lib/x86_64-linux-gnu/gtk-2.0/include
linux: INCLUDEPATH += /usr/include/atk-1.0
linux: INCLUDEPATH += /usr/include/cairo
linux: INCLUDEPATH += /usr/include/gdk-pixbuf-2.0
linux: INCLUDEPATH += /usr/include/pango-1.0
linux: INCLUDEPATH += /usr/include/gio-unix-2.0
linux: INCLUDEPATH += /usr/include/freetype2
linux: INCLUDEPATH += /usr/include/glib-2.0
linux: INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include
linux: INCLUDEPATH += /usr/include/pixman-1
linux: INCLUDEPATH += /usr/include/libpng12
linux: INCLUDEPATH += /usr/include/harfbuzz

macx: OBJECTIVE_SOURCES += src/PlatformMacOSX.mm
macx: OBJECTIVE_SOURCES += src/main-macosx.mm
macx: QMAKE_CXXFLAGS += -x c++ -x objective-c++ -stdlib=libc++ -w -arch x86_64
macx: LIBS += -F/System/Library/Frameworks -framework GameKit
macx: LIBS += -F/System/Library/Frameworks -framework IOKit
macx: LIBS += -F/System/Library/Frameworks -framework QuartzCore
macx: LIBS += -F/System/Library/Frameworks -framework OpenAL
macx: LIBS += -F/System/Library/Frameworks -framework OpenGL
macx: LIBS += -F/System/Library/Frameworks -framework Cocoa
macx: LIBS += -F/System/Library/Frameworks -framework Foundation

win32: SOURCES += src/PlatformWindows.cpp
win32: SOURCES += src/main-windows.cpp
win32: DEFINES += WIN32 _UNICODE UNICODE
win32: INCLUDEPATH += $$(DXSDK_DIR)/Include
win32: QMAKE_CXXFLAGS_WARN_ON -= -w34100
win32: QMAKE_CXXFLAGS_WARN_ON -= -w34189
win32: QMAKE_CXXFLAGS_WARN_ON -= -w4302
