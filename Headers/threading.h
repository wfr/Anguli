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

#ifndef THREADING_H
#define THREADING_H
#include <QThread>
#include <QObject>
#include <QProgressDialog>
#include "generation.h"
#include "dialog.h"

using namespace anguli;
using namespace std;

class QThreadEx : public QThread
{
public:
    void run() { exec(); }
};

class thread_work : public QObject, public Generator
{
    Q_OBJECT
public :
        thread_work() { }
        ~thread_work() { }
public slots:
    void generate();
    void generate_density_maps();
signals:
    void update(int y);
    void finish();
public:
    int start_finger;
    dialog *Dialog;
    console_progressDialog *console_progress;
};

class noise_work :  public QObject, public noise::Noise
{
    Q_OBJECT
public:
    noise_work() { }
    ~noise_work() { }
public slots:
    void generate_impressions();
signals:
    void update(int y);
    void finish();
public:
    dialog *Dialog;
    console_progressDialog *console_progress;
    QString input_dir;
    QString output_dir;
    int id;
};

class threaded_app: public QObject
{
    Q_OBJECT
    public:
    
    threaded_app() {childs_finished = 0;}
    bool create_dir (const char *path);
    void setDialog(dialog *dialog)
    {
        Dialog = dialog;
    }
    void setConsoleProgress(console_progressDialog *dialog)
    {
        console_progress = dialog;
    }
    void delete_filter();
    void save_config(QString path);
    void showErr(QString err);
public slots:
   void start();
   void noise_start();
   void child_finished();
signals:
    void update(int y);
    void finish();
private:
    dialog *Dialog;
    console_progressDialog* console_progress;
    vector <QThreadEx *> thread_pool;
    vector <thread_work *> work_pool;
    vector <noise_work *> nwork_pool;
    int childs_finished;
};
#endif // THREADING_H
