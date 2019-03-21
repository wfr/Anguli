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

#include "threading.h"

#include "filter_image.h"
#include "generation.h"
#include "densitymap.h"
#include "Noise/n_noise.h"
#include "Noise/n_cv_type_converter.h"

#include <QtGui/QApplication>
#include <QDir>
#include <QtXml/QDomDocument>
#include <QTextStream>
#include <QMessageBox>
#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <time.h>
#include "Noise/global.h"

using namespace std;

/*
  Generates the density maps.
  */
void thread_work::generate_density_maps()
{
    for (n_fp = 1; n_fp <= density_map_count; n_fp = n_fp + 1) {
        random_densitymap();
    }
}

/*
  Generates the fingerprints from starting number to end number which are set before
  calling this function.
  Handle the console mode and GUI mode appropriately.
  */
void thread_work::generate()
{
    for (n_fp = start_finger; n_fp < (no_fin + start_from_finger);) {
            n_fo = ceil((float)n_fp / size_fo);
            generation();

            if(is_GUI)
            {
                Dialog->lockMutex();
                if(Dialog->wasPaused())
                {
                    Dialog->waitForCondition();
                }
                Dialog->unloMutex();
            }

            emit update(no_fin);
            n_fp += n_threads;

            if(is_GUI && Dialog->wasCanceled())
            {
                cout << "Canceled!\n";
                break;
            }
    }
    emit finish();
}

/*
  Generates the impressions of fingerprints and stores them in appropriate
  folder.
  */
void noise_work::generate_impressions()
{

    QDir dire(input_dir);

    dire.setFilter(QDir::Dirs);
    QStringList entries = dire.entryList();

    QStringList::ConstIterator entry = entries.begin();

    cvReleaseImage(&f_print1);
    cvReleaseImage(&f_print2);

    f_print1 = 0;
    f_print2 = 0;

    QString str;

    do
    {
        QString name = *entry;
        if(name == "..")
        {
            entry++;
            continue;
        }
        QString path(input_dir);
        path.append("/");
        path.append(name);
        QDir files(path);
        files.setFilter(QDir::Files);
        QStringList filterList;
        filterList << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.tif" << "*.tiff";
        files.setNameFilters(filterList);
        files.setSorting(QDir::Name);
        QStringList fileentries = files.entryList();
        QStringList::ConstIterator fileentry = fileentries.begin();
        for(int j = 0; j < id && fileentry != fileentries.end(); j++)
        {
            fileentry++;
        }
        
        for(; fileentries.size() > 0 && fileentry != fileentries.end();)
        {
            QApplication::processEvents();
            QString file_name = *fileentry;
            str = QString(input_dir).append("/").append(name).append("/").append(file_name);
            f_print1 = cvLoadImage(str.toStdString().c_str(), CV_LOAD_IMAGE_GRAYSCALE);
            noise::Ipltomat2D(f_print1, f_print1_2Dmat);
            for (int i = 0; i < noise::H + noise::margin; i++) {
                for (int j = 0; j < noise::W + noise::margin; j++) {
                    if (f_print1_2Dmat[i][j] > 127)
                        f_print1_2Dmat[i][j] = 255;
                    else
                        f_print1_2Dmat[i][j] = 0;
                }
            }

            int num_scratches = min_scratches + (max_scratches - min_scratches) * noise::ahaq_rand();

            for(int num = 0; num < num_scratches; num++)
            {
                int width = f_print1->width;
                int height = f_print1->height;
                int part = 10;
                int x1 = (width / part) + noise::ahaq_rand() * (((part - 2) * width / part));

                int y1 = (height / part) + noise::ahaq_rand() * (((part - 2) * height / part));

                int x2 = (width / part) + noise::ahaq_rand() * (((part - 2) * width / part));

                int y2 = (height / part) + noise::ahaq_rand() * (((part - 2) * height / part));

                int wide = 1 + noise::ahaq_rand() * 6;

                CvScalar white = CV_RGB(255,255,255);
                cvLine(f_print1, cvPoint(x1, y1), cvPoint(x2, y2), white, wide, 4, 0);
            }

            fixed_noise(f_print1);

            f_print2 = cvCloneImage(f_print1);

            for (int i = 0; i < noise::H + noise::margin; i++) {
                for (int j = 0; j < noise::W + noise::margin; j++) {
                    f_print2_2Dmat[i][j] = f_print1_2Dmat[i][j];
                }
            }

            for(int k = 1; k <= n_impr; k++)
            {
                noise::Ipltomat2D(f_print1, f_print1_2Dmat);
                for (int i = 0; i < noise::H + noise::margin; i++) {
                    for (int j = 0; j < noise::W + noise::margin; j++) {
                        if (f_print1_2Dmat[i][j] > 127)
                            f_print1_2Dmat[i][j] = 255;
                        else
                           f_print1_2Dmat[i][j] = 0;
                    }
                }
                random_noise(f_print1);
                str = name + "/" + file_name;
                std::stringstream stream;
                stream << path_fing << "_" << k << "/" << str.toStdString().c_str();

                distortion(stream.str().c_str());

                cvCopy(f_print2, f_print1);
            }
            if(is_GUI)
            {
                Dialog->lockMutex();
                if(Dialog->wasPaused())
                {
                    Dialog->waitForCondition();
                }
                Dialog->unloMutex();
            }

            if(is_GUI && Dialog->wasCanceled())
            {
                cout << "Canceled!\n";
                break;
            }
            emit update(no_fin);
            cvReleaseImage(&f_print1);
            cvReleaseImage(&f_print2);

            for(int j = 0; j < (n_threads) && fileentry != fileentries.end(); j++, fileentry++);
        }
        entry++;

        if(is_GUI && Dialog->wasCanceled())
            break;
    } while(entry != entries.end());

    emit finish();
}

