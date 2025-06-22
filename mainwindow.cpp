#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "switchbutton.h"
#include "adrenalinslider.h"
#include <QProcess>
#include <iostream>
#include <QDebug>
#include <QRegularExpression>

QString getVideoMemory() {
    QProcess proc;
    proc.start("glxinfo", QStringList());
    proc.waitForFinished(3000);
    QString output = proc.readAllStandardOutput();

    QRegularExpression rx(R"(Video memory:\s*(\d+)\s*MB)", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = rx.match(output);

    if (match.hasMatch()) {
        return match.captured(1) + " MB";
    }

    return "N/A";
}

QString getKernelVersion() {
    QProcess unameProc;
    unameProc.start("uname", QStringList() << "-r");
    unameProc.waitForFinished(1000);
    QString version = unameProc.readAllStandardOutput().trimmed();
    return version.isEmpty() ? "N/A" : version;
}

QString getCurrentResolutionWithRefresh() {
    QProcess xrandr;
    xrandr.start("xrandr", QStringList());
    xrandr.waitForFinished(3000);
    QString output = xrandr.readAllStandardOutput();

    // Шукаємо рядок з активною роздільністю (має `*`)
    QRegularExpression rx(R"( (\d+)x(\d+)\s+(\d+\.\d+)\*)");
    QRegularExpressionMatch match = rx.match(output);

    if (match.hasMatch()) {
        QString width = match.captured(1);
        QString height = match.captured(2);
        QString refresh = match.captured(3);
        return QString("%1x%2@%3Hz").arg(width, height, refresh);
    }

    return "N/A";
}

QString getValueFromLine(const QString &line) {
    return line.section(':', 1).trimmed();
}

QMap<QString, QString> parseLshwDisplay(const QString &text) {
    QMap<QString, QString> info;
    QStringList lines = text.split('\n');
    for (QString line : lines) {
        line = line.trimmed();
        if (line.startsWith("description:")) {
            info["description"] = line.section(':', 1).trimmed();
        } else if (line.startsWith("product:")) {
            info["product"] = line.section(':', 1).trimmed();
        } else if (line.startsWith("vendor:")) {
            info["vendor"] = line.section(':', 1).trimmed();
        } else if (line.startsWith("logical name:")) {
            info["logicalName"] = line.section(':', 1).trimmed();
        } else if (line.startsWith("version:")) {
            info["version"] = line.section(':', 1).trimmed();
        } else if (line.startsWith("width:")) {
            info["width"] = line.section(':', 1).trimmed();
        } else if (line.startsWith("clock:")) {
            info["clock"] = line.section(':', 1).trimmed();
        } else if (line.startsWith("configuration:")) {
            QString conf = line.section(':', 1).trimmed();
            QRegularExpression rxDriver("driver=([^\\s]+)");
            QRegularExpression rxResolution("resolution=([^\\s]+)");

            auto matchDriver = rxDriver.match(conf);
            if (matchDriver.hasMatch())
                info["driver"] = matchDriver.captured(1);

            auto matchResolution = rxResolution.match(conf);
            if (matchResolution.hasMatch())
                info["resolution"] = matchResolution.captured(1);
        }
    }
    return info;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QProcess proc;
    proc.start("/usr/bin/lshw", QStringList() << "-C" << "display");
    proc.waitForFinished(15000);
    QString output = proc.readAllStandardOutput();
    QString error = proc.readAllStandardError();

    qDebug() << "OUTPUT:" << output;
    qDebug() << "ERROR:" << error;



    auto info = parseLshwDisplay(output);

    ui->gpuName->setText(info.value("product", "N/A"));
    ui->gpuResolusion->setText(getCurrentResolutionWithRefresh());
    ui->gpuDriver->setText(info.value("driver", "N/A"));
    ui->linuxKernel->setText(getKernelVersion());
    ui->gpuMemory->setText(getVideoMemory());

    SwitchButton *switchBtn = new SwitchButton(this);
    switchBtn->setChecked(true);
    ui->verticalLayout->addWidget(switchBtn);

    connect(switchBtn, &QCheckBox::toggled, this, [=](bool checked) {
        std::cerr << "Switch 1:" << (checked ? "ON" : "OFF") << std::endl;
    });



    SwitchButton *switchBtn2 = new SwitchButton(this);
    switchBtn2->setChecked(true);

    ui->verticalLayout_2->addWidget(switchBtn2);

    connect(switchBtn2, &QCheckBox::toggled, this, [=](bool checked) {
        std::cerr << "Switch 2:" << (checked ? "ON" : "OFF") << std::endl;
    });






    AdrenalinSlider *powerSlider = new AdrenalinSlider(0, 100, this);
    ui->powerVertical->addWidget(powerSlider);

    connect(powerSlider, &AdrenalinSlider::valueChanged, this, [](int val) {
        std::cerr << "Power Limit set to:" << val << "W";
    });



}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_updateDriverButton_clicked() {
    QProcess *proc = new QProcess(this);
    QString cmd =
        "pkexec bash -c '"
        "pacman -Sy --noconfirm mesa lib32-mesa vulkan-radeon lib32-vulkan-radeon "
        "xf86-video-amdgpu libva-mesa-driver lib32-libva-mesa-driver "
        "mesa-vdpau lib32-mesa-vdpau opencl-mesa lib32-opencl-mesa"
        "'";
    proc->start("bash", QStringList() << "-c" << cmd);
    connect(proc, &QProcess::readyReadStandardOutput, this, [=]() {
        qDebug() << proc->readAllStandardOutput();
    });
    connect(proc, &QProcess::readyReadStandardError, this, [=]() {
        qDebug() << proc->readAllStandardError();
    });
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [](int exitCode, QProcess::ExitStatus) {
                std::cerr << "Driver installation finished with code:" << exitCode << std::endl;
            });
}
