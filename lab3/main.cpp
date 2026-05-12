#include <QApplication>
#include "view/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("File Manager");
    app.setApplicationDisplayName("Учет файлов");

    MainWindow window;
    window.show();

    return app.exec();
}