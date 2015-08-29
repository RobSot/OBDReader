greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport
} else {
    include($$QTSERIALPORT_PROJECT_ROOT/src/serialport/qt4support/serialport.prf)
}

TARGET = OBDReader_GUI
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    console.cpp \
    obd.cpp \
    pidselectdialog.cpp

HEADERS += \
    mainwindow.h \
    settingsdialog.h \
    console.h \
    defines.h \
    obd.h \
    pidselectdialog.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui \
    pidselectdialog.ui

RESOURCES += \
    OBDReader_GUI.qrc

RC_FILE = OBDReader.rc
