#ifndef INFO_DIALOG_H
#define INFO_DIALOG_H

#include <QDialog>
#include <QString>
#include <QDir>
#include <QStandardPaths>
#include <QDirIterator>
#include <QProcess>

namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = nullptr);
    ~InfoDialog();

private slots:

private:
    Ui::InfoDialog *ui;

};


#endif // INFO_DIALOG_H
