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

#ifndef IMPRESSION_WINDOW_H
#define IMPRESSION_WINDOW_H
#include <QMainWindow>
#include "threading.h"
#include "ui_impression.h"

class Impression_Window : public QMainWindow
{
    Q_OBJECT
    public :
        Impression_Window(QWidget *parent = 0);
        ~Impression_Window();
        QString OpenDir_Dialog();
        bool setVals();
        void Impression_load_config();
    public slots:
        void Impression_load_defaults();
        void Impression_save_config();
        void inputDir_Clicked();
        void outputDir_Clicked();
        void worker_finished();
        void Impression_Generate_Clicked();
        void load_conf_clicked();
        void translation_sliderChanged(int val);
        void rotation_sliderChanged(int val);
        void sliderChanged(int low, int high);
    private:
        void load_config(QString path);
        void save_config(QString path);
        
        Ui::ImpressionWindow *impressionW;
        threaded_app* app;
        dialog Dialog;
        QTimer *t;
        QThreadEx* th;
};
#endif // IMPRESSION_WINDOW_H
