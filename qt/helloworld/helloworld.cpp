#include <QApplication>
#include <QLabel>
#include <QPushButton>

int main(int argc, char **argv)
{
        QApplication app(argc,argv);

//        QLabel *hello = new QLabel("<font color=blue>Hello <i>World!</i></font>",0);
//        hello->show();
        QPushButton *button = new QPushButton("Quit",0);
        button->resize(120,35);
        button->move(300, 300);
        button->show();

        return app.exec();
}
