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

# Updated 2019-03 Wolfgang Frisch

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Anguli
TEMPLATE = app
HEADERS += Headers/threading.h \
    Headers/orientmap.h \
    Headers/noise.h \
    Headers/mainwindow.h \
    Headers/impression_window.h \
    Headers/global.h \
    Headers/generation.h \
    Headers/g_cap.h \
    Headers/fingerprint_window.h \
    Headers/filter_image.h \
    Headers/dialog.h \
    Headers/densitymap.h \
    Headers/cv_type_converter.h \
    Headers/ahaque_complex.h \
    Headers/Noise/n_noise.h \
    Headers/Noise/n_cv_type_converter.h \
    Headers/Noise/global.h
SOURCES += Sources/threading.cpp \
    Sources/orientmap.cpp \
    Sources/noise.cpp \
    Sources/mainwindow.cpp \
    Sources/main.cpp \
    Sources/impression_window.cpp \
    Sources/generation.cpp \
    Sources/g_cap.cpp \
    Sources/fingerprint_window.cpp \
    Sources/filter_image.cpp \
    Sources/dialog.cpp \
    Sources/densitymap.cpp \
    Sources/cv_type_converter.cpp \
    Sources/ahaque_complex.cpp \
    Sources/Noise/n_noise.cpp \
    Sources/Noise/n_cv_type_converter.cpp
FORMS += Forms/mainwindow.ui \
    Forms/impression.ui \
    Forms/Generator.ui \
    Forms/about.ui
INCLUDEPATH += ./Include/ \
     ./Include/Qxt/QxtGui \
     ./Include/Qxt/QxtCore \
    ./Headers \
    ./Headers/Noise
CONFIG(debug, debug|release):DESTDIR = debug
else:DESTDIR = release
OBJECTS_DIR = $$DESTDIR/build
MOC_DIR = $$DESTDIR/moc
RCC_DIR = $$DESTDIR/rcc
UI_DIR = $$DESTDIR/ui

linux-g++ { 
    LIBS += `pkg-config --libs opencv`
    LIBS += /usr/lib/libqxt-tulip-4.8.so.0
}

QT += xml
RESOURCES += Resources/Anguli.qrc
CONFIG  += qxt
QXT     += core gui

DISTFILES += \
    README.md

