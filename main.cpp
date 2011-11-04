#include <QtGui/QApplication>
#include "todo.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("ToDo");
    app.setApplicationName("ToDo");

    ToDo todo;
    todo.show();

    return app.exec();
}
