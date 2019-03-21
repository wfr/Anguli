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

#include "fingerprint_window.h"
#include <iostream>
#include <QFileDialog>
#include <QtXml/QDomDocument>
#include <QTextStream>
#include <QMessageBox>
#include <QDesktopWidget>
#include <iomanip>
#include <QToolTip>
#include <time.h>
#include <algorithm>

Fingerprint_Window::Fingerprint_Window(QWidget *parent)
    :QMainWindow(parent), fingerprintW(new Ui::GeneratorWindow)
{
    fingerprintW->setupUi(this);
    
    fingerprintW->ClassDistri->setToolTip       ("Select the distribution of fingerprints to be generated.");
    fingerprintW->FingersPerFolder->setToolTip  ("Set the number of fingerprints per directory. Some filesystems \nhave limit on maximum number of files per directory and \nlisting will be slower if directory contains to many files.");
    fingerprintW->Generate->setToolTip          ("Click to start the generation of fingerprints.");
    fingerprintW->ImageType->setToolTip         ("Choose the type of fingerprint image.");
    fingerprintW->ImprPerFinger->setToolTip     ("Set the number of impressions to be generated per fingerprint.");
    fingerprintW->metaSave->setToolTip          ("Check if meta information, like fingerprint type, \nis to be saved in text file.");
    fingerprintW->NumOfFinger->setToolTip       ("Set the number of fingerprints to be generated.");
    fingerprintW->OutputDir->setToolTip         ("Set the directory where fingerprints, impressions \nand meta information is to be saved.");
    fingerprintW->StartFinger->setToolTip       ("Set the number from where the numbers to \nfingerprint images should be given.");
    fingerprintW->threads->setToolTip           ("Set the number of threads to run to generate \nfingerprints simultaneously.");
    fingerprintW->Default->setToolTip           ("Click to reset the setting to default values.");
    fingerprintW->load_conf->setToolTip         ("Click to load the setting from a file.");
    fingerprintW->Save->setToolTip              ("Click to save the current setting.");
    fingerprintW->customCheck->setToolTip       ("Check if custom seed value is to be given for \nramdom number generator.");
    fingerprintW->seedVal->setToolTip           ("Enter the value of seed for random number generator.");
    fingerprintW->noise_level->setToolTip       ("Change the interval for noise level");
    fingerprintW->num_scratches->setToolTip     ("Change the interval for number of scatches per Impression");
    fingerprintW->trans_slider->setToolTip      ("Change the translation (in pixels).");
    fingerprintW->rot_slider->setToolTip        ("Change the rotation (in degrees).");
    
    connect(fingerprintW->pushButton, SIGNAL(clicked()), this, SLOT(Opendir_Clicked()));
    connect(fingerprintW->Generate, SIGNAL(clicked()), this, SLOT(Finger_Generate_Clicked()));
    connect(fingerprintW->Save, SIGNAL(clicked()), this, SLOT(Finger_save_config()));
    connect(fingerprintW->Default, SIGNAL(clicked()), this, SLOT(Finger_load_defaults()));
    connect(fingerprintW->customCheck, SIGNAL(toggled(bool)), SLOT(custom_Clicked(bool)));
    connect(fingerprintW->load_conf, SIGNAL(clicked()), SLOT(load_conf_clicked()));

    connect(fingerprintW->rot_slider, SIGNAL(valueChanged(int)), SLOT(rotation_sliderChanged(int)));
    connect(fingerprintW->trans_slider, SIGNAL(valueChanged(int)), SLOT(translation_sliderChanged(int)));
    connect(fingerprintW->noise_level, SIGNAL(spanChanged(int,int)), SLOT(sliderChanged(int,int)));
    connect(fingerprintW->num_scratches, SIGNAL(spanChanged(int,int)), SLOT(sliderChanged(int,int)));

    setFixedSize(this->width(), this->height() - 25);
    
    QRect frect = frameGeometry();
    frect.moveCenter(QDesktopWidget().availableGeometry().center());
    move(frect.topLeft());
    fingerprintW->customCheck->setChecked(false);
    fingerprintW->seedVal->setEnabled(false);
}
Fingerprint_Window::~Fingerprint_Window()
{
    
}

/*
  Handles the range slider change event and shows the selected range as tool tip.
  */
void Fingerprint_Window::sliderChanged(int low, int high)
{
    std::stringstream value;
    value << low << " - " << high;
    QToolTip::showText(QCursor::pos(), value.str().c_str());
}

/*
  Handles the change event of rotation slider to show the current
  value chosen.
  */
void Fingerprint_Window::rotation_sliderChanged(int val)
{
    std::stringstream value;
    value << val << "<sup>o</sup>";

    fingerprintW->rotation_label->setText(value.str().c_str());
}

