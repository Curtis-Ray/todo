#ifndef COLORWIDGET_H
#define COLORWIDGET_H

#include <QWidget>

class ColorWidget : public QWidget
{
    Q_OBJECT

  public:

    explicit ColorWidget(QWidget *parent = 0);

    ~ColorWidget() {};

    /**
     * Set widget color.
     */
    void setColor(QColor color);

    /**
     * Get widget state.
     */
    bool getState();

    /**
     * Set widget state.
     */
    void setState(bool state);

  signals:

    /**
     * User clicked to widget and state changed.
     */
    void stateChanged();

  private:

    void mousePressEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

    bool state; /**< Widget is checked. */
    QColor color; /**< Widget color. */

};

#endif // COLORWIDGET_H
