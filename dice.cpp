#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>


int main(){
  cv::Mat image=cv::imread("dice6.png");
  cv::namedWindow("My Image");
  cv::imshow("My Image",image);
  cv::waitKey(0);
  return 1;
}
