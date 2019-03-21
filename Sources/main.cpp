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

#define MAIN
#include "global.h"
#include "Noise/global.h"

#include <QApplication>
#include <QLayout>
#include <QPixmap>
#include <QSplashScreen>
#include "mainwindow.h"

#include "cv_type_converter.h"

#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <QtXml/QDomDocument>
#include <QTextStream>
#include <QFile>
#include <time.h>
#include <QMessageBox>

#ifdef WIN32
#define _WIN32_WINNT 0x500
#include <wincon.h>
#include <winuser.h>
#endif

using namespace anguli;
using namespace std;

/*
*  Shows help for command line.
*/
void show_help() {
    cout << "Usage: 1. Anguli -num <number> [options]" << endl;
    cout << "       2. Anguli -indir <path> -impr -ni <number> [options]" << endl << endl;
    cout << "   -h                              : Show help" << endl;
    cout << "   -num     <number>               : Number of Fingerprints" << endl;
    cout << "   -npd     <number>               : Number of Fingerprints per directory" << endl;
    cout << "   -cdist   <class>                : Fingerprint Class Distribution" << endl;
    cout << "   -meta                           : Save meta information" << endl;
    cout << "   -ni      <number>               : Number of Impressions per Fingerprint" << endl;
    cout << "   -outdir  <path>                 : Directory for saving Fingerprints" << endl;
    cout << "   -numT    <number>               : Number of Threads" << endl;
    cout << "   -seed    <number>               : Seed value for random generator" << endl;
    cout << "   -scratch '<number> <number>'    : Number of scratches " << endl;
    cout << "   -noise   '<number> <number>'    : Noise level from 0 to 8" << endl;
    cout << "   -rot     <number>               : Rotation" << endl;
    cout << "   -trans   <number>               : Translation" << endl;
    cout << "   -itype   <image type extension> : Type of image in which fingerprint is to be saved" << endl;
    cout << "   -indir   <path>                 : Directory containing Fingerprints (Used with -impr option)" << endl;
    cout << "   -impr                           : Generate impressions Only" << endl;
    cout << "   -cfg     <path>                 : Configuration file to be used" << endl;

    cout << "" << endl;
    cout << "\nFingerprint Class Distribution" << endl;
    cout << "   natural : Natural Distribution" << endl;
    cout << "   arch    : Arch Only" << endl;
    cout << "   tarch   : Tented Arch Only" << endl;
    cout << "   right   : Right Only" << endl;
    cout << "   left    : Left Only" << endl;
    cout << "   dloop   : Double Loop Only" << endl;
    cout << "   whirl   : Whirl Only" << endl << endl;

    /*cout << "" << endl;
    cout << "\nImage Types" << endl;
    cout << " jpg" << endl;
    cout << " jpeg" << endl;
    cout << " png" << endl;
    cout << " bmp" << endl;
    cout << " tif" << endl;
    cout << " tiff" << endl;*/
    exit(0);
}

/*
  Reads the configuration file given in command line.
  and initiliazes parameters accordingly.
  */
