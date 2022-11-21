#include "widget.h"

#include <QApplication>
#include <QComboBox>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
                                        // ComboBox
//    QComboBox *comboBox = new QComboBox();
//    comboBox->addItem("KDE");
//    comboBox->addItem("Gnome");
//    comboBox->addItem("FVWM");
//    comboBox->addItem("CDE");
//    comboBox->setEditable(true);   // true : 새로운 옵션이 마지막에 추가 됨, false : 선택된 값만 이요
//    comboBox->show();
    Widget w;
    w.show();


    return a.exec();
}
