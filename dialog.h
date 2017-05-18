#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    void parseFile(QString filepath, int fileCount);

private slots:
    void on_addButton_clicked();

    void on_deleteButton_clicked();

    void on_okButton_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
