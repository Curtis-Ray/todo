#include "todo.h"

ToDo::ToDo(QWidget *parent)
  : QMainWindow(parent), trayIcon(new QSystemTrayIcon(this)), ui(new Ui::ToDo),
    contextMenu(new QMenu(this)), inactivity(new QTimer(this))
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

  // Text edits refreshing inactive timer.
  connect(ui->diaryTextEdit, SIGNAL(textChanged()), this, SLOT(userActive()));
  connect(ui->notesTextEdit, SIGNAL(textChanged()), this, SLOT(userActive()));

  // Inactive timer expired.
  connect(inactivity, SIGNAL(timeout()), this, SLOT(reload()));

  // Filters.
  connect(ui->color1Widget, SIGNAL(stateChanged()), this, SLOT(filtersChanged()));
  connect(ui->color2Widget, SIGNAL(stateChanged()), this, SLOT(filtersChanged()));
  connect(ui->color3Widget, SIGNAL(stateChanged()), this, SLOT(filtersChanged()));
  connect(ui->color4Widget, SIGNAL(stateChanged()), this, SLOT(filtersChanged()));
  connect(ui->color5Widget, SIGNAL(stateChanged()), this, SLOT(filtersChanged()));
  connect(ui->color6Widget, SIGNAL(stateChanged()), this, SLOT(filtersChanged()));
  connect(ui->color7Widget, SIGNAL(stateChanged()), this, SLOT(filtersChanged()));
  connect(ui->color8Widget, SIGNAL(stateChanged()), this, SLOT(filtersChanged()));

  // Save config when quit.
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(saveConfig()));

  // Tray icon action.
  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
          this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

  // Enable key sequences.
  grabShortcut(QKeySequence("Ctrl+1"));
  grabShortcut(QKeySequence("Ctrl+2"));
  grabShortcut(QKeySequence("Ctrl+3"));
  grabShortcut(QKeySequence("Ctrl+4"));
  grabShortcut(QKeySequence("Ctrl+5"));
  grabShortcut(QKeySequence("Ctrl+6"));
  grabShortcut(QKeySequence("Ctrl+7"));
  grabShortcut(QKeySequence("Ctrl+8"));

  // Show all informations in calendar and textedits.
  emit display();
}

ToDo::~ToDo()
{
  delete ui;
  delete contextMenu;
  delete trayIcon;
}

bool ToDo::event(QEvent *event)
{
  if (event->type() == QEvent::Shortcut)
  {
    QShortcutEvent *shortcut = (QShortcutEvent *)event;
    if (shortcut->key() == QKeySequence("Ctrl+1"))
    {
      ui->color1Widget->toggleState();
      return true;
    }
    else if (shortcut->key() == QKeySequence("Ctrl+2"))
    {
      ui->color2Widget->toggleState();
      return true;
    }
    else if (shortcut->key() == QKeySequence("Ctrl+3"))
    {
      ui->color3Widget->toggleState();
      return true;
    }
    else if (shortcut->key() == QKeySequence("Ctrl+4"))
    {
      ui->color4Widget->toggleState();
      return true;
    }
    else if (shortcut->key() == QKeySequence("Ctrl+5"))
    {
      ui->color5Widget->toggleState();
      return true;
    }
    else if (shortcut->key() == QKeySequence("Ctrl+6"))
    {
      ui->color6Widget->toggleState();
      return true;
    }
    else if (shortcut->key() == QKeySequence("Ctrl+7"))
    {
      ui->color7Widget->toggleState();
      return true;
    }
    else if (shortcut->key() == QKeySequence("Ctrl+8"))
    {
      ui->color8Widget->toggleState();
      return true;
    }
  }

  return QWidget::event(event);
}

bool ToDo::dataLessThan(const struct note &d1, const struct note &d2)
{
  if (d1.date.isNull() || d2.date.isNull())
  { // Dont care about notes without date.
    return false;
  }

  if (d1.date == d2.date)
  { // On same dates, we sort according times.
    return (d1.time < d2.time);
  }

  // Compare dates.
  return (d1.date < d2.date);
}