/*
  Checks if the directory, whose path is given as argument, is present or not.
  if directory is not present and creates the directory.
  Return true if directory is present or mkdir does not give error, else return
  false.
  */
bool threaded_app::create_dir(const char *path) {

    if(!QDir(path).exists())
    {
        if(QDir().mkdir(path))
            return true;
    }
    else
        return true;
    showErr(QString("Cannon create directory: ") + path);
    return false;
}

/* Called when a child(worker) thread exits. When all worker
   threads are finished then emits finish signal and releases
   allocated memory.
   */
void threaded_app::child_finished()
{
    childs_finished++;

    if(childs_finished == thread_pool.size())
    {
        while(!thread_pool.empty())
        {
            QThreadEx *tr = thread_pool.back();
            while(tr->isRunning())
            {
                QApplication::processEvents();

                tr->wait(1000);
            }
            thread_pool.pop_back();
            if(!noise_only) {
                thread_work *work = work_pool.back();
                work_pool.pop_back();
                delete work;
            }
            else
            {
                noise_work *work = nwork_pool.back();
                nwork_pool.pop_back();
                delete work;
            }
            delete tr;
        }
        emit finish();
        childs_finished = 0;
    }
}

/*
  Displays the error accroding to mode.
  */
void threaded_app::showErr(QString err)
{
    if(is_GUI)
    {
        QMessageBox::information(Dialog, "Anguli", err, 
                             QMessageBox::Ok, QMessageBox::Ok);
    }
    else
    {
        cout << "\rWARNING: " << err.toStdString() << endl;
        exit(0);
    }
}

/*
  Starting point of Generation process. Creates the required directories.
  Creates the threads as per the setting, initializes the worker threads
  appropriately.
  In command line mode waits for each worker thread.
  */
