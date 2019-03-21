/*
#
# COPYRIGHT INFORMATION
#
# Copyright (C) 2012 Indian Institute of Science Bangalore 560012, INDIA
#
# This program is part of the Anguli synthetic fingerprint generator
# software distribution deveoped at the Database Systems Lab, Indian
# Institute of Science. The use of the software is free and its use is
# governed by the licensing agreement set up between the copyright owner,
# Indian Institute of Science, and the licensee.
#
# This program is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# The URL of the Anguli project is
#     http://dsl.serc.iisc.ernet.in/projects/Anguli/
# and the email contact address is
#     anguli@dsl.serc.iisc.ernet.in
#
#
# For any issues, contact
# Prof. Jayant R. Haritsa
# SERC
# Indian Institute of Science
# Bangalore 560012, India.
# Telephone : (+91) 80 2293-2793
# Fax       : (+91) 80 2360-2648
# Email     : haritsa@dsl.serc.iisc.ernet.in
# WWW: http://dsl.serc.iisc.ernet.in/~haritsa/
#
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include "fingerprint_window.h"
#include "impression_window.h"
#include "ui_about.h"

using namespace anguli;

namespace Ui {
    class MainWindow;
    class GeneratorWindow;
    class ImpressionWindow;
    class infoWindow;
}

class about_Window : public QMainWindow {
    Q_OBJECT
public:
    about_Window(QWidget *parent = 0);
    ~about_Window();
private:
    Ui::infoWindow *infow;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void fingerprints_Clicked();
    void impressions_Clicked();
    void about_Clicked();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    Fingerprint_Window *f_window;
    Impression_Window *i_window;
    about_Window *a_window;
};
#endif // MAINWINDOW_H
