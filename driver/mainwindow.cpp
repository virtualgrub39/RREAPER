#include "mainwindow.h"

#include <QSerialPortInfo>
#include <QtWidgets>

MainWindow::MainWindow (QWidget *parent) : QMainWindow (parent)
{
    ui.setupUi (this);

    for (auto &info : QSerialPortInfo::availablePorts ()) ui.portSelector->addItem (info.portName ());

    connect (ui.connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClick);
    connect (ui.readButton, &QPushButton::clicked, this, &MainWindow::onReadButtonClick);
    connect (ui.writeButton, &QPushButton::clicked, this, &MainWindow::onWriteButtonClick);
}

void
MainWindow::onConnectButtonClick ()
{
    ui.statusBar->showMessage ("CONNECT CLICKED!", 2000);
}

void
MainWindow::onReadButtonClick ()
{
    ui.statusBar->showMessage ("READ CLICKED!", 2000);
}

void
MainWindow::onWriteButtonClick ()
{
    ui.statusBar->showMessage ("WRITE CLICKED!", 2000);
}
