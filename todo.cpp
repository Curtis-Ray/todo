#include "todo.h"

ToDo::ToDo(QWidget *parent)
  : QMainWindow(parent), trayIcon(new QSystemTrayIcon(this)), ui(new Ui::ToDo),
    contextMenu(new QMenu(this))
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

  // Save config when quit.
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(saveConfig()));

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

void ToDo::handleMessage(const QString &)
{
  // Get focus when message come.
  hide();
  show();
}

void ToDo::mainMenu(const QPoint &)
{
  // Show menu in point.
  contextMenu->popup(QCursor::pos());
  contextMenu->exec();
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
  else
  {
    trayIcon->hide();
  }
  resize(settings.value("general/size").value<QSize>());
  move(settings.value("general/position").value<QPoint>());
  ui->splitter->restoreState(settings.value("general/splitter").value<QByteArray>());
  if (settings.value("general/frameless").value<bool>())
  {
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->show();
  }
  else
  {
    setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
    this->show();
  }
  setWindowOpacity(settings.value("general/opacity").value<double>());
  QPalette pal = palette();
  pal.setColor(QPalette::Window, settings.value("general/background").value<QColor>());
  pal.setColor(QPalette::Base, settings.value("general/background").value<QColor>());
  pal.setColor(QPalette::WindowText, settings.value("general/foreground").value<QColor>());
  ui->notesTextEdit->setPalette(pal);
  ui->diaryTextEdit->setPalette(pal);
  ui->calendarWidget->setPalette(pal);
  setFont(settings.value("general/font").value<QFont>());

  // Load format date and time.
  dateFormat = settings.value("format/date").value<QString>();
  timeFormat = settings.value("format/time").value<QString>();

  int size = settings.beginReadArray("colors");
  colors.clear();
  for (int i = 0; i < size; ++i)
  { // Load colors vector.
    settings.setArrayIndex(i);
    colors.insert(i, settings.value("color").value<QColor>());
  }
  settings.endArray();

  size = settings.beginReadArray("data");
  struct note temp;
  data.clear();
  for (int i = 0; i < size; ++i)
  { // Load all data.
    settings.setArrayIndex(i);
    temp.date = settings.value("date").value<QDate>();
    temp.time = settings.value("time").value<QTime>();
    temp.color = settings.value("color").value<int>();
    temp.content = settings.value("content").value<QString>();
    data.insert(i, temp);
  }
  settings.endArray();
}

void ToDo::saveConfig()
{
  // Save window setting.
  settings.beginGroup("general");
  settings.setValue("tray", trayIcon->isVisible());
  settings.setValue("position", pos());
  settings.setValue("size", size());
  settings.setValue("splitter", ui->splitter->saveState());
  settings.setValue("frameless", 0 != (windowFlags() & Qt::FramelessWindowHint));
  settings.setValue("opacity", windowOpacity());
  settings.setValue("foreground", ui->notesTextEdit->palette().color(QPalette::WindowText));
  settings.setValue("background", ui->notesTextEdit->palette().color(QPalette::Window));
  settings.setValue("font", font());
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

void ToDo::settingsDialog()
{
  // Open new window ui.
  Ui::Settings ui;
  QDialog *dialog = new QDialog;

  // Show window.
  ui.setupUi(dialog);

  // Save values before changes.
  saveConfig();

  ////// TODO: load settings

  ui.trayCheckBox->setChecked(settings.value("general/tray").value<bool>());
  ui.decorationCheckBox->setChecked(settings.value("general/frameless").value<bool>());
  ui.opacitySlider->setValue(settings.value("general/opacity").value<double>() * 100);
  ui.color1Button->setCurrentColor(settings.value("colors/1/color").value<QColor>());
  ui.color1Button->setStandardColors();
  ui.color2Button->setCurrentColor(settings.value("colors/2/color").value<QColor>());
  ui.color2Button->setStandardColors();
  ui.color3Button->setCurrentColor(settings.value("colors/3/color").value<QColor>());
  ui.color3Button->setStandardColors();
  ui.color4Button->setCurrentColor(settings.value("colors/4/color").value<QColor>());
  ui.color4Button->setStandardColors();
  ui.color5Button->setCurrentColor(settings.value("colors/5/color").value<QColor>());
  ui.color5Button->setStandardColors();
  ui.color6Button->setCurrentColor(settings.value("colors/6/color").value<QColor>());
  ui.color6Button->setStandardColors();
  ui.color7Button->setCurrentColor(settings.value("colors/7/color").value<QColor>());
  ui.color7Button->setStandardColors();
  ui.color8Button->setCurrentColor(settings.value("colors/8/color").value<QColor>());
  ui.color8Button->setStandardColors();
  ui.foregroundButton->setCurrentColor(settings.value("general/foreground").value<QColor>());
  ui.foregroundButton->setStandardColors();
  ui.backgroundButton->setCurrentColor(settings.value("general/background").value<QColor>());
  ui.backgroundButton->setStandardColors();
  ui.fontComboBox->setCurrentFont(settings.value("general/font").value<QFont>());

  dialog->exec();

  if (dialog->result() == QDialog::Accepted)
  { // Apply changes.

    ////// TODO: apply changes from form

    // Save changes.
    settings.setValue("general/tray", ui.trayCheckBox->isChecked());
    settings.setValue("general/frameless", ui.decorationCheckBox->isChecked());
    settings.setValue("general/opacity", static_cast<double>(ui.opacitySlider->value()) / 100);
    settings.setValue("colors/1/color", ui.color1Button->currentColor());
    settings.setValue("colors/2/color", ui.color2Button->currentColor());
    settings.setValue("colors/3/color", ui.color3Button->currentColor());
    settings.setValue("colors/4/color", ui.color4Button->currentColor());
    settings.setValue("colors/5/color", ui.color5Button->currentColor());
    settings.setValue("colors/6/color", ui.color6Button->currentColor());
    settings.setValue("colors/7/color", ui.color7Button->currentColor());
    settings.setValue("colors/8/color", ui.color8Button->currentColor());
    settings.setValue("general/foreground", ui.foregroundButton->currentColor());
    settings.setValue("general/background", ui.backgroundButton->currentColor());
    settings.setValue("general/font", ui.fontComboBox->currentFont());

    // Apply changes.
    loadConfig();

    emit reload();
  }

  delete dialog;
  dialog = NULL;
}
