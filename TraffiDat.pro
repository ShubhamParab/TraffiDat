#-------------------------------------------------
#
# Project created by QtCreator 2019-02-10T16:04:20
#
#-------------------------------------------------

QT       += core gui sql multimediawidgets multimedia datavisualization widgets
QXT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TraffiDat
TEMPLATE = app

include ( ${QWT_ROOT}/qwt.prf )

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    database.cpp \
    addroaddialog.cpp \
    addtrafficlightdialog.cpp \
    addjunctiondialog.cpp \
    dumptestroaddialog.cpp \
    dumptesttrafficlightdialog.cpp \
    structtreeitem.cpp \
    structtreemodel.cpp \
    roadwidget.cpp \
    Section.cpp \
    mapgraphicsscene.cpp \
    mapmodel.cpp \
    junctiongraphicsitem.cpp \
    roadgraphicsitem.cpp \
    detection.cpp \
    tracker/AssignmentProblemSolver.cpp \
    tracker/BackgroundSubtractor.cpp \
    tracker/Detector.cpp \
    tracker/TrackerKalman.cpp \
    tracker/Transformation.cpp \
    tracker/VehicleTracker.cpp \
    analyticswizard.cpp \
    statswizardpage.cpp \
    mutroadwidget.cpp \
    mutroadtablemodel.cpp \
    road2trafflwizardpage.cpp \
    road2traffltablemodel.cpp \
    trafflitemdelegate.cpp \
    model.cpp \
    traffltimerwizardpage.cpp \
    traffictimertablemodel.cpp \
    traffltimeritemdelegate.cpp \
    visualgraphdialog.cpp \
    sessionreport.cpp \
    roaddetailtablemodel.cpp

HEADERS += \
        mainwindow.h \
    model.h \
    database.h \
    addroaddialog.h \
    addtrafficlightdialog.h \
    addjunctiondialog.h \
    constants.h \
    dumptestroaddialog.h \
    dumptesttrafficlightdialog.h \
    structtreeitem.h \
    structtreemodel.h \
    roadwidget.h \
    Section.h \
    mapgraphicsscene.h \
    mapmodel.h \
    junctiongraphicsitem.h \
    roadgraphicsitem.h \
    detection.h \
    tracker/AssignmentProblemSolver.hpp \
    tracker/BackgroundSubtractor.hpp \
    tracker/Detector.hpp \
    tracker/TrackerKalman.hpp \
    tracker/Transformation.hpp \
    tracker/VehicleTracker.hpp \
    analyticswizard.h \
    statswizardpage.h \
    mutroadwidget.h \
    mutroadtablemodel.h \
    road2trafflwizardpage.h \
    road2traffltablemodel.h \
    trafflitemdelegate.h \
    traffltimerwizardpage.h \
    traffictimertablemodel.h \
    traffltimeritemdelegate.h \
    visualgraphdialog.h \
    sessionreport.h \
    roaddetailtablemodel.h

FORMS += \
        mainwindow.ui \
    addroaddialog.ui \
    addtrafficlightdialog.ui \
    addjunctiondialog.ui \
    dumptesttrafficlightdialog.ui \
    dumptestroaddialog.ui \
    roadwidget.ui \
    statswizardpage.ui \
    mutroadwidget.ui \
    road2trafflwizardpage.ui \
    traffltimerwizardpage.ui \
    visualgraphdialog.ui \
    configdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lopencv_world401
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lopencv_world401d

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

RESOURCES +=

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Downloads/qwt-master/build-qwt-Desktop_Qt_5_12_0_MSVC2015_64bit-Release/lib/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Downloads/qwt-master/build-qwt-Desktop_Qt_5_12_0_MSVC2015_64bit-Release/lib/ -lqwtd

INCLUDEPATH += $$PWD/../../../Downloads/qwt-master/qwt-master/src
DEPENDPATH += $$PWD/../../../Downloads/qwt-master/qwt-master/src

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Downloads/qwtplot3d-master/build-qwtplot3d-Desktop_Qt_5_12_0_MSVC2015_64bit-Release/lib/ -lqwtplot3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Downloads/qwtplot3d-master/build-qwtplot3d-Desktop_Qt_5_12_0_MSVC2015_64bit-Release/lib/ -lqwtplot3d

INCLUDEPATH += $$PWD/../../../Downloads/qwtplot3d-master/qwtplot3d-master/include
DEPENDPATH += $$PWD/../../../Downloads/qwtplot3d-master/qwtplot3d-master/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Downloads/kdreports-1.8.0/build-kdreports-Desktop_Qt_5_12_0_MSVC2015_64bit-Release/lib/ -lkdreports1
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Downloads/kdreports-1.8.0/build-kdreports-Desktop_Qt_5_12_0_MSVC2015_64bit-Release/lib/ -lkdreports1d

INCLUDEPATH += $$PWD/../../../Downloads/kdreports-1.8.0/kdreports-1.8.0/include/KDReports
DEPENDPATH += $$PWD/../../../Downloads/kdreports-1.8.0/kdreports-1.8.0/include/KDReports
