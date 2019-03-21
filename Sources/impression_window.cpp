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

#include "impression_window.h"
#include <QFileDialog>
#include <QtXml/QDomDocument>
#include <QTextStream>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QToolTip>
#include <iostream>
#include <algorithm>

Impression_Window::Impression_Window(QWidget *parent)
    :QMainWindow(parent), impressionW(new Ui::ImpressionWindow)
{
    impressionW->setupUi(this);
    
    impressionW->Default->setToolTip("Click to reset the setting to default values.");
    impressionW->load_conf->setToolTip         ("Click to load the setting from a file.");
    impressionW->Save->setToolTip("Click to save the current setting.");
    impressionW->InputDir->setToolTip("Set the directory which contains the fingerprints   \nwhose impressions are to be generated. Fingerprints  \nshould be in same directory or in direct subfolders.");
    //impressionW->ImageType->setToolTip("Select the type of image for saving impressions.");
    impressionW->Generate->setToolTip("Click to start the generation of impressions.");
    impressionW->OutputDir_2->setToolTip("Set the directory where impressions are to be saved.");
    impressionW->threads_2->setToolTip("Set the number of threads to run to \ngenerate impressions simultaneously.");
    impressionW->ImprPerFinger_2->setToolTip("Set the number of impressions to be \ngenerated per fingerprint.");

    impressionW->noise_level->setToolTip       ("Change the interval for noise level");
    impressionW->num_scratches->setToolTip     ("Change the interval for number of scatches per Impression");
    
    impressionW->trans_slider->setToolTip      ("Change the translation (in pixels).");
    impressionW->rot_slider->setToolTip        ("Change the rotation (in degrees).");
    
    connect(impressionW->InputDir_button, SIGNAL(clicked()), this, SLOT(inputDir_Clicked()));
    connect(impressionW->OutputDir_button, SIGNAL(clicked()), this, SLOT(outputDir_Clicked()));
    
    connect(impressionW->Generate, SIGNAL(clicked()), this, SLOT(Impression_Generate_Clicked()));
    connect(impressionW->Save, SIGNAL(clicked()), this, SLOT(Impression_save_config()));
    connect(impressionW->Default, SIGNAL(clicked()), this, SLOT(Impression_load_defaults()));
    connect(impressionW->load_conf, SIGNAL(clicked()), this, SLOT(load_conf_clicked()));
    
    connect(impressionW->rot_slider, SIGNAL(valueChanged(int)), SLOT(rotation_sliderChanged(int)));
    connect(impressionW->trans_slider, SIGNAL(valueChanged(int)), SLOT(translation_sliderChanged(int)));
    connect(impressionW->noise_level, SIGNAL(spanChanged(int,int)), SLOT(sliderChanged(int,int)));
    connect(impressionW->num_scratches, SIGNAL(spanChanged(int,int)), SLOT(sliderChanged(int,int)));

    setFixedSize(this->width(), this->height() - 25);
    
    QRect frect = frameGeometry();
    frect.moveCenter(QDesktopWidget().availableGeometry().center());
    move(frect.topLeft());
}

Impression_Window::~Impression_Window()
{
    
}

/*
  Handles the range slider change event and shows the selected range as tool tip.
  */
void Impression_Window::sliderChanged(int low, int high)
{
    std::stringstream value;
    value << low << " - " << high;
    QToolTip::showText(QCursor::pos(), value.str().c_str());
}

/*
  Handles the change event of rotation slider to show the current
  value chosen.
  */
void Impression_Window::rotation_sliderChanged(int val)
{
    std::stringstream value;
    value << val << "<sup>o</sup>";

    impressionW->rotation_label->setText(value.str().c_str());
}

/*
  Handles the change event of translation slider to show the current
  value chosen.
  */
void Impression_Window::translation_sliderChanged(int val)
{
    std::stringstream value;
    value << val;
    impressionW->translation_label->setText(value.str().c_str());
}

/*
  Displays the dialog box for selecting the Folder.
  Returns the selected path.
  */
QString Impression_Window::OpenDir_Dialog() {
    QString path;
    QFileDialog dialog;

    dialog.setWindowModality(Qt::WindowModal);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);
    path = dialog.getExistingDirectory(this, tr("Select Directory"), ".", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    return path;
}

/*
  Handles the clicked event of load config button.
  */
void Impression_Window::load_conf_clicked()
{
    QFileDialog dialog;
    dialog.setWindowModality(Qt::WindowModal);
    QString path = dialog.getOpenFileName(this, tr("Select File"), QString::null,QString::null);
    load_config(path);
}

/*
  Handle the clicked signal of Input Directory browse
  button.
  */
