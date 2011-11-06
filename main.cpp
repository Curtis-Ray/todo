#include <QtGui>
#include <QtCore>
#include <qtsingleapplication.h>
#include "todo.h"

int main(int argc, char *argv[])
{
  QtSingleApplication app(argc, argv);
  if (app.sendMessage("Wake up!"))
  { // Another instance running.
    return EXIT_SUCCESS;
  }

  // Set names for configuration file.
  app.setOrganizationName("ToDo");
  app.setApplicationName("ToDo");

  // Run main window.
  ToDo todo;
  todo.show();

  // Handle incomming message to get focus.
  app.setActivationWindow(&todo);
  QObject::connect(&app, SIGNAL(messageReceived(const QString&)),
                   &todo, SLOT(handleMessage(const QString&)));

  return app.exec();
}