void threaded_app::start()
{
    int start_fo, end_fo, n_fo;
    start_fo = ceil(((float) start_from_finger) / size_fo);
    end_fo = ceil((float)(start_from_finger + no_fin - 1) / size_fo);

    if (create_dir(path_fing.c_str()) == false) {
        
            showErr("Cannot create Fingerprint Directory");
            return;
    }

    path_o_f = path_fing;
    path_o_f.append("/Fingerprints");

    path_M_F = path_fing;
    path_M_F.append("/Master Fingerprints");

    path_o_ref = path_fing;
    path_o_ref.append("/Meta Info");

    path_impr = path_fing;
    path_impr.append("/Impression");

    density_map_count = 2000;

    if (create_dir(path_o_f.c_str()) == false) {
            
            return;
    }
   /*if (create_dir(path_M_F) == false) {
            return;
    }*/
    if (save_meta && create_dir(path_o_ref.c_str()) == false) {
            return;
    }
    int i;
    for(i = 1; i <= n_impr; i++)
    {
        std::stringstream stream;
        stream << path_impr << "_" << i;
        if (create_dir(stream.str().c_str()) == false) {
                return;
        }
    }
    cout << endl;
    if(is_GUI)
        Dialog->setLabelText("Creating directories");
    else
        console_progress->console("Creating directories");
    
    for (n_fo = start_fo; n_fo <= end_fo; n_fo = n_fo + 1) {
        
        std::stringstream stream;
        stream << path_o_f << "/fp_" << n_fo;

        if (create_dir(stream.str().c_str()) == false) {
            return;
        }
        /*
        sprintf(cmd, "%s/%d", path_M_F, n_fo);
        if (create_dir(cmd) == false) {
          return;
        }*/

        stream.str("");
        stream << path_o_ref << "/fp_" << n_fo;
        if (save_meta && create_dir(stream.str().c_str()) == false) {
            return;
        }

       for(i = 1; i <= n_impr; i++)
       {
           stream.str("");
           stream << path_impr << "_" << i << "/fp_" << n_fo;
            if (create_dir(stream.str().c_str()) == false) {
              return;
            }
        }
    }

    int start_fingerprint = start_from_finger;

    if(!is_GUI) 
    {
        console_progress->console("Generating Fingerprints");
        save_config(QString(path_fing.c_str()) + "/Anguli.conf");
    }

    int threads_to_run = n_threads;

    i = 1;

    for(; i <= threads_to_run; i++)
    {
        QThreadEx *tr = new QThreadEx();
        thread_work *work = new thread_work();

        work->moveToThread(tr);
        if(is_GUI)
            work->Dialog = Dialog;
        else
            work->console_progress = console_progress;
        
#ifdef WIN32
        work->generation_seed = Noise::rand_r(&custom_seed);
        work->rand_seed = Noise::rand_r(&custom_seed);
#else
        work->generation_seed = rand_r(&custom_seed);
        work->rand_seed = rand_r(&custom_seed);
#endif

        work->start_finger = start_fingerprint++;

        work->connect(tr, SIGNAL(started()), SLOT(generate()));
        tr->connect(work, SIGNAL(finish()), SLOT(quit()));
        connect(work, SIGNAL(finish()), SLOT(child_finished()));
        tr->start();
        if(is_GUI)
            Dialog->connect(work, SIGNAL(update(int)), SLOT(set_values(int)));
        else
            console_progress->connect(work, SIGNAL(update(int)), SLOT(set_values(int)));

        thread_pool.push_back(tr);
        work_pool.push_back(work);
    }

    while(!is_GUI && !thread_pool.empty())
    {
        QApplication::processEvents();
#ifdef WIN32
        _sleep(1000);
#else
        sleep(1);
#endif
    }
}

/*
  Starting point of Generating impressions. Creates the required directories.
  Creates the threads as per the setting, initializes the worker threads
  appropriately.
  In command line mode waits for each worker thread.
  */
