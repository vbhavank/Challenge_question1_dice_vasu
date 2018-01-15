#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "opencv2/imgproc/imgproc.hpp"

std::ostringstream ttext;
cv::Mat src,dst,roi,roi_g;
int src_gray=0;
using namespace cv;
using namespace std;

int countpips(cv::Mat roi)
{
//resizing images of individual dices
  cv::resize(roi, roi, cv::Size(150, 150));
  cvtColor(roi, roi_g, CV_RGB2GRAY);
  cv::threshold(roi_g, roi_g, 200, 255, cv::THRESH_BINARY | CV_THRESH_OTSU );
//Using flooding of corners for better isolation of the pits 
  cv::floodFill(roi_g, cv::Point(0,0), cv::Scalar(255));
  cv::floodFill(roi_g, cv::Point(0,149), cv::Scalar(255));
  cv::floodFill(roi_g, cv::Point(149,0), cv::Scalar(255));
  cv::floodFill(roi_g, cv::Point(149,149), cv::Scalar(255));

  vector<Vec3f> cir;
//Detecting all pits in a single dice
  HoughCircles( roi_g, cir, CV_HOUGH_GRADIENT,1 , roi_g.rows/6,7, 10,9 , 30 );
//Iterating through all the pits in a dice 
  for( size_t i = 0; i < cir.size(); i++ )
  {
      Point center(cvRound(cir[i][0]), cvRound(cir[i][1]));
      int radius = cvRound(cir[i][2]);
      //Drawing a green circle around each pit in a dice
      circle( roi, center, radius, Scalar(0,255,0), 3, 8, 0 );
   }

  //returns the number of pits in a dice
  return cir.size();
}

int main()
{
 std::vector<std::vector<cv::Point> > conto;
 std::vector<cv::Vec4i> hirer;
 cv::Mat ds,Th,ca,droi;

//Reading the image dice1.png(modify according to requirement)
 cv::Mat image=cv::imread("/home/bxv7657/NASCENT/diced/im/dice6.png");
//Detecting the canny edges for isolating edges: Helps removing the wood baground
 cv::Canny( image, ca, 200, 200*5, 3 );

 int erosion_size = 35;  
 Mat element = getStructuringElement(cv::MORPH_RECT,cv::Size(2 * erosion_size + 1, 2 *erosion_size + 1),cv::Point(erosion_size, erosion_size));
 
// Perform erosion or dilation to improve segmentation :This was crucial
dilate(ca,dst,element);        
erode(dst,ca,element); 
int tot=0;
// Finding Contours helps us obtain co-ordinates of individual dice+pips
cv::findContours( ca,conto, hirer,CV_RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
//Iterate through all contours(including dice and pips)
for(int i=0; i<conto.size(); i++)
{
// Calculate contour area to check if the detected contour is a pip or dice
  double contArea = cv::contourArea(conto[i]);

// filter contours based on our area
  if (contArea > 320){

//Get bounding rectangle co-ordinates
    cv::Rect Bounco = cv::boundingRect(cv::Mat(conto[i]));

// Crop individual dices
     droi = image(Bounco);

// count number of pips in one dice
     src_gray=countpips(droi);
// Total pips in the image
     tot+=src_gray;
    
if(src_gray > 0)
{
// Individual dice value
          std::ostringstream diceText;
          diceText << "" << src_gray;

// Draw the value of each dice on image
        cv::putText(image, diceText.str(),
        cv::Point(Bounco.x,Bounco.y+Bounco.height+20),
        cv::FONT_HERSHEY_SIMPLEX,0.8,cv::Scalar::all(255),1,8);
        rectangle(image,Bounco.tl(), Bounco.br(),Scalar(0,255,0),2,8,0);
        }

      }

    }
// Total dice value in the image
   ttext << "Sum:" << tot;
// Draw the total value on the image
   cv::putText(image,ttext.str(),cv::Point(100,100),cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar::all(255), 1, 8);
// Displaying final image
  cv::namedWindow("DiceDot", CV_WINDOW_AUTOSIZE );
    cv::imshow("DiceDot", image);
    cv::waitKey(0);
//std::cout << "Number of pips :" << tot<< std::endl;
// Writing the final image 'Result_dice1.png'
   cv::imwrite( "/home/bxv7657/NASCENT/diced/im/Result_dice1.png", image);
  
}
  

