/********************************************************************************
** Form generated from reading UI file 'auth.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTH_H
#define UI_AUTH_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Auth
{
public:
    QPushButton *LoginButton;
    QCheckBox *checkBox;
    QLineEdit *LoginEdit;
    QLineEdit *PasswordEdit;
    QLabel *labelLogin;
    QLabel *labelPassword;

    void setupUi(QWidget *Auth)
    {
        if (Auth->objectName().isEmpty())
            Auth->setObjectName("Auth");
        Auth->resize(271, 294);
        LoginButton = new QPushButton(Auth);
        LoginButton->setObjectName("LoginButton");
        LoginButton->setGeometry(QRect(30, 200, 211, 51));
        checkBox = new QCheckBox(Auth);
        checkBox->setObjectName("checkBox");
        checkBox->setGeometry(QRect(30, 160, 101, 20));
        LoginEdit = new QLineEdit(Auth);
        LoginEdit->setObjectName("LoginEdit");
        LoginEdit->setGeometry(QRect(30, 60, 211, 22));
        PasswordEdit = new QLineEdit(Auth);
        PasswordEdit->setObjectName("PasswordEdit");
        PasswordEdit->setGeometry(QRect(30, 130, 211, 22));
        labelLogin = new QLabel(Auth);
        labelLogin->setObjectName("labelLogin");
        labelLogin->setGeometry(QRect(30, 20, 141, 31));
        QFont font;
        font.setPointSize(18);
        font.setBold(true);
        labelLogin->setFont(font);
        labelLogin->setTextFormat(Qt::AutoText);
        labelLogin->setScaledContents(false);
        labelPassword = new QLabel(Auth);
        labelPassword->setObjectName("labelPassword");
        labelPassword->setGeometry(QRect(30, 90, 141, 31));
        labelPassword->setFont(font);
        labelPassword->setTextFormat(Qt::AutoText);
        labelPassword->setScaledContents(false);

        retranslateUi(Auth);

        QMetaObject::connectSlotsByName(Auth);
    } // setupUi

    void retranslateUi(QWidget *Auth)
    {
        Auth->setWindowTitle(QCoreApplication::translate("Auth", "\320\220\320\262\321\202\320\276\321\200\320\270\320\267\320\260\321\206\320\270\321\217", nullptr));
        LoginButton->setText(QCoreApplication::translate("Auth", "\320\237\321\200\320\276\320\264\320\276\320\273\320\266\320\270\321\202\321\214", nullptr));
        checkBox->setText(QCoreApplication::translate("Auth", "\320\240\320\265\320\263\320\270\321\201\321\202\321\200\320\260\321\206\320\270\321\217", nullptr));
        labelLogin->setText(QCoreApplication::translate("Auth", "\320\233\320\276\320\263\320\270\320\275", nullptr));
        labelPassword->setText(QCoreApplication::translate("Auth", "\320\237\320\260\321\200\320\276\320\273\321\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Auth: public Ui_Auth {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTH_H