void ToDo::parse()
{
  // Actual process row.
  QString row("");
  // Create cursor for diary textEdit.
  QTextCursor *cursor = new QTextCursor(ui->diaryTextEdit->document());
  // Text coloring.
  QTextCharFormat format;
  // Date regexp.
  QRegExp rx("^\\s*(0?[1-9]|[12][0-9]|3[01]).(0?[1-9]|1[0-2]).(\\d{4})\\s+(.*)");
  // Time regexp.
  QRegExp rx2("^(0?[0-9]|1[0-9]|2[0-4]):([0-5][0-9])\\s+(.*)");
  // Color regexp.
  QRegExp rx3("^\\*([1-8])\\*\\s+(.*)");
  // Temporary variables.
  struct note tempNote;
  QColor tempColor;
  int counter = 0;

  // Clear whole data structure.
  data.clear();

  for(int i = 0; i < 2; i++)
  { // Two textEdits to parse.
    // Move cursor to start.
    cursor->setPosition(0);

    while(!cursor->atEnd())
    { // Parsing loop.
      // Select line.
      cursor->select(QTextCursor::BlockUnderCursor);
      row = cursor->selectedText().trimmed();

      // Which color is line.
      format = cursor->charFormat();
      tempColor = format.foreground().color();
      for(int j = 0; j < colors.size(); j++)
      { // Which color is it?
        if(tempColor == colors[j])
          tempNote.color = j;
      }

      // Date and time on line.
      if(rx.indexIn(row) == -1)
      { // No date - it is note.
        tempNote.date = QDate();
        tempNote.time = QTime();
        tempNote.content = row.trimmed();
        // Color token on line.
        if(rx3.indexIn(tempNote.content) != -1)
        {
          if(rx.indexIn(rx3.cap(2).trimmed()) == -1)
          { // If line started as a date.
            tempNote.color = rx3.cap(1).toInt() - 1;
            tempNote.content = rx3.cap(2).trimmed();
          }
        }
      }
      else
      { // It has date - it is diary record.
        tempNote.date = QDate(rx.cap(3).toInt(),
                              rx.cap(2).toInt(),
                              rx.cap(1).toInt());
        // Time on line.
        if(rx2.indexIn(rx.cap(4)) == -1)
        { // No time.
          tempNote.time = QTime();
          tempNote.content = rx.cap(4).trimmed();
          // Color token on line.
          if(rx3.indexIn(tempNote.content) != -1)
          {
            tempNote.color = rx3.cap(1).toInt() - 1;
            tempNote.content = rx3.cap(2).trimmed();
          }
        }
        else
        { // It has time.
          tempNote.time = QTime(rx2.cap(1).toInt(), rx2.cap(2).toInt());
          tempNote.content = rx2.cap(3).trimmed();
          // Color token on line.
          if(rx3.indexIn(tempNote.content) != -1)
          {
            tempNote.color = rx3.cap(1).toInt() - 1;
            tempNote.content = rx3.cap(2).trimmed();
          }
        }
      }

      if(!(tempNote.date.isNull() && tempNote.content == ""))
      // Save temp to data structure.
      data.insert(counter++, tempNote);

      // Move cursor to next line.
      cursor->movePosition(QTextCursor::NextBlock);
    }


    // Move cursor to second textEdit.
    *cursor = QTextCursor(ui->notesTextEdit->document());
  }

  // Sort data.
  qStableSort(data.begin(), data.end(), ToDo::dataLessThan);

  delete cursor;
  cursor = NULL;
}

