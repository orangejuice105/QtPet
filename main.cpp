#include "petwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PetWindow w;
    w.show();
    return QApplication::exec();
}
