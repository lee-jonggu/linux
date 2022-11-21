#include "widget.h"
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QValidator>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    //doubleValidator->setRange(10.0,100.0,3);
    doubleValidator->setBottom(10.0);
    doubleValidator->setTop(100.0);
    doubleValidator->setDecimals(3);

    QIntValidator *intValidator = new QIntValidator(this);
//    intValidator->setRange(13,19);
    intValidator->setBottom(13);
    intValidator->setTop(19);

    QRegularExpressionValidator *regExpValidator = new QRegularExpressionValidator(this);
    //regExpValidator->setRegularExpression(QRegularExpression("^\\d{1,3}.\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$"));
    regExpValidator->setRegularExpression(QRegularExpression("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
            "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
            "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
            "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"));

    QLineEdit *lineEditDouble = new QLineEdit(this);
    lineEditDouble->setValidator(doubleValidator);

    QLineEdit *lineEditInt = new QLineEdit(this);
    lineEditInt->setValidator(intValidator);

    QLineEdit *lineEditRegExp = new QLineEdit(this);
    lineEditRegExp->setValidator(regExpValidator);

    QFormLayout *formLayout = new QFormLayout(this);
    formLayout->addRow("&Double", lineEditDouble);
    formLayout->addRow("&int", lineEditInt);
    formLayout->addRow("&Regular Expression", lineEditRegExp);

    setWindowTitle("Validator");
}

Widget::~Widget()
{
}

