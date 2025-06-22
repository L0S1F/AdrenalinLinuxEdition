#include "switchbutton.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

SwitchButton::SwitchButton(QWidget *parent) : QCheckBox(parent) {
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
}

void SwitchButton::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF rect = QRectF(0, 0, width(), height()).adjusted(7, 7, -7, -7);
    qreal circleRadius = height() - 8;
    qreal x = isChecked() ? width() - circleRadius - 4 : 4;
    QRectF circleRect(x, 4, circleRadius, circleRadius);

    QColor bg = isChecked() ? QColor("#F30022") : QColor("#7F7F7F");
    p.setBrush(bg);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect, height() / 3.5f, height() / 3.5f);

    p.setBrush(Qt::white);
    p.drawEllipse(circleRect);
}

void SwitchButton::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        setChecked(!isChecked());
        event->accept();
        update();
    } else {
        QCheckBox::mousePressEvent(event);
    }
}
