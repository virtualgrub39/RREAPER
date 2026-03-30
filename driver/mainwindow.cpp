#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ui.setupUi(this);  // instantiates and places all widgets from the .ui file

    // widgets are now accessible as ui.whatever — named by you in Designer
    connect(ui.connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
}

void MainWindow::onConnectClicked() {
    ui.logView->append("clicked");
}
