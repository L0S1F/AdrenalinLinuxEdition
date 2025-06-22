#include "adrenalinslider.h"
#include <QIntValidator>

AdrenalinSlider::AdrenalinSlider(int min, int max, QWidget *parent)
    : QWidget(parent) {
    slider = new QSlider(Qt::Horizontal);
    slider->setRange(min, max);
    slider->setFixedHeight(26);

    slider->setStyleSheet(R"(
    QSlider::groove:horizontal {
        height: 6px;
        background: #111;
        border-radius: 1px;
    }
    QSlider::sub-page:horizontal {
        background: #AE0B2D;
        border-radius: 1px;
        height: 10px;
    }
    QSlider::add-page:horizontal {
        background: #404040;
        border-radius: 1px;
        height: 10px;
    }
    QSlider::handle:horizontal {
        background: white;
        width: 14px;
        height: 26px;
        margin: -10 0;
        border-radius: 3px;
        border: 1px solid #888;
    }
)");



    valueField = new QLineEdit(QString::number(min));
    valueField->setFixedWidth(50);
    valueField->setAlignment(Qt::AlignCenter);
    valueField->setValidator(new QIntValidator(min, max, this));
    valueField->setStyleSheet(R"(
        QLineEdit {
            background-color: #292929;
            color: #939393;
            border: 2px solid #3E3C3D;
            height: 20px;
            border-radius: 2px;
            font-size: 15px;
            padding: 2px;
        }
    )");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->addWidget(valueField);
    layout->addWidget(slider);

    connect(slider, &QSlider::valueChanged, this, &AdrenalinSlider::handleSliderChanged);
    connect(valueField, &QLineEdit::editingFinished, this, &AdrenalinSlider::handleTextChanged);
}

void AdrenalinSlider::setValue(int value) {
    slider->setValue(value);
    valueField->setText(QString::number(value));
}

int AdrenalinSlider::value() const {
    return slider->value();
}

void AdrenalinSlider::handleSliderChanged(int val) {
    valueField->setText(QString::number(val));
    emit valueChanged(val);
}

void AdrenalinSlider::handleTextChanged() {
    int val = valueField->text().toInt();
    slider->setValue(val);
}

void AdrenalinSlider::paintEvent(QPaintEvent *event) {
    QStyleOptionSlider opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
