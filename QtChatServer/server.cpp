#include "server.h"

Server::Server()
{
    if(this->listen(QHostAddress::Any, 2323))
    {
        qDebug() << "start";
        WriteLog("Server started");
    }
    else
    {
        qDebug() << "error";
        WriteLog("Server didn't start");
    }
    nextBlockSize = 0;

    //подключаем бд
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./chatDB.db");
    if(db.open())
    {
        qDebug("db open");
        WriteLog("Database connected");
        query = new QSqlQuery(db);
        query->exec("CREATE TABLE users(Login TEXT, Password TEXT);");

        model = new QSqlTableModel(this,db);
        model->setTable("users");
    }
    else
    {
        qDebug("db no open");
        WriteLog("Database connection error");
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    //при отключении клиента удаляем сокет    
    connect(socket, &QTcpSocket::disconnected, this, &Server::LogOut);
    Sockets.push_back(socket);
    qDebug() << "client connected" << socketDescriptor;
    WriteLog("Client connected "+QString::number(socketDescriptor));
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_7);
    if(in.status() == QDataStream::Ok)
    {
        qDebug() << "reading...";
        for(;;)
        {
            //размер блока неизвестен
            if(nextBlockSize == 0)
            {
                qDebug() << "nextBlockSize = 0";
                //для чтения должны быть доступны как минимум 2 байта
                if(socket->bytesAvailable() < 2)
                {
                    qDebug() << "Data < 2, break";
                    break;
                }
                in >> nextBlockSize;
                qDebug() << "nextBlockSize = " << nextBlockSize;             
            }
            //проверка получения всего сообщения
            if(socket->bytesAvailable() < nextBlockSize)
            {
                qDebug() << "Data not full, break";
                break;
            }
            //получили полное сообщение
            //получили тип сообщения
            quint16 msgType;
            in >> msgType;
            qDebug() << "Msg type = " << msgType;

            //от типа сообщения зависит, личное оно или общее + авторизация
            QString Password;
            QString str;
            QTime time;

            //типы сообщений 0-регистрация, 1-вход, 2-общее сообщение, 3-личное сообщение
            switch (msgType)
            {
            case 0:
                in >> UserLogin >> Password;
                qDebug() << "Регистрация " << UserLogin << " " << Password;
                WriteLog("Register "+UserLogin+" "+Password);
                if(Registration(UserLogin,Password))
                {
                    //отправляем сообщение об успешной регистрации
                    //приписываем сокету логин пользователя
                    qDebug() << "Успех регистрации";
                    WriteLog("Register OK");
                    mapUser.insert(socket,UserLogin);
                    UsermsgType = 0;//успешная регистрация
                    Usocket = socket;
                    SendToClient("");
                }
                else
                {
                    qDebug() << "не вышло зарегистрироваться";
                    WriteLog("Register Error");
                    //отправляем сообщение о провале регистрации
                    UsermsgType = 1;//ошибка регистрации
                    Usocket = socket;
                    SendToClient("");
                }

                break;

            case 1:
                in >> UserLogin >> Password;
                qDebug() << "Вход " << UserLogin << " " << Password;
                WriteLog("Login "+UserLogin+" "+Password);
                if(LogIn(UserLogin,Password))
                {
                    //отправляем сообщение об успешном входе
                    //приписываем сокету логин пользователя
                    qDebug() << "Успех входа";
                    WriteLog("Login OK");
                    mapUser.insert(socket,UserLogin);
                    UsermsgType = 2;//успешный вход
                    Usocket = socket;
                    SendToClient("");
                }
                else
                {
                    qDebug() << "не вышло войти";
                    WriteLog("Login error");
                    //отправляем сообщение о провале входа
                    UsermsgType = 3;//провал входа
                    Usocket = socket;
                    SendToClient("");
                }
                break;

            case 2:
                in >> time >> UserLogin >> str;
                qDebug() << "Общее сообщение " << time << " " << UserLogin << ": " << str;
                WriteLog("Shared message "+UserLogin+": "+str);
                UsermsgType = 4;//Общее сообщение
                SendToClient(str);
                break;

            case 3:
                bool online = 0;
                in >> time >> UserLogin >> UserSendTo >> str;
                qDebug() << "Личное сообщение " << time << " " << UserLogin << " to "<< UserSendTo << ": " << str;                
                //если адресат в сети
                for (int i = 0; i < Sockets.size(); i++)
                {
                    if(mapUser.contains(Sockets[i]))
                    {
                        if(mapUser[Sockets[i]] == UserSendTo)
                        {
                            online = 1;
                            WriteLog("Private message from"+UserLogin+" to "+UserSendTo+": "+str);
                            UsermsgType = 5;//Личное сообщение
                            SendToClient(str);
                            break;
                        }
                    }
                }
                if(online==0)
                {
                WriteLog("Private message from"+UserLogin+" to "+UserSendTo+" failed");
                UsermsgType = 6;//Пользователь не в сети
                Usocket = socket;
                SendToClient("");
                break;
                }
            }
            nextBlockSize = 0;         
            break;
        }
    }
    else
    {
        qDebug() << "DataStream error";
    }
}

