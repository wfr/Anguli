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

#include "dialog.h"
#include <iostream>
#include "global.h"

using namespace std;
using namespace anguli;

/*
  Constructor for Progress Dialog.
  */
dialog::dialog(QWidget *parent) :
    QDialog(parent)
{
    qDeleteAll(this->children());
    toplayout = new QVBoxLayout(this);
    toplayout->setSpacing(5);

    label = new QLabel("", this);
    label->setAlignment(Qt::AlignCenter);
    toplayout->addWidget(label, 0, Qt::AlignCenter);
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    toplayout->addWidget(progressBar, 0, Qt::AlignCenter);
    progressBar->setFixedWidth(350);
    cancel = new QPushButton("Cancel", this);
    pause = new QPushButton("Pause", this);
    vb2 = new QHBoxLayout(this);
    connect(cancel, SIGNAL(clicked()), this, SLOT(canceled()));
    connect(pause, SIGNAL(clicked()), this, SLOT(paused()));


    toplayout->addLayout(vb2);
    vb2->setSpacing(5);
    vb2->addSpacing(200);
    vb2->addWidget(pause);
    vb2->addWidget(cancel);
    vb2->addSpacing(50);

    cancelStatus = false;
    pauseStatus = false;

    setLayout(toplayout);
    setFixedWidth(400);
}

/*
  Increments the generated fingerprint count and updates the Progress Dialog.
  */
void dialog::set_values(int y)
{
        generated++;
        stringstream label;

        int elapsed_time = start_time.secsTo(QDateTime::currentDateTime());

        int remaining_time = (double) elapsed_time * (y - generated) / generated;
        int days = remaining_time / (24 * 60 * 60);
        remaining_time = remaining_time % (24 * 60 * 60);

        int hours = remaining_time / 3600;
        remaining_time = remaining_time % 3600;
        int mins = remaining_time / 60;
        remaining_time = remaining_time % 60;

        stringstream str;
        if (days)
            str << days << " Days ";

        str << hours << ":";
        str << mins << ":";
        str << remaining_time;


        if (noise_only)
            label << "Generating Impressions (" << generated << " / " << y << ")\nRemaining Time " << str.str();
        else
            label << "Generating Fingerprints (" << generated << " / " << y << ")\nRemaining Time: " << str.str();

        this->setLabelText(label.str().c_str());

        this->setValue(( generated * 100) / y);
}

/*
  Sets maximum value of progress bar.
  */
void dialog::setMaximum(int maximum)
{
    progressBar->setMaximum(maximum);
}

/*
  Sets the current value of progress bar.
  */
void dialog::setValue(int value)
{
    progressBar->setValue(value);
}

/*
  Sets the text of label.
  */
void dialog::setLabelText(const QString &text)
{
    label->setText(text);
}

/*
  Resets the dialog window.
  */
bool dialog::reset()
{
    cancelStatus = false;
    pauseStatus = false;
    progressBar->setValue(0);
    pause->setText("Pause");
    return true;
}

/*
  Sets the cancel status to true. Wakes up worker thread if
  the process was paused.
  */
void dialog::canceled() 
{
    cancelStatus = true;
    if(pauseStatus == true)
    {
        pauseStatus = false;
        wakeup_condition.wakeAll();
    }
    hide();
}

/*
  Returns true if cancel button was clicked.
  */
bool dialog::wasCanceled()
{
    return cancelStatus;
}

/*
  Same as cancel button click.
  */
void dialog::closeEvent(QCloseEvent *e)
{
    canceled();
    if(pauseStatus == true)
    {
        pauseStatus = false;
        wakeup_condition.wakeAll();
    }
    QDialog::closeEvent(e);
}

/*
  Handles the event of pausing the process.
  Sets the puase status and mutex locking for critical condition
  handling of reading and writing pause status.
  */
void dialog::paused()
{
    lockMutex();
    if(pauseStatus == true)
    {
        pause->setText("Pause");
        pauseStatus = false;
        emit resumedS();
        wakeup_condition.wakeAll();
    }
    else
    {
        pause->setText("Resume");
        pauseStatus = true;
        emit pausedS();
    }
    unloMutex();
}

/*
  Returns true if pause button was clicked.
  */
bool dialog::wasPaused() {
    return pauseStatus;
}

/*
  Constructor for Progress Dialog when the application is run in console mode.
  */
console_progressDialog::console_progressDialog()
{
    int i;
    for(i = 0; i < 52; i++)
    {
        progress[i] = '.';
    }
    progress[0] = '[';
    progress[51] = ']';
    progress[52] = NULL;
}

/*
  Increments the generated fingerprint coun and updates the console Progress
  Dialog.
  */
void console_progressDialog::set_values(int y)
{
    int i;
    generated++;
    static int last = 1;
    i = (generated * 50 ) / y;

    int elapsed_time = start_time.secsTo(QDateTime::currentDateTime());

    int remaining_time = (double) elapsed_time * (y - generated) / generated;
    int days = remaining_time / (24 * 60 * 60);
    remaining_time = remaining_time % (24 * 60 * 60);

    int hours = remaining_time / 3600;
    remaining_time = remaining_time % 3600;
    int mins = remaining_time / 60;
    remaining_time = remaining_time % 60;

    stringstream str;
    if (days)
        str << days << " Days ";

    str << hours << ":";
    str << mins << ":";
    str << remaining_time;
    //cout.width(200);
    for (;last <= i ; last ++)
        progress[last] = '=';
    if(i < 50)
        cout << "\r" << progress << "   " << i * 2
                << (noise_only ?  "%  Generating Impressions " : "%  Generating Fingerprints " )
                << generated  << " / " << y << ", " << "Remaining Time " << str.str();
    else
        cout << "\r" << progress << "   " << 100
                << (noise_only ? "%  Impression generation complete                           \n\n" : "%  Fingerprint generation complete                          \n\n ");
                //<< generated  << " / " << y << ", " << "Remaining Time " << str.str() << "          ";
    cout.flush();
}

/*
  Displays the Console Progress Dialog.
  */
void console_progressDialog::console(const char *text)
{
    cout << "\r" << progress << "   " <<  0 << "% " << text;
    cout.flush();
}
