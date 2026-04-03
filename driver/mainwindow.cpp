/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#include "mainwindow.h"

#include <QSerialPortInfo>
#include <QtWidgets>
#include <qhashfunctions.h>
#include <qserialport.h>

MainWindow::MainWindow (QWidget *parent) : QMainWindow (parent)
{
    ui.setupUi (this);

    stage = Stage::Disconnected;

    for (auto &info : QSerialPortInfo::availablePorts ()) ui.portSelector->addItem (info.portName ());

    connect (ui.connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClick);
    connect (ui.readButton, &QPushButton::clicked, this, &MainWindow::onReadButtonClick);
    connect (ui.writeButton, &QPushButton::clicked, this, &MainWindow::onWriteButtonClick);
}

void
MainWindow::configurePort (qint32 baud)
{
    port.setBaudRate (baud);
    port.setDataBits (QSerialPort::Data8);
    port.setParity (QSerialPort::NoParity);
    port.setStopBits (QSerialPort::OneStop);
    port.setFlowControl (QSerialPort::NoFlowControl);
}

void
MainWindow::portConnect ()
{
    auto port_name = ui.portSelector->currentText ();
    auto baud_txt = ui.baudInput->text ();
    if (baud_txt.length () == 0)
    {
        ui.statusBar->showMessage ("Invalid baud rate");
        return;
    }

    port.close ();
    stage = Stage::Disconnected;

    port.setPortName (port_name);
    if (!port.open (QIODevice::ReadWrite))
    {
        ui.statusBar->showMessage (tr ("Can't open %1, error code %2").arg (port_name).arg (port.error ()), 0);
        return;
    }
    configurePort (baud_txt.toUInt ());

    connect (&port, &QSerialPort::readyRead, this, &MainWindow::onReadyRead);

    rxBuf.clear ();
    sendReset ();

    ui.statusBar->showMessage (tr ("Connecting to %1…").arg (port_name));
}

void
MainWindow::portDisconnect ()
{
    port.close ();
    stage = Stage::Disconnected;
    ui.statusBar->showMessage (tr ("Disconnected"), 0);
    ui.connectButton->setText ("CONNECT");
}

void
MainWindow::onConnectButtonClick ()
{
    if (port.isOpen ())
        portDisconnect ();
    else
        portConnect ();
}

void
MainWindow::onReadyRead ()
{
    rxBuf += port.readAll ();
    processIncoming ();
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

void
MainWindow::sendReset ()
{
    port.write ("\x03");
}

void
MainWindow::processIncoming ()
{
    while (rxBuf.contains ("\r\n"))
    {
        int end = rxBuf.indexOf ("\r\n");
        QByteArray line = rxBuf.left (end).trimmed ();
        rxBuf.remove (0, end + 2);

        if (line.length () == 0) continue;
        if (line == "RESET")
        {
            stage = Stage::Disconnected;
            ui.statusBar->showMessage (tr ("Device Reset"), 0);
            continue;
        }

        switch (stage)
        {
        case Stage::Disconnected: {
            if (line == "READY")
            {
                stage = Stage::Connected;
                ui.statusBar->showMessage (tr ("Connected"), 0);
                ui.connectButton->setText ("DISCONNECT");
            }

            break;
        }
        default: qDebug () << "Device:" << line;
        }
    }
}

// template <std::unsigned_integral T>
// void MainWindow::ihexRead(T& out)
// {

// }

// template <std::unsigned_integral T>
// void MainWindow::ihexWrite(T value)
// {

// }

// void MainWindow::ihexRead(span<uint8_t> out)
// {

// }

// void MainWindow::ihexWrite(span<const uint8_t> data)
// {

// }
