#include "dialog.h"
#include "ui_dialog.h"
#include <QString>
#include <QFileDialog>
#include <QTextStream>
#include <QRegExp>
#include <string>
#include <QDebug>
#include <QTextCodec>

using namespace std;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_addButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Document"),
                        QDir::currentPath(), tr("All files (*.*)"));
    ui->filesListWidget->addItem(filename);
}

void Dialog::on_deleteButton_clicked()
{
    delete ui->filesListWidget->currentItem();
}

void Dialog::on_okButton_clicked()
{
    for(int i = 0, k = 0; i < ui->filesListWidget->count(); i++)
    {
        if(ui->filesListWidget->item(i)->text() != "")
        {
            parseFile(ui->filesListWidget->item(i)->text(), k);
            k++;
        }
    }
}

void Dialog::parseFile(QString filepath, int fileCount)
{
    QFile file(filepath);
    QString fileText;
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream readFileStream(&file);
        readFileStream.setCodec(QTextCodec::codecForName("Windows-1251"));
        fileText = readFileStream.readAll();
        if(readFileStream.status()!= QTextStream::Ok)
        {
            qDebug() << "Ошибка чтения файла";
        }
    }

    QRegExp stationRegExp("([^.]t=)([А-я0-9-\\s]*)");
    QStringList stationList;
    int pos = 0;
    while((pos = stationRegExp.indexIn(fileText, pos)) != -1)
    {
        stationList << stationRegExp.cap(2);
        pos += stationRegExp.matchedLength();
    }
    stationList.sort();
    for(auto it = stationList.begin(); it !=stationList.end()-1;)
    {
        if(*it == *(it+1))
        {
            stationList.erase(it);
        } else it++;
    }

    QRegExp dateRegExp("(^[d]:)([0-9]*)(,)([M]:)([0-9]*)");
    QString dateString;
    pos = 0;
    if((pos = dateRegExp.indexIn(fileText, pos)) != -1)
    {
        dateString = dateRegExp.cap(5) + "-" + dateRegExp.cap(2);
    }

    QRegExp timeRegExp("(^[d]:[0-9]*,[M]:[0-9]*,)([h]:)([0-9]*)(,)([m])(:[0-9]*)(,)([s])(:[0-9]*)");
    QString hourString;
    pos = 0;
    if((pos = timeRegExp.indexIn(fileText, pos)) != -1)
    {
        hourString = timeRegExp.cap(3);
    }

    QRegExp nextTimeRegExp("(((\\nm|m)(:[0-9]*)(,s)(:[0-9]*)(,))|((\\ns|s)(:[0-9]*)(,)))([^]t=)([А-я0-9-\\s]*)([А-яA-z0-9,=-\\s?\\._]*,)");
    pos = 0;
    int position;
    int count = 0;
    QString minute;
    QString seconds;
    QFile resultFile("result" + QString::number(fileCount) + ".txt");
    QTextStream writeFileStream(&resultFile);
    resultFile.open(QIODevice::WriteOnly);
    while(count < stationList.length())
    {
        while((pos = nextTimeRegExp.indexIn(fileText, pos)) != -1)
        {
            QRegExp stationAndPulsesRegExp("([^.]t=)([А-я0-9-\\s]*)(,)([А-яA-Z0-9,=-\\s?\\._\\\\]*)");
            position = 0;
            while((position = stationAndPulsesRegExp.indexIn(nextTimeRegExp.cap(13), position)) != -1)
            {
                if(nextTimeRegExp.cap(4) != "")
                    minute = nextTimeRegExp.cap(4);
                if(stationList.at(count) == stationAndPulsesRegExp.cap(2))
                {
                    if(nextTimeRegExp.cap(6) == "")
                        seconds = nextTimeRegExp.cap(10);
                    else seconds = nextTimeRegExp.cap(6);
                    writeFileStream << "\n" + stationAndPulsesRegExp.cap(2) + " " + dateString + " " + hourString + minute +
                                seconds + " --- " + stationAndPulsesRegExp.cap(4);
                    break;
                } else position += stationAndPulsesRegExp.matchedLength();
            }
            pos += nextTimeRegExp.matchedLength();
        }
        count++;
        pos = 0;
    }

    file.close();
    resultFile.close();
}
