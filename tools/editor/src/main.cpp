#include "ProjectWizard.h"
#include "EditorWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ProjectWizard wizard;
    EditorWindow editor;

    if (wizard.initialize() )
    {
        wizard.setEditor(&editor);
        editor.setProjectWizard(&wizard);
        wizard.show();

        return app.exec();
    }
    else
    {
        return EXIT_FAILURE;
    }
}
