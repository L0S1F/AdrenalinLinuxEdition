#pragma once
#include <QCheckBox>

class SwitchButton : public QCheckBox {
    Q_OBJECT
public:
    explicit SwitchButton(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    QSize sizeHint() const override { return QSize(60, 30); }
};
