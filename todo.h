#ifndef TODO_H
#define TODO_H

#include <QtCore>
#include <QtGui>

namespace Ui
{
  class ToDo;
}

class ToDo : public QMainWindow
{
  Q_OBJECT

  public:

    explicit ToDo(QWidget *parent = 0);

    ~ToDo();

  private:

    /**
     * Load configuration from file.
     */
    void loadConfig();

    /**
     * Load data from file.
     */
    void loadData();

    /** Data structur for notes. */
    struct note
    {
      QDate date;
      QTime time;
      QColor color;
      QString content;
    };

    Ui::ToDo *ui; /**< Main window. */
    QVector<struct note> data; /**< Data summary. */
    QVector<QColor> colors; /**< Colors. */
    Qt::DateFormat dateFormat; /**< Date format. */
    Qt::DateFormat timeFormat; /**< Time format. */
    bool tray; /**< Using tray. */
    bool decorations; /**< Showing decorations. */

  private slots:

    /**
     * Process changed data.
     */
    void reload();

};

#endif // TODO_H
