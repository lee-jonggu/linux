#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

int main(int argc, char **argv)
{
//        QApplication app(argc,argv);

//        QLabel *hello = new QLabel("<font color=blue>Hello <i>World!</i></font>",0);
//        hello->show();
//        QPushButton *button = new QPushButton("Quit",0);
//        button->resize(120,35);
//        button->move(300, 300);
//        button->show();
        Mat image(300,400,CV_8UC1, Scalar(255));
        imshow("Hello World",image);
        waitkey(0);
        return 0;

        //return app.exec();
}