void threaded_app::noise_start()
{
    int i;
    QDir dir;
    cout << endl;
    if(!dir.exists(path_noise_in.c_str()))
    {
        if(is_GUI)
        {
            Dialog->canceled();
        }
        showErr(QString("Path does not exists: ") + path_noise_in.c_str());
        emit finish();
        return;
    }
    
    if (create_dir(path_fing.c_str()) == false) {
        return;
    }
    
    if(!is_GUI)
        save_config(QString(path_fing.c_str()) + "/Anguli.conf");
    
    path_fing.append("/Impression");
    
    for(i = 1; i <= n_impr; i++)    {
        std::stringstream cmd;
        cmd << path_fing << "_" << i;
         if (create_dir(cmd.str().c_str()) == false) {
           return;
         }
     }
    
    QDir dire(path_noise_in.c_str());
    dire.setFilter(QDir::Dirs);
    QStringList entries = dire.entryList();
    no_fin = 0;
    for(QStringList::const_iterator iter = entries.begin(); iter != entries.end(); iter++)
    {
        QString file_name = *iter;
        if((entries.size() > 2 && file_name == ".") || file_name == "..")
            continue;
        QString str = dire.absolutePath() + "/" + file_name;
        QDir dir(str);
        dir.setFilter(QDir::Files);
        QStringList filterList;
        filterList << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp" << "*.tif" << "*.tiff";
        dir.setNameFilters(filterList);
        no_fin += dir.entryList().size();
    }
    if(no_fin == 0)
    {
        showErr("No Image Files!");
        return;
    }
    else
    {
        for(i = 1; i <= n_impr; i++)
        {
            std::stringstream cmd;
            cmd << path_fing << "_" << i;
            for(QStringList::ConstIterator entry=entries.begin(); entry != entries.end(); entry++)
            {
                QString name = *entry;
                if(name == "." || name == "..")
                    continue;
                QString path(cmd.str().c_str());
                path.append("/");
                path.append(name);
                if(create_dir(path.toStdString().c_str()) == false)
                {
                    return;
                }
            }
        }
    }

    
    if(is_GUI)
        Dialog->show();
    
    
    for(int i = 0; i < n_threads; i++)
    {
        noise_work *wthread = new noise_work();

        wthread->rotmax = ((double)rotation * 22) / (7 * 180);
        wthread->rotmin = -(wthread->rotmax);

        wthread->transmax = (double)translation;
        wthread->transmin = -(wthread->transmax);

        wthread->max_noiseLevel = max_noise;
        wthread->min_noiseLevel = min_noise;

        wthread->input_dir = path_noise_in.c_str();
        wthread->output_dir = path_fing.c_str();
        wthread->id = i;
        nwork_pool.push_back(wthread);
        QThreadEx *tr = new QThreadEx();
        wthread->moveToThread(tr);


        wthread->connect(tr, SIGNAL(started()), SLOT(generate_impressions()));
        tr->connect(wthread, SIGNAL(finish()), SLOT(quit()));
        connect(wthread, SIGNAL(finish()), SLOT(child_finished()));
        if(is_GUI)
            Dialog->connect(wthread, SIGNAL(update(int)), SLOT(set_values(int)));
        else
            console_progress->connect(wthread, SIGNAL(update(int)), SLOT(set_values(int)));

        wthread->Dialog = Dialog;
        thread_pool.push_back(tr);
        tr->start();
    }

    while(!is_GUI && !thread_pool.empty())
    {
        QApplication::processEvents();
#ifdef WIN32
        _sleep(1000);
#else
        sleep(1);
#endif
    }
}

/*
    Deletes the memory allocated for filters and noise blobs.
  */
void threaded_app::delete_filter() {
    int i, k;
    int freq_ind, orient_ind;
    max_filter_size = 0;
    int filter_size;

    for (freq_ind = 0; freq_ind < distnct_f; freq_ind++) {
            for (orient_ind = 0; orient_ind < distnct_o; orient_ind++) {

                    filter_size = filter_size_2Dmat[freq_ind][orient_ind];

                    for (k = 0; k < filter_size; k++)
                            delete(filterbank_4Dmat[freq_ind][orient_ind][k]);

                    delete(filterbank_4Dmat[freq_ind][orient_ind]);
            }
    }

    for (i = 0; i < distnct_f; i++)
            delete(filterbank_4Dmat[i]);

    delete(filterbank_4Dmat);
}

/*
  Saves the configuration in command line mode.
  */
