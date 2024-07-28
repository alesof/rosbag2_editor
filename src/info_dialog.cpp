#include "info_dialog.h"
#include "ui_info_dialog.h"

InfoDialog::InfoDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InfoDialog)
{
    ui->setupUi(this);
    setWindowTitle("Info");

}

InfoDialog::~InfoDialog()
{
    delete ui;
}
