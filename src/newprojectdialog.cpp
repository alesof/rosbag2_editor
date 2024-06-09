#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

NewProjectDialog::NewProjectDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
    setWindowTitle("Info");

}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}


