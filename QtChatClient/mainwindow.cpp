#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    nextBlockSize = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ConnectButton_clicked()
{
    form = new Auth;
    connect(form, &Auth::signal, this, &MainWindow::slotAuth);
    form->show();
    this->setDisabled(true);
}

void MainWindow::SendToServer(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_7);
    //выделяем 2 байта для размера блока сообщения 
    out << quint16(0);

    //типы сообщений 0-регистрация, 1-вход, 2-общее сообщение, 3-личное сообщение
    switch (msgType)
    {
    case 0:
        out << quint16(0) << Login << Password;
    break;

    case 1:
        out << quint16(1) << Login << Password;
    break;

    case 2:
        out << quint16(2) << QTime::currentTime() << Login << str;
    break;

    case 3:
        QString SendTo = ui->SendToEdit->text();
       out << quint16(3) << QTime::currentTime() << Login << SendTo << str;
    break;
    }

    //переходим в начало блока
    out.device()->seek(0);
    //записываем размер сообщения
    out << quint16(Data.size() - sizeof(quint16));

    socket->write(Data);
    ui->lineEdit->clear();
}

void MainWindow::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_7);
    if(in.status()==QDataStream::Ok)
    {
        for(;;)
       {
           //размер блока неизвестен
           if(nextBlockSize == 0)
            {
               //для чтения должны быть доступны как минимум 2 байта
                if(socket->bytesAvailable() < 2)
               {
                   break;
               }
                in >> nextBlockSize;
            }
           //проверка получения всего сообщения
           if(socket->bytesAvailable() < nextBlockSize)
            {
                break;
            }
        QMessageBox msgBox;
        QString str;
        QTime time;
        quint16 ServermsgType;
        in >> ServermsgType;
       //типы сообщений 0-успешная регистрация, 1-ошибка регистрации, 2-успешный вход, 3-ошибка входа, 4-общее сообщение, 5-личное сообщение, 6-адресат не в сети
        switch(ServermsgType)
        {
        case 0:
            form->hide();
            this->setDisabled(false);
            ui->textBrowser->clear();
            ui->ConnectButton->setDisabled(true);
            ui->ConnectButton->setText("Connected as "+ Login);
            break;

        case 1:
            msgBox.setText("Имя пользователя недоступно.");
            msgBox.exec();
            break;

        case 2:
            form->hide();
            this->setDisabled(false);
            ui->textBrowser->clear();
            ui->ConnectButton->setDisabled(true);
            ui->ConnectButton->setText("Connected as "+ Login);
            break;

        case 3:
            msgBox.setText("Неверное имя пользователя или пароль.");
            msgBox.exec();
            break;

        case 4:
            in >> time >> str;
             ui->textBrowser->append(time.toString() + " " + str);
            break;

        case 5:
            in >> time >> str;
            ui->textBrowser->append(time.toString() + " " + str);
            break;

        case 6:
          ui->textBrowser->append("Пользователь не в сети");
            break;
        }
        nextBlockSize = 0;
       }
    }
    else
    {
        ui->textBrowser->append("read error");
    }
}
//получение значений логина и пароля, типа операции (регистрация или вход), отправка данных на сервер для авторизации
void MainWindow::slotAuth(quint16 type, QString login, QString password)
{
    ui->textBrowser->append(QString::number(type) + " " + login + " " + password);
    msgType = type;
    Login = login;
    Password = password;
    socket->connectToHost("127.0.0.1",2323);
    SendToServer("");

   // socket->disconnectFromHost();
}


void MainWindow::on_SendButton_clicked()
{
    if(ui->SendToEdit->text() == "Общий" || ui->SendToEdit->text() == "")
    msgType = 2;
    else
    msgType = 3;
    SendToServer(ui->lineEdit->text());
    ui->SendToEdit->setText("Общий");
}


void MainWindow::on_lineEdit_returnPressed()
{
    if(ui->SendToEdit->text() == "Общий" || ui->SendToEdit->text() == "")
        msgType = 2;
    else
        msgType = 3;
    SendToServer(ui->lineEdit->text());
    ui->SendToEdit->setText("Общий");
}





