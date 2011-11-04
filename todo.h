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
     * Hide to tray or save setting when closing.
     */
    void closeEvent(QCloseEvent *event);

    /**
     * Minimized window to tray.
     */
    void changeEvent(QEvent *event);

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

    /**
     * Create context menu.
     */
    void createContextMenu();

    /** Data structur for notes. */
    struct note
    {
      QDate date;
      QTime time;
      int color;
      QString content;
    };

    QSystemTrayIcon *trayIcon; /**< Tray icon. */
    Ui::ToDo *ui; /**< Main window. */
    QVector<struct note> data; /**< Data summary. */
    QVector<QColor> colors; /**< Colors. */
    QString dateFormat; /**< Date format. */
    QString timeFormat; /**< Time format. */
    bool decorations; /**< Showing decorations. */
    QSettings settings; /**< Application settings. */
    QMenu *contextMenu; /**< Context menu. */

  private slots:

    /**
     * Process changed data.
     */
    void reload();

    /**
     * Show context menu.
     */
    void mainMenu(const QPoint &);

    /**
     * Open settings window.
     */
    void settingsDialog();

    /**
     * Action in tray.
     */
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // TODO_H
