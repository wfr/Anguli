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

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <vector>
#include <QMutex>
#include <QWaitCondition>

class dialog :  public QDialog
{
    Q_OBJECT
public:
    explicit dialog(QWidget *parent = 0);

    void lockMutex() {
        mutex.lock();
    }

    void unloMutex() {
        mutex.unlock();
    }

    void waitForCondition()
    {
        wakeup_condition.wait(&mutex);
    }

public slots:
    void set_values(int y);
    void setValue(int progress);
    void setLabelText(const QString &text);
    void canceled();
    void paused();
    void setMaximum(int maximum);
    bool wasCanceled();
    bool reset();
    bool wasPaused();

signals:
    void pausedS();
    void resumedS();
    void exitedS();

private:
    void closeEvent(QCloseEvent *event);
    bool cancelStatus;
    bool pauseStatus;
    QLabel *label;
    QPushButton *pause, *cancel;
    QProgressBar *progressBar;
    QHBoxLayout *vb2;
    QVBoxLayout *toplayout;
    QMutex mutex;
    QWaitCondition wakeup_condition;
};

class console_progressDialog : public QObject
{
    Q_OBJECT
public:
    console_progressDialog();
    void console(const char *text);
public slots:
    void set_values(int y);
private:
    char progress[53];
};

#endif // DIALOG_H
