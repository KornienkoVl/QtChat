#include "auth.h"
#include "ui_auth.h"

Auth::Auth(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Auth)
{
    ui->setupUi(this);
}

Auth::~Auth()
{
    delete ui;
}

void Auth::on_LoginButton_clicked()
{
    QMessageBox msgBox;
    QString login;
    QString password;
    qint16 type;
    if(ui->checkBox->isChecked())
    {
        type = 0;
    }
    else
    {
        type = 1;
    }

    login = ui->LoginEdit->text();  
    password = ui->PasswordEdit->text();

    if(login == "Общее")
    {
        msgBox.setText("Имя пользователя недоступно.");
        msgBox.exec();
    }
    else if(login=="" || password == "")
    {
        msgBox.setText("Поля не могут быть пустыми.");
        msgBox.exec();
    }
    else emit signal(type,login,password);
}

