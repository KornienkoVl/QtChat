#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QFile>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    QTcpSocket *socket;

private:
    QVector <QTcpSocket*> Sockets; //Сокеты для каждого подключения
    QByteArray Data; //Данные между сервером и клиентами
    quint16 nextBlockSize; //размер блока данных
    QSqlDatabase db; //база данных
    QSqlQuery *query; //запрос к бд
    QSqlTableModel *model; //модель бд для ввода/вывода данных
    QMap<QTcpSocket*, QString> mapUser; //логин пользователя привязывается к сокету

    //сокет до авторизации
    QTcpSocket *Usocket;
    //источник и цель сообщения
    QString UserLogin;
    QString UserSendTo;
    //тип сообщения для клиента
    quint16 UsermsgType;

    void SendToClient(QString str);
    bool Registration(QString Login, QString Password);
    bool LogIn(QString Login, QString Password);
    void WriteLog(QString str);

public slots:
    void incomingConnection(qintptr socketDescriptor); //обработка новых подключений
    void slotReadyRead(); //обработчик полученных от клиента сообщений
    void LogOut(); //отключение сокета
};

#endif // SERVER_H
