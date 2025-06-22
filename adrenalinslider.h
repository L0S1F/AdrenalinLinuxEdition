// AdrenalinSlider.h
#pragma once
#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>

class AdrenalinSlider : public QWidget {
    Q_OBJECT

public:
    explicit AdrenalinSlider(int min, int max, QWidget *parent = nullptr);
    void setValue(int value);
    int value() const;

signals:
    void valueChanged(int newValue);

private:
    QLineEdit *valueField;
    QSlider *slider;

private slots:
    void handleSliderChanged(int val);
    void handleTextChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
};
