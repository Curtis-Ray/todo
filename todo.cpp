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
  // Show all informations in calendar and textedits.
  emit reload();
}

ToDo::~ToDo()
{
  delete ui;
}

void ToDo::loadConfig()
{
  // Load tray and decorations setting.
  tray = settings.value("general/tray").value<bool>();
  decorations = settings.value("general/decorations").value<bool>();

  // Load format date and time.
  dateFormat = settings.value("format/date").value<QString>();
  timeFormat = settings.value("format/time").value<QString>();

  int size = settings.beginReadArray("colors");
  for (int i = 0; i < size; ++i)
  { // Load colors vector.
    settings.setArrayIndex(i);
    colors[i] = settings.value("color").value<QColor>();
  }
  settings.endArray();

  size = settings.beginReadArray("data");
  for (int i = 0; i < size; ++i)
  { // Load all data.
    settings.setArrayIndex(i);
    data[i].date = settings.value("color").value<QDate>();
    data[i].time = settings.value("color").value<QTime>();
    data[i].color = settings.value("color").value<int>();
    data[i].content = settings.value("color").value<QString>();
  }
  settings.endArray();

}

void ToDo::saveConfig()
{
  // Save tray and decorations setting.
  settings.beginGroup("general");
  settings.setValue("tray", tray);
  settings.setValue("decorations", decorations);
  settings.endGroup();

  // Save format date and time.
  settings.beginGroup("formats");
  settings.setValue("date", dateFormat);
  settings.setValue("time", timeFormat);
  settings.endGroup();

  settings.beginWriteArray("colors");
  for (int i = 0; i < colors.size(); ++i)
  { // Save colors vector.
    settings.setArrayIndex(i);
    settings.setValue("color", colors.at(i));
  }
  settings.endArray();

  settings.beginWriteArray("data");
  for (int i = 0; i < data.size(); ++i)
  { // Save all data.
    settings.setArrayIndex(i);
    settings.setValue("date", data.at(i).date);
    settings.setValue("time", data.at(i).time);
    settings.setValue("color", data.at(i).color);
    settings.setValue("content", data.at(i).content);
  }
  settings.endArray();
}

void ToDo::parse()
{
  QString row;
  // Create cursor for diary textEdit.
  QTextCursor *cursor = new QTextCursor(ui->diaryTextEdit->document());
  // Move cursor to start
  cursor->setPosition(0);
}

void ToDo::display()
{

}

void ToDo::reload()
{
  // Disable signals to avoid recursion.
  disconnect(ui->notesTextEdit, SIGNAL(textChanged()), this, SLOT(reload()));
  disconnect(ui->diaryTextEdit, SIGNAL(textChanged()), this, SLOT(reload()));

  // Parse text and save it to our data structures.
  parse();
  // Display changes in textArea.
  display();

  // Reenable signals.
  connect(ui->notesTextEdit, SIGNAL(textChanged()), this, SLOT(reload()));
  connect(ui->diaryTextEdit, SIGNAL(textChanged()), this, SLOT(reload()));
}

void ToDo::mainMenu(const QPoint &)
{
  QMenu* contextMenu = new QMenu(this);

  contextMenu->addAction("Settings", this, SLOT (settingsDialog()));
  contextMenu->popup(QCursor::pos());
  contextMenu->exec();
  delete contextMenu;
  contextMenu = 0;
}

void ToDo::settingsDialog()
{
  // Open new window ui.
  Ui::Settings ui;
  QDialog *dialog = new QDialog;

  ui.setupUi(dialog);
  dialog->exec();
  delete dialog;
  dialog = 0;
}