void ToDo::display()
{
  // Delete all from textEdits.
  ui->diaryTextEdit->clear();
  ui->notesTextEdit->clear();

  // Create cursors - one for diary, one for notes.
  QTextCursor *cursorDiary = new QTextCursor(ui->diaryTextEdit->document());
  QTextCursor *cursorNotes = new QTextCursor(ui->notesTextEdit->document());
  // Text coloring.
  QTextCharFormat format;
  // Temp variable.
  QString temp;
  bool firstDiary = true;
  bool firstNote = true;

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
      temp += (!row.time.isNull()) ? row.time.toString("hh:mm") + " " : "";
      temp += row.content;
      // Insert it.
      if(!firstDiary)
      {// Not a first line.
        cursorDiary->insertBlock();
      }
      else { firstDiary = false; }
      cursorDiary->insertText(temp, format);
    }
    else
    { // It's note.
      // Set color of row.
      format.setForeground(QBrush(colors[row.color]));
      // Create row only from content.
      temp = row.content;
      // Insert it.
      if(!firstNote)
      {// Not a first line.
        cursorNotes->insertBlock();
      }
      else { firstNote = false; }
      cursorNotes->insertText(temp, format);
    }
  }
}

void ToDo::userActive()
{
  // Wait 5s for another key press.
  inactivity->start(5000);
}

void ToDo::reload()
{
  // Parse text and save it to our data structures.
  parse();
  // Display changes in textArea.
  display();

  inactivity->stop();
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

void ToDo::filtersChanged()
{
  filters[0] = ui->color1Widget->getState();
  filters[1] = ui->color2Widget->getState();
  filters[2] = ui->color3Widget->getState();
  filters[3] = ui->color4Widget->getState();
  filters[4] = ui->color5Widget->getState();
  filters[5] = ui->color6Widget->getState();
  filters[6] = ui->color7Widget->getState();
  filters[7] = ui->color8Widget->getState();

  display();
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

  settings.beginReadArray("filters");
  int size = 8; // Ugly magic constant.
  filters.clear();
  for (int i = 0; i < size; ++i)
  { // Load filters vector.
    settings.setArrayIndex(i);
    filters.insert(i, settings.value("filter", true).value<bool>());
  }
  ui->color1Widget->setState(filters[0]);
  ui->color2Widget->setState(filters[1]);
  ui->color3Widget->setState(filters[2]);
  ui->color4Widget->setState(filters[3]);
  ui->color5Widget->setState(filters[4]);
  ui->color6Widget->setState(filters[5]);
  ui->color7Widget->setState(filters[6]);
  ui->color8Widget->setState(filters[7]);
  settings.endArray();

  settings.beginReadArray("colors");
  size = 8; // Ugly magic constant.
  colors.clear();
  for (int i = 0; i < size; ++i)
  { // Load colors vector.
    settings.setArrayIndex(i);
    colors.insert(i, settings.value("color", QColor(Qt::black)).value<QColor>());
  }
  ui->color1Widget->setColor(colors[0]);
  ui->color2Widget->setColor(colors[1]);
  ui->color3Widget->setColor(colors[2]);
  ui->color4Widget->setColor(colors[3]);
  ui->color5Widget->setColor(colors[4]);
  ui->color6Widget->setColor(colors[5]);
  ui->color7Widget->setColor(colors[6]);
  ui->color8Widget->setColor(colors[7]);
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

  settings.beginWriteArray("filters");
  for (int i = 0; i < filters.size(); ++i)
  { // Save filters vector.
    settings.setArrayIndex(i);
    settings.setValue("filter", filters.at(i));
  }
  settings.endArray();

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
  ui.fontSpinBox->setValue(settings.value("general/font").value<QFont>().pointSize());
  ui.timeComboBox->addItem("HH:MM");
  ui.dateComboBox->addItem("DD.MM.YYYY");

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
    settings.setValue("general/font", QFont(ui.fontComboBox->currentFont().toString(), ui.fontSpinBox->value()));

    // Apply changes.
    loadConfig();

    emit reload();
  }

  delete dialog;
  dialog = NULL;
}