/*
  Handles the change event of translation slider to show the current
  value chosen.
  */
void Fingerprint_Window::translation_sliderChanged(int val)
{
    std::stringstream value;
    value << val;
    fingerprintW->translation_label->setText(value.str().c_str());
}

/*
  Handles the check event of custom seed check box to enable or disable the
  input box for custo seed value.
  */
void Fingerprint_Window::custom_Clicked(bool val)
{
    if(val)
    {
        fingerprintW->seedVal->setEnabled(true);
    }
    else
    {
        fingerprintW->seedVal->setEnabled(false);
    }
}

/*
  Displays the dialog box for selecting the Folder.
  Returns the selected path.
  */
QString Fingerprint_Window::OpenDir_Dialog() {
    QString path;
    QFileDialog dialog;

    dialog.setWindowModality(Qt::WindowModal);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    path = dialog.getExistingDirectory(this, tr("Select Directory"), ".", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    return path;
}

/*
  Handle the clicked signal of Output Directory browse
  button.
  */
void Fingerprint_Window::Opendir_Clicked()
{
    QString path;
    path = OpenDir_Dialog();
    
    if(path.size() != 0)
        fingerprintW->OutputDir->setText( path );
}

/*
  Handles the clicked event of load config button.
  */
void Fingerprint_Window::load_conf_clicked()
{
    QFileDialog dialog;
    QString path;
    dialog.setWindowModality(Qt::WindowModal);
    path = dialog.getOpenFileName(this, tr("Select File"), QString::null,QString::null);
    load_config(path);
}

/*
  Handles the clicked signal of Generate Button. Initializes the 
  variables, makes appropriate connections between signals and
  handler routines.
  */
void Fingerprint_Window::Finger_Generate_Clicked()
{
    app = new threaded_app();
    if(!setVals())
        return;
    
    Dialog.reset();
    
    Dialog.setLabelText("Generating Fingerprints");
    Dialog.setMaximum(100);
    Dialog.connect(app, SIGNAL(update(int)), SLOT(set_values(int)));
    Dialog.setWindowModality(Qt::WindowModal);
    Dialog.setFixedSize(400, 120);
    Dialog.show();
    app->setDialog(&Dialog);
    this->connect(app, SIGNAL(finish()), SLOT(worker_finished()));
    app->start();
    fingerprintW->Generate->setEnabled(false);
    start_time = QDateTime::currentDateTime();
}

/*
  Handles the finish signal emitted by main worker thread created in Generate_Clicked()
  function. Does the cleanup and re-enables the Generate button.
  */
void Fingerprint_Window::worker_finished()
{
    delete app;
    th = NULL;
    app = NULL;
    fingerprintW->Generate->setEnabled(true);
    Dialog.hide();
    if(!Dialog.wasCanceled())
        QMessageBox::information(this, "Anguli", "Fingerprint Generation is Complete!", QMessageBox::Ok, QMessageBox::Ok);
}

/*
  Initializes the variables with appropriate values from GUI components.
  This sould be called before generation starts.
  */
bool Fingerprint_Window::setVals()
{
    no_fin  = atoi(fingerprintW->NumOfFinger->text().toStdString().c_str());
    size_fo = atoi (fingerprintW->FingersPerFolder->text().toStdString().c_str());
    n_impr  = atoi(fingerprintW->ImprPerFinger->text().toStdString().c_str());
    
    path_fing = fingerprintW->OutputDir->text().toStdString();

    save_meta       = (fingerprintW->metaSave->checkState() == Qt::Checked);
    
    n_threads = atoi(fingerprintW->threads->text().toStdString().c_str());
    start_from_finger = atoi(fingerprintW->StartFinger->text().toStdString().c_str());
    class_distribution = fingerprintW->ClassDistri->currentIndex();

    image_type = fingerprintW->ImageType->currentText().toStdString();
    generated = 0;
    use_custom_seed = (fingerprintW->customCheck->checkState() == Qt::Checked);
    if(use_custom_seed)
        custom_seed = fingerprintW->seedVal->value();
    else
        custom_seed = time(NULL);
    
    fingerprintW->seedVal->setValue(custom_seed);
    
    max_noise = fingerprintW->noise_level->upperPosition();
    min_noise = fingerprintW->noise_level->lowerPosition();
    max_scratches = fingerprintW->num_scratches->upperPosition();
    min_scratches = fingerprintW->num_scratches->lowerPosition();
    rotation = fingerprintW->rot_slider->value();
    translation = fingerprintW->trans_slider->value();

    if (app->create_dir(path_fing.c_str()) == false) {
            Dialog.hide();
            fingerprintW->Generate->setEnabled(true);
            return false;
    }
    
    save_config(QString(path_fing.c_str()) + "/Anguli.conf");
    return true;
}

/*
  Loads the setting from the xml file.
  */
void Fingerprint_Window::load_config(QString path)
{
    QFile file (path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QDomDocument doc;
    doc.setContent(&file);
    QDomElement docElements = doc.documentElement();
    QDomNodeList finger_elem = docElements.elementsByTagName("FingerprintGen");
    if(finger_elem.size() == 0)
    {
        return;
    }

    docElements = finger_elem.at(0).toElement();
    QDomNodeList elements;
    QDomElement conf;
    elements = docElements.elementsByTagName("NumFingerprints");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->NumOfFinger->setValue(atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("ImprPerFinger");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->ImprPerFinger->setValue(atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("ClassDistr");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->ClassDistri->setCurrentIndex(atoi(conf.attribute("value", "0").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("OutDir");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->OutputDir->setText(conf.attribute("value", "./Fingerprints"));
    }
    
    elements = docElements.elementsByTagName("StartFinger");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->StartFinger->setValue(atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("FingerPerDir");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->FingersPerFolder->setValue(atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("ImageType");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->ImageType->setCurrentIndex(atoi(conf.attribute("value", "0").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("NumThreads");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->threads->setValue(atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("SaveMetaInfo");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->metaSave->setChecked(atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("customSeed");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->customCheck->setChecked(atoi(conf.attribute("value", "1").toStdString().c_str()));
        fingerprintW->seedVal->setValue(atoi(conf.attribute("seedValue", "1").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("NoiseLevel");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->noise_level->setLowerPosition(atoi(conf.attribute("low", "1").toStdString().c_str()));
        fingerprintW->noise_level->setUpperPosition(atoi(conf.attribute("high", "1").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("NumScratches");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->num_scratches->setLowerPosition(atoi(conf.attribute("low", "1").toStdString().c_str()));
        fingerprintW->num_scratches->setUpperPosition(atoi(conf.attribute("high", "1").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("Rotation");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->rot_slider->setValue(atoi(conf.attribute("value", "1").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("Translation");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        fingerprintW->trans_slider->setValue(atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
}

/*
  Loads the default config file present in current directory.
  Called when Fingerprint window is opened.
  */
void Fingerprint_Window::Fingerprint_load_config()
{
    load_config("Anguli.conf");
}

/*
  Save the current settings to a xml file.
  */
void Fingerprint_Window::save_config(QString path)
{
    QFile file (path);
    if(!file.open(QIODevice::ReadWrite| QIODevice::Text))
    {
        exit(0);
    }
    QDomDocument doc;
    doc.setContent(&file);
    QDomElement docElements = doc.documentElement();
    QDomElement root_node;
    if( docElements.isNull())
    {
        root_node = doc.createElement("Anguli");
        doc.insertBefore(root_node, QDomNode());
    }
    else
        root_node = docElements;
    
    QDomNodeList finger_elem = docElements.elementsByTagName("FingerprintGen");
    QDomElement finger_node;
    if(finger_elem.size() == 0)
    {
        finger_node = doc.createElement("FingerprintGen");
        root_node.appendChild(finger_node);
    }
    else
        finger_node = finger_elem.at(0).toElement();
    docElements = finger_node;
    QDomNodeList elements;
    QDomElement conf;
    elements = docElements.elementsByTagName("NumFingerprints");
    if(elements.size() == 0)
    {
        conf = doc.createElement("NumFingerprints");
        conf.setAttribute("value", fingerprintW->NumOfFinger->text());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", fingerprintW->NumOfFinger->text());
    }
    
    elements = docElements.elementsByTagName("ImprPerFinger");
    if(elements.size() == 0)
    {
        conf = doc.createElement("ImprPerFinger");
        conf.setAttribute("value", fingerprintW->ImprPerFinger->text());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", fingerprintW->ImprPerFinger->text());
    }
    
    elements = docElements.elementsByTagName("ClassDistr");
    if(elements.size() == 0)
    {
        conf = doc.createElement("ClassDistr");
        conf.setAttribute("value", fingerprintW->ClassDistri->currentIndex());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", fingerprintW->ClassDistri->currentIndex());
    }
    
    elements = docElements.elementsByTagName("OutDir");
    if(elements.size() == 0)
    {
        conf = doc.createElement("OutDir");
        conf.setAttribute("value", fingerprintW->OutputDir->text());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", fingerprintW->OutputDir->text());
    }
    
    elements = docElements.elementsByTagName("StartFinger");
    if(elements.size() == 0)
    {
        conf = doc.createElement("StartFinger");
        conf.setAttribute("value", fingerprintW->StartFinger->text());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", fingerprintW->StartFinger->text());
    }
    
    elements = docElements.elementsByTagName("FingerPerDir");
    if(elements.size() == 0)
    {
        conf = doc.createElement("FingerPerDir");
        conf.setAttribute("value", fingerprintW->FingersPerFolder->text());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", fingerprintW->FingersPerFolder->text());
    }
    
    elements = docElements.elementsByTagName("ImageType");
    if(elements.size() == 0)
    {
        conf = doc.createElement("ImageType");
        conf.setAttribute("value", fingerprintW->ImageType->currentIndex());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", fingerprintW->ImageType->currentIndex());
    }

    elements = docElements.elementsByTagName("NumThreads");
    if(elements.size() == 0)
    {
        conf = doc.createElement("NumThreads");
        conf.setAttribute("value", fingerprintW->threads->text());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", fingerprintW->threads->text());
    }
    
    elements = docElements.elementsByTagName("SaveMetaInfo");
    if(elements.size() == 0)
    {
        conf = doc.createElement("SaveMetaInfo");
        conf.setAttribute("value", (fingerprintW->metaSave->checkState() == Qt::Checked));
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", (fingerprintW->metaSave->checkState() == Qt::Checked));
    }
    
    elements = docElements.elementsByTagName("customSeed");
    if(elements.size() == 0)
    {
        conf = doc.createElement("customSeed");
        conf.setAttribute("value", (fingerprintW->customCheck->checkState() == Qt::Checked));
        conf.setAttribute("seedValue", fingerprintW->seedVal->text());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", (fingerprintW->customCheck->checkState() == Qt::Checked));
        conf.setAttribute("seedValue", fingerprintW->seedVal->text());
    }

    elements = docElements.elementsByTagName("NoiseLevel");
    if(elements.size() == 0)
    {
        conf = doc.createElement("NoiseLevel");
        conf.setAttribute("low", fingerprintW->noise_level->lowerPosition());
        conf.setAttribute("high", fingerprintW->noise_level->upperPosition());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("low", fingerprintW->noise_level->lowerPosition());
        conf.setAttribute("high", fingerprintW->noise_level->upperPosition());
    }

    elements = docElements.elementsByTagName("NumScratches");
    if(elements.size() == 0)
    {
        conf = doc.createElement("NumScratches");
        conf.setAttribute("low", fingerprintW->num_scratches->lowerPosition());
        conf.setAttribute("high", fingerprintW->num_scratches->upperPosition());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("low", fingerprintW->num_scratches->lowerPosition());
        conf.setAttribute("high", fingerprintW->num_scratches->upperPosition());
    }

    elements = docElements.elementsByTagName("Rotation");
    if(elements.size() == 0)
    {
        conf = doc.createElement("Rotation");
        conf.setAttribute("value", fingerprintW->rot_slider->value());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", fingerprintW->rot_slider->value());
    }

    elements = docElements.elementsByTagName("Translation");
    if(elements.size() == 0)
    {
        conf = doc.createElement("Translation");
        conf.setAttribute("value", fingerprintW->trans_slider->value());
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", fingerprintW->trans_slider->value());
    }
    
    file.seek(0);
    file.resize(0);
    QTextStream stream(&file);

    stream << doc.toString();
    file.close();
}

/*
  Handles the clicked event of save configuration button.
  */
void Fingerprint_Window::Finger_save_config()
{
    save_config("Anguli.conf");
}

/*
  Handles the clicked event of Load Defaults button.
  */
void Fingerprint_Window::Finger_load_defaults()
{
    fingerprintW->NumOfFinger->setValue(1);
    fingerprintW->ImprPerFinger->setValue(1);
    fingerprintW->ClassDistri->setCurrentIndex(0);
    fingerprintW->OutputDir->setText("./Fingerprints");
    fingerprintW->metaSave->setChecked(1);
    fingerprintW->FingersPerFolder->setValue(1000);
    fingerprintW->StartFinger->setValue(1);
    fingerprintW->ImageType->setCurrentIndex(0);
    fingerprintW->noise_level->setLowerPosition(0);
    fingerprintW->noise_level->setUpperPosition(0);
    fingerprintW->num_scratches->setLowerPosition(0);
    fingerprintW->num_scratches->setUpperPosition(0);
    fingerprintW->rot_slider->setValue(15);
    fingerprintW->trans_slider->setValue(50);

    /*
      Get the processor count from system and reduce it by one.
      If single processor system the do nothin, keep one thread as
      it is.
      */
#ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo (&sysinfo);

    int processors = sysinfo.dwNumberOfProcessors;
#else
    int processors = std::max(1, int(std::thread::hardware_concurrency()));
#endif


    if(processors > 1)
    {
        processors--;
    }
    fingerprintW->threads->setValue(processors);
    
    fingerprintW->customCheck->setChecked(false);
    fingerprintW->seedVal->setValue(1);
}
