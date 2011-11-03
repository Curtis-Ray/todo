#include "todo.h"
#include "ui_todo.h"
#include "ui_settings.h"

ToDo::ToDo(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::ToDo)
{
  // Load UI from QT UI file.
  ui->setupUi(this);

  // Context menu after right click.
  connect(ui->diaryTextEdit, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(mainMenu(const QPoint &)));
  connect(ui->notesTextEdit, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(mainMenu(const QPoint &)));
  connect(ui->calendarWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(mainMenu(const QPoint &)));
  // Text edits refreshing, after changes.
  connect(ui->diaryTextEdit, SIGNAL(textChanged()), this, SLOT(reload()));
  connect(ui->notesTextEdit, SIGNAL(textChanged()), this, SLOT(reload()));

  loadConfig();
  loadData();

  // Show all informations in calendar and textedits.
  emit reload();
}

ToDo::~ToDo()
{
  delete ui;
}

void ToDo::loadConfig()
{

}

void ToDo::loadData()
{

}

void ToDo::saveConfig()
{

}

void ToDo::saveData()
{

}

void ToDo::parse(QString text)
{

}

void ToDo::display()
{

}

void ToDo::reload()
{
  // Merge both textEdits.
  QString merged= ui->diaryTextEdit->toHtml() + ui->diaryTextEdit->toHtml();

  // Parse text and save it to our data structures.
  parse(merged);
  // Display changes in textArea.
  display();
}

void ToDo::mainMenu(const QPoint &)
{
  QMenu* contextMenu = new QMenu(this);
  Q_CHECK_PTR (contextMenu);

  contextMenu->addAction("Settings", this, SLOT (settings()));
  contextMenu->popup(QCursor::pos());
  contextMenu->exec();
  delete contextMenu;
  contextMenu = 0;
}

void ToDo::settings()
{
  // Open new window ui.
  Ui::Settings ui;
  QDialog *dialog = new QDialog;

  ui.setupUi(dialog);
  dialog->exec();
  delete dialog;
  dialog = 0;
}