void Impression_Window::inputDir_Clicked()
{
    QString path;
    path = OpenDir_Dialog();
    if(path.size() != 0)
        impressionW->InputDir->setText(path);
}

/*
  Handle the clicked signal of Output Directory browse
  button.
  */
void Impression_Window::outputDir_Clicked()
{
    QString path;
    path = OpenDir_Dialog();
    if(path.size() != 0)
        impressionW->OutputDir_2->setText(path);
}

/*
  Handles the finish signal emitted by main worker thread created in Generate_Clicked()
  function. Does the cleanup and re-enables the Generate button.
  */
void Impression_Window::worker_finished()
{
    delete app;
    th = NULL;
    app = NULL;
    impressionW->Generate->setEnabled(true);
    Dialog.hide();
    if(!Dialog.wasCanceled())
        QMessageBox::information(this, "Anguli", "Impressions Generation is Complete!", QMessageBox::Ok, QMessageBox::Ok);
}

/*
  Initializes the variables with appropriate values from GUI components.
  This sould be called before generation starts.
  */
bool Impression_Window::setVals()
{
    n_impr  = atoi(impressionW->ImprPerFinger_2->text().toStdString().c_str());
    n_threads = atoi(impressionW->threads_2->text().toStdString().c_str());

    path_noise_in = impressionW->InputDir->text().toStdString();
    path_fing = impressionW->OutputDir_2->text().toStdString();

    max_noise = impressionW->noise_level->upperPosition();
    min_noise = impressionW->noise_level->lowerPosition();

    max_scratches = impressionW->num_scratches->upperPosition();
    min_scratches = impressionW->num_scratches->lowerPosition();
    rotation = impressionW->rot_slider->value();
    translation = impressionW->trans_slider->value();

    generated = 0;
    
    if (app->create_dir(path_fing.c_str()) == false) {
        impressionW->Generate->setEnabled(true);
        Dialog.hide();
         return false;
    }
    
    save_config(QString(path_fing.c_str()) + "/Anguli.conf");
    return true;
}

/*
  Loads the configuration from xml file.
  */
