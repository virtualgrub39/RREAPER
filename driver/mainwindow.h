#pragma once

#include "ui_mainwindow.h"
#include <QMainWindow>

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
    Ui::MainWindow ui;
};