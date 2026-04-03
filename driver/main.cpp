/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#include "mainwindow.h"
#include <QApplication>

int
main (int argc, char *argv[])
{
    QApplication app (argc, argv);
    MainWindow w;
    w.show ();
    return app.exec ();
}
