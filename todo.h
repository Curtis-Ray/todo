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
     * Load configuration and last session properties from file.
     */
    void loadConfig();

    /**
     * Load data from file.
     */
    void loadData();

    /**
     * Save configuration to file.
     */
    void saveConfig();

    /**
     * Save data to file.
     */
    void saveData();

    /**
     * Parse text to our data structures.
     */
    void parse(QString text);

    /**
     * Display our data structures in textEdits and calendar.
     */
    void display();

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

    /**
     * Main context menu.
     */
    void mainMenu(const QPoint &);

    /**
     * Open settings window.
     */
    void settings();

};

#endif // TODO_H
