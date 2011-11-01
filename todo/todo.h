#ifndef TODO_H
#define TODO_H

#include <QMainWindow>

namespace Ui {
    class ToDo;
}

class ToDo : public QMainWindow
{
    Q_OBJECT

public:
    explicit ToDo(QWidget *parent = 0);
    ~ToDo();

private:
    Ui::ToDo *ui;
};

#endif // TODO_H
