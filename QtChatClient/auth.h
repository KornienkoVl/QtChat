#ifndef AUTH_H
#define AUTH_H

#include <QWidget>
#include <QMessageBox>

namespace Ui {
class Auth;
}

class Auth : public QWidget
{
    Q_OBJECT

public:
    explicit Auth(QWidget *parent = nullptr);
    ~Auth();

private slots:
    void on_LoginButton_clicked();

private:
    Ui::Auth *ui;

signals:
    void signal(quint16  type, QString login, QString password);
};

#endif // AUTH_H
