#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QString>
#include <QDir>
#include <QStandardPaths>
#include <QDirIterator>
#include <QProcess>

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = nullptr);
    ~NewProjectDialog();

private slots:

private:
    Ui::NewProjectDialog *ui;

};


#endif // NEWPROJECTDIALOG_H
