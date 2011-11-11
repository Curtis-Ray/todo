#ifndef TODO_H
#define TODO_H

#include <QtCore>
#include <QtGui>
#include <qtcolorpicker.h>
#include "ui_todo.h"
#include "ui_settings.h"

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

    /** Data structur for notes. */
    struct note
    {
      QDate date;
      QTime time;
      int color;
      QString content;
    };

    /**
     * Compare data structures for sorting.
     */
    static bool dataLessThan(const struct note &d1, const struct note &d2);

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

    QSystemTrayIcon *trayIcon; /**< Tray icon. */
    Ui::ToDo *ui; /**< Main window. */
    QVector<struct note> data; /**< Data summary. */
    QVector<QColor> colors; /**< Colors. */
    QString dateFormat; /**< Date format. */
    QString timeFormat; /**< Time format. */
    QSettings settings; /**< Application settings. */
    QMenu *contextMenu; /**< Context menu. */
    QTimer *inactivity; /**< Inactivity timer. */

  private slots:

    /**
     * User is active - reseting inactivity timer.
     */
    void userActive();
    /**
     * Process changed data.
     */
    void reload();

    /**
     * Save configuration to file.
     */
    void saveConfig();

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

    /**
     * Handle message from another instance and show window.
     */
    void handleMessage(const QString &);
};

#endif // TODO_H
