#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void scaling(Mat img, Mat& dst, Size size)
{
        dst = Mat(size, img.type(), Scalar(0));
        double ratioY = (double)size.height / img.rows;
        double ratioX = (double)size.width / img.cols;

        for (int i=0;i<img.rows;i++)
        {
                for (int j=0; j<img.cols;j++)
                {
                        int x = (int)(j*ratioX);
                        int y = (int)(i*ratioY);
                        dst.at<uchar>(y,x) = img<uchar>(i,j);
                }
        }
}


int main()
{
        Mat image = imread("./lena.bmp",0);
        CV_Assert(image.data);

        Mat dst1, dst2;
        scaling(image, dts1, size(150,200));
        scaling(image, dts2, size(300,400));

        imshow("image",image);
        inshow("dst1 --", dst1);
        inshow("dst2 --", dst2);
        resizeWindow("dst-expension",dst2)
        waitKey();
        return 0;
}