void Impression_Window::load_config(QString path)
{
    QFile file (path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QDomDocument doc;
    doc.setContent(&file);
    QDomElement docElements = doc.documentElement();
    QDomNodeList impr_elem = docElements.elementsByTagName("ImpressionGen");
    if(impr_elem.size() == 0)
    {
        return;
    }
    docElements = impr_elem.at(0).toElement();
    QDomNodeList elements;
    QDomElement conf;
    
    elements = docElements.elementsByTagName("InputDir");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        impressionW->InputDir->setText(conf.attribute("value", "./Fingerprints"));
    }
    
    elements = docElements.elementsByTagName("OutDir");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        impressionW->OutputDir_2->setText(conf.attribute("value", "./Impressions"));
    }
    
    /*elements = docElements.elementsByTagName("ImageType");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        impressionW->ImageType->setCurrentIndex(atoi(conf.attribute("value", "0").toStdString().c_str()));
    }*/
    
    elements = docElements.elementsByTagName("NumThreads");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        impressionW->threads_2->setValue(atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("ImprPerFinger");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        impressionW->ImprPerFinger_2->setValue(atoi(conf.attribute("value", "0").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("NoiseLevel");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        impressionW->noise_level->setLowerPosition(atoi(conf.attribute("low", "0").toStdString().c_str()));
        impressionW->noise_level->setUpperPosition(atoi(conf.attribute("high", "0").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("NumScratches");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        impressionW->num_scratches->setLowerPosition(atoi(conf.attribute("low", "0").toStdString().c_str()));
        impressionW->num_scratches->setUpperPosition(atoi(conf.attribute("high", "0").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("Rotation");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        impressionW->rot_slider->setValue(atoi(conf.attribute("value", "0").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("Translation");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        impressionW->trans_slider->setValue(atoi(conf.attribute("value", "0").toStdString().c_str()));
    }
}

/*
  Loads the values from configuration files and sets the appropriate values
  of components.
  */
void Impression_Window::Impression_load_config()
{
    load_config(".Anguli.conf");
}

/*
  Loads the default values into appropriate UI components.
  */
void Impression_Window::Impression_load_defaults()
{

    impressionW->OutputDir_2->setText("./Impressions");
    impressionW->InputDir->setText("./Fingerprints/");
    impressionW->ImprPerFinger_2->setValue(1);

    impressionW->noise_level->setLowerPosition(0);
    impressionW->noise_level->setUpperPosition(0);
    impressionW->num_scratches->setLowerPosition(0);
    impressionW->num_scratches->setUpperPosition(0);
    impressionW->rot_slider->setValue(15);
    impressionW->trans_slider->setValue(50);

    //impressionW->ImageType->setCurrentIndex(0);
    
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
    impressionW->threads_2->setValue(processors);
}

/*
  Saves the current configuration to a xml file.
  */
void Impression_Window::save_config(QString path)
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
    
    QDomNodeList impr_elem = docElements.elementsByTagName("ImpressionGen");
    QDomElement impr_node;
    if(impr_elem.size() == 0)
    {
        impr_node = doc.createElement("ImpressionGen");
        root_node.appendChild(impr_node);
    }
    else
        impr_node = impr_elem.at(0).toElement();
    
    docElements = impr_node;
    
    QDomNodeList elements;
    QDomElement conf;
    elements = docElements.elementsByTagName("InputDir");
    if(elements.size() == 0)
    {
        conf = doc.createElement("InputDir");
        conf.setAttribute("value", impressionW->InputDir->text());
        impr_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", impressionW->InputDir->text());
    }
    
    elements = docElements.elementsByTagName("OutDir");
    if(elements.size() == 0)
    {
        conf = doc.createElement("OutDir");
        conf.setAttribute("value", impressionW->OutputDir_2->text());
        impr_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", impressionW->OutputDir_2->text());
    }
    
    /*elements = docElements.elementsByTagName("ImageType");
    if(elements.size() == 0)
    {
        conf = doc.createElement("ImageType");
        conf.setAttribute("value", impressionW->ImageType->currentIndex());
        impr_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", impressionW->ImageType->currentIndex());
    }*/
    
    elements = docElements.elementsByTagName("NumThreads");
    if(elements.size() == 0)
    {
        conf = doc.createElement("NumThreads");
        conf.setAttribute("value", impressionW->threads_2->text());
        impr_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", impressionW->threads_2->text());
    }
    
    elements = docElements.elementsByTagName("ImprPerFinger");
    if(elements.size() == 0)
    {
        conf = doc.createElement("ImprPerFinger");
        conf.setAttribute("value", impressionW->ImprPerFinger_2->text());
        impr_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", impressionW->ImprPerFinger_2->text());
    }

    elements = docElements.elementsByTagName("NoiseLevel");
    if(elements.size() == 0)
    {
        conf = doc.createElement("NoiseLevel");
        conf.setAttribute("low", impressionW->noise_level->lowerPosition());
        conf.setAttribute("high", impressionW->noise_level->upperPosition());
        impr_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("low", impressionW->noise_level->lowerPosition());
        conf.setAttribute("high", impressionW->noise_level->upperPosition());
    }

    elements = docElements.elementsByTagName("NumScratches");
    if(elements.size() == 0)
    {
        conf = doc.createElement("NumScratches");
        conf.setAttribute("low", impressionW->num_scratches->lowerPosition());
        conf.setAttribute("high", impressionW->num_scratches->upperPosition());
        impr_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("low", impressionW->num_scratches->lowerPosition());
        conf.setAttribute("high", impressionW->num_scratches->upperPosition());
    }

    elements = docElements.elementsByTagName("Rotation");
    if(elements.size() == 0)
    {
        conf = doc.createElement("Rotation");
        conf.setAttribute("value", impressionW->rot_slider->value());
        impr_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", impressionW->rot_slider->value());
    }

    elements = docElements.elementsByTagName("Translation");
    if(elements.size() == 0)
    {
        conf = doc.createElement("Translation");
        conf.setAttribute("value", impressionW->trans_slider->value());
        impr_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", impressionW->trans_slider->value());
    }

    
    file.seek(0);
    file.resize(0);
    QTextStream stream(&file);

    stream << doc.toString();
    file.close();
}

/*
  Saves the current configuration into confi file.
  */
void Impression_Window::Impression_save_config()
{
    save_config("Anguli.conf");
}

/*
  Handles the clicked event of Generate button.
  */
void Impression_Window::Impression_Generate_Clicked()
{
    noise_only = true;
    impressionW->Generate->setEnabled(false);
    app = new threaded_app();
    if(!setVals())
        return;
    Dialog.reset();

    Dialog.setLabelText("Generating Impressions");
    Dialog.setMaximum(100);
    Dialog.connect(app, SIGNAL(update(int)), SLOT(set_values(int)));
    Dialog.setWindowModality(Qt::WindowModal);
    Dialog.setFixedSize(400, 120);
    Dialog.show();
    app->setDialog(&Dialog);

    this->connect(app, SIGNAL(finish()), SLOT(worker_finished()));
    start_time = QDateTime::currentDateTime();
    app->noise_start();
}
