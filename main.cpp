#include "dialog.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec *cyrillicCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(cyrillicCodec);
    QApplication a(argc, argv);
    Dialog w;
    w.show();
    return a.exec();
}