void Server::SendToClient(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_7);    
    //выделяем 2 байта для размера блока сообщения
    out << quint16(0);

    //типы сообщений 0-успешная регистрация, 1-ошибка регистрации, 2-успешный вход, 3-ошибка входа, 4-общее сообщение, 5-личное сообщение, 6-адресат не в сети
    switch (UsermsgType)
    {
    case 0:
        out << quint16(0);
        out.device()->seek(0);
        out << quint16(Data.size() - sizeof(quint16));
        Usocket->write(Data);
        break;

    case 1:
        out << quint16(1);
        out.device()->seek(0);
        out << quint16(Data.size() - sizeof(quint16));
        Usocket->write(Data);
        break;

    case 2:
        out << quint16(2);
        out.device()->seek(0);
        out << quint16(Data.size() - sizeof(quint16));
        Usocket->write(Data);
        break;

    case 3:
        out << quint16(3);
        out.device()->seek(0);
        out << quint16(Data.size() - sizeof(quint16));
        Usocket->write(Data);
        break;

    case 4:
        str = " "+ UserLogin + ": " + str;
        out << quint16(4) << QTime::currentTime() << str;
        out.device()->seek(0);
        out << quint16(Data.size() - sizeof(quint16));
        //высылаем сообщение каждому авторизированному клиенту
        for (int i = 0; i < Sockets.size(); i++)
        {
            if(mapUser.contains(Sockets[i]))
            {
                 Sockets[i]->write(Data);
            }
        }
        break;

    case 5:
        str = " [Личное] " + UserLogin + ": " + str;
        out << quint16(5) << QTime::currentTime() << str;
        out.device()->seek(0);
        out << quint16(Data.size() - sizeof(quint16));
        //высылаем сообщение адресату и отправителю
        for (int i = 0; i < Sockets.size(); i++)
        {
            if(mapUser.contains(Sockets[i]))
            {
                if(mapUser[Sockets[i]] == UserLogin || mapUser[Sockets[i]] == UserSendTo)
                {
                Sockets[i]->write(Data);
                }
            }
        }
        break;
    case 6:
        out << quint16(6);
        out.device()->seek(0);
        out << quint16(Data.size() - sizeof(quint16));
        Usocket->write(Data);
        break;
    }
}

bool Server::Registration(QString Login, QString Password)
{
    QString request = QString("SELECT * from users WHERE Login = '"+ Login +"';");
    QSqlQuery q(request);
    //если логин уже существует

    if(q.first())
    {
        return 0;
    }
    else
    {
        //внесение данных в бд
         QSqlRecord rcd = model->record();
         rcd.setValue("Login",  Login);
         rcd.setValue("Password", Password);
         model->insertRecord(-1,rcd);
         return 1;
    }
}

bool Server::LogIn(QString Login, QString Password)
{
    //Пользователь с данным логином уже в сети
    for (int i = 0; i < Sockets.size(); i++)
    {
        if(mapUser.contains(Sockets[i]))
        {
            if(mapUser[Sockets[i]] == UserLogin )
            {
                return 0;
            }
        }
    }

    QString request = QString("SELECT * from users WHERE Login = '"+ Login +"';");
    QSqlQuery q(request);
    //находим логин
    if(q.first())
    {
        if(q.value(1).toString() == Password)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }

}
//добавляем сообщение в конец файла логов
void Server::WriteLog(QString str)
{
    QFile logFile;
    logFile.setFileName("ServerLogs.txt");
    if(logFile.open(QIODevice::Append|QIODevice::Text))
    {
        QTextStream writeStream(&logFile);
        writeStream << str<<"\n";
        logFile.close();
    }
}

void Server::LogOut()
{
    socket = (QTcpSocket*)sender();
    if(mapUser.contains(socket))
    {
        WriteLog("Client log out "+ mapUser[socket] + " " +QString::number(socket->socketDescriptor()));
    }
    else
    {
    WriteLog("Client log out "+ QString::number(socket->socketDescriptor()));
    }
    Sockets.erase(std::remove(Sockets.begin(),Sockets.end(), socket),Sockets.end());
    socket->deleteLater();
}







