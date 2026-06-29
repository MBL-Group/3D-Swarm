QT       += core gui
QT += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cameras.cpp \
    canvas.cpp \
    coils.cpp \
    main.cpp \
    mainwindow.cpp \
    mp285.cpp \
    ni6343.cpp

HEADERS += \
    cameras.h \
    canvas.h \
    coils.h \
    mainwindow.h \
    mp285.h \
    ni6343.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../third_libs/opencv-4.7.0-windows/opencv/build/x64/vc16/lib/ -lopencv_world470
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../third_libs/opencv-4.7.0-windows/opencv/build/x64/vc16/lib/ -lopencv_world470d

INCLUDEPATH += $$PWD/../third_libs/opencv-4.7.0-windows/opencv/build/include
DEPENDPATH += $$PWD/../third_libs/opencv-4.7.0-windows/opencv/build/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../third_libs/nidaqmx/ExternalCompilerSupport/C/lib64/msvc/ -lNIDAQmx
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../third_libs/nidaqmx/ExternalCompilerSupport/C/lib64/msvc/ -lNIDAQmx

INCLUDEPATH += $$PWD/../third_libs/nidaqmx/ExternalCompilerSupport/C/include
DEPENDPATH += $$PWD/../third_libs/nidaqmx/ExternalCompilerSupport/C/include
