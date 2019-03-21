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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include <QToolTip>

using namespace std;
using namespace anguli;

/*
  Constructor for about Window.
  */
about_Window::about_Window(QWidget *parent) : QMainWindow(parent),
              infow(new Ui::infoWindow)
{
    infow->setupUi(this);
    
    QRect frect = frameGeometry();
    frect.moveCenter(QDesktopWidget().availableGeometry().center());
    move(frect.topLeft());
}

about_Window::~about_Window()
{
 
}

/*
*  MainWindow Constructor.
*  Initializes the UI and register the function for appropriate
*  Signals generated from UI components.
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Anguli: Synthetic Fingerprint Generator");
    setFixedSize(this->width(), this->height() - 25);
    f_window = NULL;
    i_window = NULL;
    a_window = NULL;
   
    ui->fingerprints->setToolTip("Generate New Fingerprints Database.");
    ui->about->setToolTip("About.");
    ui->exit->setToolTip("Exit Anguli.");
    ui->impressions->setToolTip("Generate impressions from existing Fingerprint Database.");
    connect(ui->fingerprints, SIGNAL(clicked()), this, SLOT(fingerprints_Clicked()));
    connect(ui->impressions, SIGNAL(clicked()), this, SLOT(impressions_Clicked()));
    connect(ui->about, SIGNAL(clicked()), this, SLOT(about_Clicked()));
    connect(ui->exit, SIGNAL(clicked()), qApp, SLOT(quit()));
    QRect frect = frameGeometry();
    frect.moveCenter(QDesktopWidget().availableGeometry().center());
    move(frect.topLeft());
}

/*
*  MainWindow Destructor.
*/
MainWindow::~MainWindow()
{
    delete ui;
}

/*
*  Autocreated from QT
*/
void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/*
  Handles the clicked event of Generate Fingerprint button.
  */
void MainWindow::fingerprints_Clicked()
{
    if(f_window == NULL)
    {
        
        f_window = new Fingerprint_Window(this);
        f_window->setWindowModality(Qt::WindowModal);
        f_window->setWindowTitle("Anguli: Synthetic Fingerprint Generator");
    }
    f_window->Fingerprint_load_config();
    
    f_window->show();
}

/*
  Handles the clicked event of Generate Impressions button.
  */
void MainWindow::impressions_Clicked()
{
    if(i_window == NULL)
    {
        i_window = new Impression_Window(this);
        i_window->setWindowModality(Qt::WindowModal);
        i_window->setWindowTitle("Anguli: Synthetic Fingerprint Generator");
    }
    i_window->Impression_load_config();
    i_window->setFocus();
    i_window->show();
}

/*
  Handles the clicked event of About button.
  */
void MainWindow::about_Clicked()
{
    if(a_window == NULL)
    {
        a_window = new about_Window(this);
        a_window->setFixedSize(a_window->width(), a_window->height());
        a_window->setWindowModality(Qt::WindowModal);
        a_window->setWindowTitle("About Anguli");
    }
    a_window->show();
}
