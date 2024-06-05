#include "WordEditor.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WordEditor w;
    w.show();
    return a.exec();
}
