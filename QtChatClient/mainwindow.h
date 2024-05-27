#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTime>
#include <auth.h>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ConnectButton_clicked();

    void on_SendButton_clicked();

    void on_lineEdit_returnPressed();


private:
    Ui::MainWindow *ui;
    Auth *form;
    QTcpSocket *socket;
    QByteArray Data;
    void SendToServer(QString str);
    quint16 nextBlockSize;
    //данные для авторизации
    quint16 msgType;
    QString Login;
    QString Password;

public slots:
    void slotReadyRead();
    void slotAuth(quint16 type, QString login, QString password);

};
#endif // MAINWINDOW_H
