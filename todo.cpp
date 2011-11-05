#include "todo.h"
#include "ui_todo.h"
#include "ui_settings.h"

ToDo::ToDo(QWidget *parent)
  : QMainWindow(parent), trayIcon(new QSystemTrayIcon(this)), ui(new Ui::ToDo),
    contextMenu(new QMenu(this)), colors(3), data(5)
{
  // Load UI from QT UI file.
  ui->setupUi(this);

  // Create context menu,
  createContextMenu();

  // Load resources.
  QResource::registerResource("/icons.qrc");

  // Set tray icon properties.
  trayIcon->setContextMenu(contextMenu);
  trayIcon->setIcon(QIcon(":/icon"));
  setWindowIcon(QIcon(":/icon"));

  // Load configuration from file.
  loadConfig();

  colors[0] = QColor(255,0,0);
  colors[1] = QColor(0,255,0);
  colors[2] = QColor(0,0,255);
  data[0].color = 0;
  data[0].content = "test barva c.1";
  data[0].date = QDate(2011,9,22);
  data[0].time = QTime(22,18);
  data[1].color = 1;
  data[1].content = "test barva c.2";
  data[1].date = QDate(2011,9,25);
  data[1].time = QTime(9,33);
  data[2].color = 0;
  data[2].content = "test barva c.1 verze 2";
  data[2].date = QDate(2011,10,1);
  data[2].time = QTime(15,45);
  data[3].color = 0;
  data[3].content = "test barva c.1 - poznamka bez data";
  data[3].date = QDate();
  data[3].time = QTime();
  data[4].color = 2;
  data[4].content = "test barva c.3";
  data[4].date = QDate(2011,12,12);
  data[4].time = QTime();

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

  // Tray icon action.
  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

  // Show all informations in calendar and textedits.
  emit reload();
}

ToDo::~ToDo()
{
  delete ui;
  delete contextMenu;
  delete trayIcon;
}

void ToDo::createContextMenu()
{
  contextMenu->addAction(QString::fromUtf8("Nastavení"), this, SLOT(settingsDialog()));
  contextMenu->addAction(QString::fromUtf8("Ukončit program"), qApp, SLOT(quit()));
}

void ToDo::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
  switch (reason)
  {
    // Click or double click.
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::MiddleClick:
      if(this->isVisible())
      { // Hide open window.
        this->hide();
      }
      else
      { // Show minimized window.
        this->show();
      }
      break;
    default:
      break;
  }
}

void ToDo::closeEvent(QCloseEvent *event)
{
  if (trayIcon->isVisible())
  { // Hide in tray.
    hide();

    event->ignore();
  }
  else
  { // Save config and close.
    saveConfig();

    QMainWindow::closeEvent(event);
  }
}

void ToDo::changeEvent(QEvent *event)
{
  QMainWindow::changeEvent(event);
  if(event->type() == QEvent::WindowStateChange)
  {
    if(isMinimized() && trayIcon->isVisible())
    { // Minimized window hide to tray.
      this->hide();
    }
  }
}

void ToDo::loadConfig()
{
  // Load window setting.
  if (settings.value("general/tray", true).value<bool>())
  {
    trayIcon->show();
  }
  decorations = settings.value("general/decorations", false).value<bool>();
  resize(settings.value("general/size", QSize(250, 550)).value<QSize>());
  move(settings.value("general/position", QPoint(0, 0)).value<QPoint>());

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
  // Save window setting.
  settings.beginGroup("general");
  settings.setValue("tray", trayIcon->isVisible());
  settings.setValue("decorations", decorations);
  settings.setValue("position", pos());
  settings.setValue("size", size());
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
  // Actual process row.
  QString row("");
  // Create cursor for diary textEdit.
  QTextCursor *cursor = new QTextCursor(ui->diaryTextEdit->document());
  // Text coloring.
  QTextCharFormat format;

  // Move cursor to start.
  cursor->setPosition(0);
  while(!cursor->atEnd())
  {
    // Select line.
    cursor->select(QTextCursor::LineUnderCursor);
    format = cursor->charFormat();
    // Save current line.
    if(format.foreground().color() == Qt::green)
      row += cursor->selectedText();
    cursor->movePosition(QTextCursor::NextBlock);
  }

  // Show in second textEdit.
  ui->notesTextEdit->setPlainText(row);

  //format.setForeground(QBrush(QColor(Qt::red)));
  //cursor->setCharFormat(format);
  //------QTextCharFormat::colorProperty()

  delete cursor;
  cursor = NULL;
}

void ToDo::display()
{
  // Delete all from textEdits.
  ui->diaryTextEdit->setPlainText("");
  ui->notesTextEdit->setPlainText("");

  // Create cursors - one for diary, one for notes.
  QTextCursor *cursorDiary = new QTextCursor(ui->diaryTextEdit->document());
  QTextCursor *cursorNotes = new QTextCursor(ui->notesTextEdit->document());
  // Text coloring.
  QTextCharFormat format;
  // Temp variable.
  QString temp;

  // Move cursors to start.
  cursorDiary->setPosition(0);
  cursorNotes->setPosition(0);

  foreach(struct note row, data)
  { // Check each note, set properties, and add to textEdit.
    if(!row.date.isNull())
    { // It's diary.
      // Set color of row.
      format.setForeground(QBrush(colors[row.color]));
      // Create row from date, time and content.
      temp = row.date.toString("dd.MM.yyyy") + " ";
      temp += (!row.time.isNull()) ? row.time.toString("hh:mm") + " " : " ";
      temp += row.content + "\n";
      // Insert it.
      cursorDiary->insertText(temp, format);
    }
    else
    { // It's note.
      // Set color of row.
      format.setForeground(QBrush(colors[row.color]));
      // Create row only from content.
      temp = row.content + "\n";
      // Insert it.
      cursorNotes->insertText(temp, format);
    }
  }
}

void ToDo::reload()
{
  // Disable signals to avoid recursion.
  disconnect(ui->notesTextEdit, SIGNAL(textChanged()), this, SLOT(reload()));
  disconnect(ui->diaryTextEdit, SIGNAL(textChanged()), this, SLOT(reload()));

  // Parse text and save it to our data structures.
  //parse();
  // Display changes in textArea.
  display();

  // Reenable signals.
  connect(ui->notesTextEdit, SIGNAL(textChanged()), this, SLOT(reload()));
  connect(ui->diaryTextEdit, SIGNAL(textChanged()), this, SLOT(reload()));
}

void ToDo::mainMenu(const QPoint &)
{
  // Show menu in point.
  contextMenu->popup(QCursor::pos());
  contextMenu->exec();
}

void ToDo::settingsDialog()
{
  // Open new window ui.
  Ui::Settings ui;
  QDialog *dialog = new QDialog;

  // Show window.
  ui.setupUi(dialog);
  ////// TODO: load settings
  dialog->exec();

  if (dialog->result() == QDialog::Accepted)
  { // Apply changes.

    ////// TODO: apply changes from form

    saveConfig();
    emit reload();
  }

  delete dialog;
  dialog = NULL;
}
