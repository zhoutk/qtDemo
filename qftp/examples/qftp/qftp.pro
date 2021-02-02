HEADERS       = ftpwindow.h
SOURCES       = ftpwindow.cpp \
                main.cpp
RESOURCES    += ftp.qrc
QT           += network widgets

CONFIG(debug, debug|release) {
    LIBS += -lQt5Ftpd
} else {
    LIBS += -lQt5Ftp
}

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtbase/network/qftp
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/qtbase/network/qftp
INSTALLS += target sources
