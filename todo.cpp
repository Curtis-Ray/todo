#include "todo.h"
#include "ui_todo.h"

ToDo::ToDo(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::ToDo)
{
  ui->setupUi(this);

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

void ToDo::reload()
{

}
