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
     * Save configuration to file.
     */
    void saveConfig();

    /**
     * Parse text to our data structures.
     */
    void parse();

    /**
     * Display our data structures in textEdits and calendar.
     */
    void display();

    /** Data structur for notes. */
    struct note
    {
      QDate date;
      QTime time;
      int color;
      QString content;
    };

    Ui::ToDo *ui; /**< Main window. */
    QVector<struct note> data; /**< Data summary. */
    QVector<QColor> colors; /**< Colors. */
    QString dateFormat; /**< Date format. */
    QString timeFormat; /**< Time format. */
    bool tray; /**< Using tray. */
    bool decorations; /**< Showing decorations. */
    QSettings settings; /**< Application settings. */

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
    void settingsDialog();

};

#endif // TODO_H