void threaded_app::save_config(QString path)
{
    QFile file (path);
    if(!file.open(QIODevice::ReadWrite| QIODevice::Text))
    {
        showErr(QString("Cannot create config file ") + path);
        return;
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
    if(noise_only)
        finger_elem = docElements.elementsByTagName("ImpressionGen");
    QDomElement finger_node;
    if(finger_elem.size() == 0)
    {
        if(!noise_only)
            finger_node = doc.createElement("FingerprintGen");
        else
            finger_node = doc.createElement("ImpressionGen");
        root_node.appendChild(finger_node);
    }
    else
        finger_node = finger_elem.at(0).toElement();
    
    docElements = finger_node;
    QDomNodeList elements;
    QDomElement conf;
    if(!noise_only)
    {
        elements = docElements.elementsByTagName("NumFingerprints");
        if(elements.size() == 0)
        {
            conf = doc.createElement("NumFingerprints");
            conf.setAttribute("value", no_fin);
            finger_node.insertBefore(conf, QDomNode());
        }
        else
        {
            conf = elements.at(0).toElement();
            conf.setAttribute("value", no_fin);
        }
        
        elements = docElements.elementsByTagName("ClassDistr");
        if(elements.size() == 0)
        {
            conf = doc.createElement("ClassDistr");
            conf.setAttribute("value", class_distribution);
            finger_node.insertBefore(conf, QDomNode());
        }
        else
        {
            conf = elements.at(0).toElement();
            conf.setAttribute("value", class_distribution);
        }
        
        elements = docElements.elementsByTagName("StartFinger");
        if(elements.size() == 0)
        {
            conf = doc.createElement("StartFinger");
            conf.setAttribute("value", start_from_finger);
            finger_node.insertBefore(conf, QDomNode());
        }
        else
        {
            conf = elements.at(0).toElement();
            conf.setAttribute("value", start_from_finger);
        }
        
        elements = docElements.elementsByTagName("FingerPerDir");
        if(elements.size() == 0)
        {
            conf = doc.createElement("FingerPerDir");
            conf.setAttribute("value", size_fo);
            finger_node.insertBefore(conf, QDomNode());
        }
        else
        {
            conf = elements.at(0).toElement();
            conf.setAttribute("value", size_fo);
        }
        
        elements = docElements.elementsByTagName("ImageType");
        if(elements.size() == 0)
        {
            conf = doc.createElement("ImageType");
            conf.setAttribute("value", QString(image_type.c_str()));
            finger_node.insertBefore(conf, QDomNode());
        }
        else
        {
            conf = elements.at(0).toElement();
            conf.setAttribute("value", QString(image_type.c_str()));
        }
        
        elements = docElements.elementsByTagName("SaveMetaInfo");
        if(elements.size() == 0)
        {
            conf = doc.createElement("SaveMetaInfo");
            conf.setAttribute("value", save_meta);
            finger_node.insertBefore(conf, QDomNode());
        }
        else
        {
            conf = elements.at(0).toElement();
            conf.setAttribute("value", save_meta);
        }
        
        elements = docElements.elementsByTagName("customSeed");
        if(elements.size() == 0)
        {
            conf = doc.createElement("customSeed");
            conf.setAttribute("value", use_custom_seed);
            conf.setAttribute("seedValue", custom_seed);
            finger_node.insertBefore(conf, QDomNode());
        }
        else
        {
            conf = elements.at(0).toElement();
            conf.setAttribute("value", use_custom_seed);
            conf.setAttribute("seedValue", custom_seed);
        }
    }
    elements = docElements.elementsByTagName("ImprPerFinger");
    if(elements.size() == 0)
    {
        conf = doc.createElement("ImprPerFinger");
        conf.setAttribute("value", n_impr);
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", n_impr);
    }
    
    elements = docElements.elementsByTagName("OutDir");
    if(elements.size() == 0)
    {
        conf = doc.createElement("OutDir");
        conf.setAttribute("value", QString(path_fing.c_str()));
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", QString(path_fing.c_str()));
    }

    elements = docElements.elementsByTagName("NumThreads");
    if(elements.size() == 0)
    {
        conf = doc.createElement("NumThreads");
        conf.setAttribute("value", n_threads);
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", n_threads);
    }
    
    

    elements = docElements.elementsByTagName("NoiseLevel");
    if(elements.size() == 0)
    {
        conf = doc.createElement("NoiseLevel");
        conf.setAttribute("low", min_noise);
        conf.setAttribute("high", max_noise);
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("low", min_noise);
        conf.setAttribute("high", max_noise);
    }

    elements = docElements.elementsByTagName("NumScratches");
    if(elements.size() == 0)
    {
        conf = doc.createElement("NumScratches");
        conf.setAttribute("low", min_scratches);
        conf.setAttribute("high", max_scratches);
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("low", min_scratches);
        conf.setAttribute("high", max_scratches);
    }

    elements = docElements.elementsByTagName("Rotation");
    if(elements.size() == 0)
    {
        conf = doc.createElement("Rotation");
        conf.setAttribute("value", rotation);
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", rotation);
    }

    elements = docElements.elementsByTagName("Translation");
    if(elements.size() == 0)
    {
        conf = doc.createElement("Translation");
        conf.setAttribute("value", translation);
        finger_node.insertBefore(conf, QDomNode());
    }
    else
    {
        conf = elements.at(0).toElement();
        conf.setAttribute("value", translation);
    }
    
    file.seek(0);
    file.resize(0);
    QTextStream stream(&file);

    stream << doc.toString();
    file.close();
}
