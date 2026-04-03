/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#pragma once

#include "ui_mainwindow.h"

#include <concepts>
#include <cstdint>
#include <span>


#include <QMainWindow>
#include <QSerialPort>

using std::span;

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow (QWidget *parent = nullptr);

  private slots:
    void onConnectButtonClick ();
    void onReadButtonClick ();
    void onWriteButtonClick ();

  private:
    enum class Stage
    {
        Disconnected,
        Connected,
        Reading,
        Writing,
    } stage;

    Ui::MainWindow ui;
    QSerialPort port;
    QByteArray rxBuf;

    void portConnect ();
    void portDisconnect ();
    void configurePort (qint32 baud);
    void onReadyRead ();
    void sendReset ();
    void processIncoming ();

    // template <std::unsigned_integral T>
    // void ihexRead(T& out);

    // template <std::unsigned_integral T>
    // void ihexWrite(T value);

    // void ihexRead(span<uint8_t> out);
    // void ihexWrite(span<const uint8_t> data);
};