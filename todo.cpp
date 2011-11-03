#include "todo.h"
#include "ui_todo.h"

ToDo::ToDo(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::ToDo)
{
  // Load UI from QT UI file.
  ui->setupUi(this);

  // Connect signals and slots.
  connect(ui->diaryTextEdit, SIGNAL(textChanged()), this, SLOT(reload()));
  connect(ui->notesTextEdit, SIGNAL(textChanged()), this, SLOT(reload()));

  loadConfig();
  loadData();

  // Show all informations in calendar and textedits.
  emit(reload);
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

void ToDo::Parse()
{

}

void ToDo::Display()
{

}

void ToDo::reload()
{
  // Merge both textEdits.
  QString merged= ui->diaryTextEdit + ui->diaryTextEdit;

  // Parse text and save it to our data structures.
  ToDo.Parse(merged);
  // Display changes in textArea.
  ToDo.Display();
}
