#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
    Mat image(200,300,CV_8U,Scalar(255));
    namedWindow("Keyboard Event",WINDOW_AUTOSIZE);
    imshow("Keyboard Event",image);

    while(1)
    {
        int key = waitKey(100);
        //printf("%x\n",key);
        if (key == 27) break;

        switch(key)
        {
            case 'a': cout << "pressed a" << endl; break;
            case 'b': cout << "pressed b" << endl; break;
            case 0x41: cout << "pressed A" << endl; break;
            case 0x42: cout << "pressed B" << endl; break;

            case 0x51: cout << "pressed left arrow" << endl; break;
            case 0x52: cout << "pressed up arrow" << endl; break;
            case 0x53: cout << "pressed right arrow" << endl; break;
            case 0x54: cout << "pressed down arrow" << endl; break;
        }
    }


    return 0;
}
