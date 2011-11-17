#include "colorwidget.h"

ColorWidget::ColorWidget(QWidget *parent)
  : QWidget(parent), state(true), color(Qt::black)
{
  repaint();
}

void ColorWidget::setColor(QColor color)
{
  this->color = color;

  repaint();
}

void ColorWidget::paintEvent(QPaintEvent *event)
{
  QPalette pal(palette());
  if (state)
  {
    pal.setBrush(QPalette::Background, QBrush(color, Qt::SolidPattern));
  }
  else
  {
    pal.setBrush(QPalette::Background, QBrush(color, Qt::Dense4Pattern));
  }

  setAutoFillBackground(true);
  setPalette(pal);

  QWidget::paintEvent(event);
}

void ColorWidget::mousePressEvent(QMouseEvent *event)
{
  setState(!state);

  QWidget::mousePressEvent(event);
}

bool ColorWidget::getState()
{
  return state;
}

void ColorWidget::setState(bool state)
{
  this->state = state;

  emit stateChanged();

  repaint();
}

void ColorWidget::toggleState()
{
  setState(!state);
}