void load_configurationFile()
{
    QFile file (cfg_path.c_str());
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cout << "Wrong Configuration File!" << endl;
        return;
    }
    QDomDocument doc;
    doc.setContent(&file);
    QDomElement docElements = doc.documentElement();
    QDomNodeList finger_elem = docElements.elementsByTagName("FingerprintGen");
    if(finger_elem.size() == 0)
    {
        cout << "Wrong Configuration File!" << endl;
        return;
    }

    docElements = finger_elem.at(0).toElement();
    QDomNodeList elements;
    QDomElement conf;
    elements = docElements.elementsByTagName("NumFingerprints");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        no_fin = (atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("ImprPerFinger");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        n_impr = (atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("ClassDistr");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        class_distribution = (atoi(conf.attribute("value", "0").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("OutDir");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        path_fing = conf.attribute("value", "./Fingerprints").toStdString();
    }
    
    elements = docElements.elementsByTagName("StartFinger");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        start_from_finger = (atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("FingerPerDir");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        size_fo = (atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("ImageType");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        int x = (atoi(conf.attribute("value", "0").toStdString().c_str()));
        if(x == 0)
            image_type = ".jpg";
        else if(x == 1)
            image_type = ".jpeg";
        else if(x == 2)
            image_type = ".bmp";
        else if(x == 3)
            image_type = ".png";
        else if(x == 4)
            image_type = ".tif";
        else if(x == 5)
            image_type = ".tiff";
    }

    elements = docElements.elementsByTagName("NumThreads");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        n_threads = (atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("SaveMetaInfo");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        save_meta = (atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
    
    elements = docElements.elementsByTagName("customSeed");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        use_custom_seed = (atoi(conf.attribute("value", "1").toStdString().c_str()));
        custom_seed = (atoi(conf.attribute("seedValue", "1").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("NoiseLevel");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        min_noise = (atoi(conf.attribute("low", "1").toStdString().c_str()));
        max_noise = (atoi(conf.attribute("high", "1").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("NumScratches");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        min_scratches = (atoi(conf.attribute("low", "1").toStdString().c_str()));
        max_scratches = (atoi(conf.attribute("high", "1").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("Rotation");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        rotation = (atoi(conf.attribute("value", "1").toStdString().c_str()));
    }

    elements = docElements.elementsByTagName("Translation");
    if(elements.size() != 0)
    {
        conf = elements.at(0).toElement();
        translation = (atoi(conf.attribute("value", "1").toStdString().c_str()));
    }
}

/*
  Processes command line arguments.
*/
void processArgs(int argc, char **arg) {
    int i;
    if (argc == 1)
        return;
    
    in_dir_used = false;
    custom_seed = time(NULL);

    
    for (i = 1; i < argc; i++) {
        if (!strcmp(arg[i], "-cfg")) {
            use_cfg = true;
            cfg_path = string(arg[++i]);
            break;
        }
    }

    if(use_cfg)
    {
        load_configurationFile();
    }
    
    for (i = 1; i < argc; i++) {

        if (!strcmp(arg[i], "-h")) {
            show_help();
        }
        else if (!strcmp(arg[i], "-cfg"))
        {
            i++;
            continue;
        }
        else if(!strcmp(arg[i], "-cdist"))
        {
            i++;
            switch(arg[i][0])
            {
            case 'n':
            case 'N':
                class_distribution = 0;
                break;
            case 'a' :
            case 'A':
                class_distribution = 1;
                break;
            case 't':
            case 'T' :
                class_distribution = 2;
                break;
            case 'r':
            case 'R':
                class_distribution = 3;
                break;
            case 'l':
            case 'L':
                class_distribution = 4;
                break;
            case 'd':
            case 'D':
                class_distribution = 5;
                break;
            case 'w':
            case 'W':
                class_distribution = 6;
                break;
            default:
                cout << "\nInvalid argument for -cdist options\n" << endl;
                exit(0);
            }
        
        }
        else if (!strcmp(arg[i], "-num")) {
            no_fin = atoi(arg[++i]);
        } else if (!strcmp(arg[i], "-npd")) {
            size_fo = atoi(arg[++i]);
        } else if (!strcmp(arg[i], "-ni")) {
            n_impr = atoi(arg[++i]);
        } else if (!strcmp(arg[i], "-outdir")) {
            path_fing.clear();
            path_fing.append(arg[++i]);
        } else if (!strcmp(arg[i], "-meta")) {
            save_meta = true;
        } else if (!strcmp(arg[i], "-numT")) {
            n_threads = atoi(arg[++i]);
        }
        else if (!strcmp(arg[i], "-indir")) {
            path_noise_in.clear();
            path_noise_in.append(arg[++i]);
            in_dir_used = true;
        }
        else if (!strcmp(arg[i], "-impr")) {
            noise_only = true;
            path_fing =  "./Impressions";
            if(use_cfg)
            {
                cout << "Error: -impr cannon be used with -cfg" << endl;
                exit(0);
             }
            //strncpy(path_fing, "./Impressions", 50);
            //path_fing.append()
        }
        else if (!strcmp(arg[i], "-seed")) {
            use_custom_seed = true;
            custom_seed = atoi(arg[++i]);
        }
        else if (!strcmp(arg[i], "-scratch")) {
            std::stringstream str;
            str << arg[++i];
            str >> min_scratches;
            str >> max_scratches;
            if(min_scratches > max_scratches)
            {
                int tmp = min_scratches;
                min_scratches = max_scratches;
                max_scratches = tmp;
            }
        }
        else if (!strcmp(arg[i], "-noise")) {
            std::stringstream str;
            str << arg[++i];
            str >> min_noise;
            str >> max_noise;
            if(min_noise > max_noise)
            {
                int tmp = min_noise;
                min_noise = max_noise;
                max_noise = tmp;
            }
        }
        else if (!strcmp(arg[i], "-rot")) {
            std::stringstream str;
            str << arg[++i];
            rotation = 0;
            str >> rotation;
        }
        else if (!strcmp(arg[i], "-trans")) {
            std::stringstream str;
            str << arg[++i];
            translation = 0;
            str >> translation;
        }
        else if (!strcmp(arg[i], "-itype")) {
            std::stringstream str;
            str << arg[++i];
            image_type = "jpg";
            str >> image_type;
        }
        else
            show_help();
    }
    if(in_dir_used && !noise_only)
    {
        cout << "Error: -indir used only with -impr!\n\n";
        exit(0);
    }
    if(noise_only && !in_dir_used)
    {
        cout << "Error: -impr used with -indir!\n\n";
        exit (0);
    }
}

/*
  Loads noise blobs and filter banks.
*/

void load_filter_blobs(MainWindow* w) {
    int freq_ind, orient_ind;
    //char s1[50];
    max_filter_size = 0;
    int filter_size;
    IplImage* filter;
    CvMat* mat;
        
     if(!noise_only) {
        filterbank_4Dmat = (int****) malloc((distnct_f) * sizeof(int***));
        
        for (int i = 0; i < distnct_f; i++)
            filterbank_4Dmat[i] = (int***) malloc((distnct_o) * sizeof(int**));
        
        for (freq_ind = 0; freq_ind < distnct_f; freq_ind++) {
            for (orient_ind = 0; orient_ind < distnct_o; orient_ind++) {
        
                //sprintf(s1, "%s/%d/%d.bmp", path_filter, freq_ind + 1, orient_ind + 1);
                std::stringstream s1;
                s1 << path_filter << "/" << freq_ind + 1 << "/" << orient_ind + 1 << ".bmp";
                if(!QFile::exists(s1.str().c_str()))
                {
                    if(is_GUI)
                        QMessageBox::information(w, "Anguli", "Not able to load Filter Bank.",
                                         QMessageBox::Ok, QMessageBox::Ok);
                    else
                        cout << "Error: Not able to load Filter Bank." << endl;
                    exit(0);
                }
                //cout << s1.str() << endl;
                filter = cvLoadImage(s1.str().c_str(), CV_LOAD_IMAGE_GRAYSCALE);
                mat = cvCreateMat(filter->height, filter->width, CV_32FC1 );
                ipl8uc1_to_cv32fc1(filter, mat);
            
                cvConvertScale(mat, mat, 1 / 255.0, 0);
                normalize32(mat);
                cvConvertScale(mat, mat, 100, -46);
            
            
                filter_size = filter->height;
                filter_size_2Dmat[freq_ind][orient_ind] = filter_size;

        
                filterbank_4Dmat[freq_ind][orient_ind] = (int**) malloc(
                    (filter_size) * sizeof(float*));
                for (int k = 0; k < filter_size; k++)
                    filterbank_4Dmat[freq_ind][orient_ind][k] = (int*) malloc(
                        (filter_size) * sizeof(float));
            
                cvmattomat2D(mat, filterbank_4Dmat[freq_ind][orient_ind]);
            
                if (filter_size > max_filter_size)
                    max_filter_size = filter_size;
            
                cvReleaseImage(&filter);
                cvReleaseMat(&mat);
            }
        }
    }
    
     if(!QFile::exists("noise_blob/1.png"))
     {
         if(is_GUI)
             QMessageBox::information(w, "Anguli", "Not able to load Noise Blobs.",
                              QMessageBox::Ok, QMessageBox::Ok);
         else
             cout << "Error: Not able to load Noise Blobs." << endl;
         exit(0);
     }

    if(!noise_only && !noise_blobs_loaded)
    {
        noise_blob[1] = cvLoadImage("noise_blob/1.png", 0);
        noise_blob[2] = cvLoadImage("noise_blob/2.png", 0);
        noise_blob[3] = cvLoadImage("noise_blob/3.png", 0);
        noise_blob[4] = cvLoadImage("noise_blob/4.png", 0);
        noise_blob[5] = cvLoadImage("noise_blob/5.png", 0);
        noise_blob[6] = cvLoadImage("noise_blob/6.png", 0);
        noise_blob[7] = cvLoadImage("noise_blob/7.png", 0);
        noise_blob[8] = cvLoadImage("noise_blob/8.png", 0);
        noise_blob[9] = cvLoadImage("noise_blob/9.png", 0);
        noise_blob[10] = cvLoadImage("noise_blob/10.png", 0);
        noise_blobs_loaded = true;
    }

    if(!noise::noise_blobs_loaded)
    {
        noise::noise_blob[1] = cvLoadImage("noise_blob/1.png", 0);
        noise::noise_blob[2] = cvLoadImage("noise_blob/2.png", 0);
        noise::noise_blob[3] = cvLoadImage("noise_blob/3.png", 0);
        noise::noise_blob[4] = cvLoadImage("noise_blob/4.png", 0);
        noise::noise_blob[5] = cvLoadImage("noise_blob/5.png", 0);
        noise::noise_blob[6] = cvLoadImage("noise_blob/6.png", 0);
        noise::noise_blob[7] = cvLoadImage("noise_blob/7.png", 0);
        noise::noise_blob[8] = cvLoadImage("noise_blob/8.png", 0);
        noise::noise_blob[9] = cvLoadImage("noise_blob/9.png", 0);
        noise::noise_blob[10] = cvLoadImage("noise_blob/10.png", 0);
        noise::noise_blobs_loaded = true;
    }
}

/*
  Initializes global variables.
*/
void initialize()
{
    flag_plot = 0;
    flag_debug = 0;
    k = 16;
    k_fact = k;
    block_size = k;
    margin = 30;
    padding = 15;

    W = 275 - margin;
    H = 400 - margin;
    line_length = 1 * k;

    distnct_f = 100;
    distnct_o = 180;
    n_threads = 1;

    path_density_maps = "./Densitymaps";
    path_fing = "./FingerPrints";
    path_filter = "./Filterbank";
    image_type = "jpg";

    size_fo = 1000;
    start_from_finger = 1;
    class_distribution = 0;
    n_impr = 0;
    no_fin = 1;
    n_threads = 1;
    generated = 0;

    max_noise = min_noise = 0;
    max_scratches = min_scratches = 0;
    rotation = 0;
    translation = 0;
}


int main(int argc, char *argv[])
{
   if(argc == 1)
    {
#ifdef WIN32
       HWND hWnd = GetConsoleWindow();
       ShowWindow(hWnd, SW_HIDE );
#endif

       // Anguli in GUI mode
        is_GUI = true;
        QApplication a(argc, argv);
        //a.setStyle("motif");
        //QPalette palet = a.palette();
        a.setStyleSheet("QToolTip {\
            border: 1px solid darkgray;\
            padding: 2px;\
            border-radius: 5px;\
            opacity: 200;\
            background-color: gray;\
            color: white; \
        }");
        
        
        //a.setStyle(new QMotifStyle);
        //a.setStyle(new QPlastiqueStyle);
        //a.setStyle(new QWindowsVistaStyle);
        //a.setStyle(new QCleanlooksStyle);

        QPixmap pixmap(":images/Images/splash_screen.png");
        QSplashScreen splash(pixmap);
        splash.show();
        MainWindow w;
        w.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint| Qt::WindowSystemMenuHint);
        initialize();
        load_filter_blobs(&w);
        //sleep(6);
        w.show();
        splash.finish(&w);
        return a.exec();
    }
    else
    {
        // Anguli in command line mode
        QApplication a(argc, argv);
        is_GUI = false;
        start_time = QDateTime::currentDateTime();
        threaded_app app;
        initialize();
        processArgs(argc, argv);
        load_filter_blobs(NULL);
        
        console_progressDialog Dialog;
        app.setConsoleProgress(&Dialog);
        if(noise_only)
            app.noise_start();
        else
            app.start();
    }
}
