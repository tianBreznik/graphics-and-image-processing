#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;


int slider = 5;
Mat image;
Mat image_blurred;
void on_trackbar(int, void *) {

slider = slider % 2 == 0 ? slider + 1 : slider;

GaussianBlur(image, image_blurred, Size(slider, slider),0);

imshow("Blurred image", image_blurred);

}
int main(int argc, char *argv[])
{
  //Print the OpenCV version
  printf("OpenCV version: %d.%d\n", CV_MAJOR_VERSION, CV_MINOR_VERSION);

  if(argc != 2)
  {
    printf(" Usage: display_image ImageToLoadAndDisplay\n");
    return -1;
  }

  //char* image_name =  (char*)malloc(100);
  //printf("Please give name of image to be displayed:\n");
  //scanf("%s", image_name);
  //getchar();

  //Mat image;
  //Mat image_blurred;
  image = imread(argv[1], CV_LOAD_IMAGE_COLOR);

  if(! image.data )                              // Check for invalid input
  {
    printf("Could not open or find the image\n");
    return -1;
  }

  //Mat grayscale;
  //cvtColor(image, grayscale, CV_BGR2GRAY);
  //imwrite("gray_image.jpg", grayscale);
  namedWindow("Original image",CV_WINDOW_AUTOSIZE/2);
  namedWindow("Blurred image", CV_WINDOW_AUTOSIZE/2);
  //namedWindow("Display window", CV_WINDOW_AUTOSIZE);// Create a window for display.
  //imshow("Display window", image);                   // Show our image inside it.
  imshow("Original image", image);
  GaussianBlur(image, image_blurred, Size(slider, slider), 0);
  imshow("Blurred image", image_blurred);
  createTrackbar("Kernel Size", "Blurred image", &slider, 41, on_trackbar);

  //imshow("Display window", grayscale);

  waitKey(0);
  return 0;
}

Mat drawHist(Mat hist, int scale)
{
  double mx = 0;
  minMaxLoc(hist, 0, &mx, 0, 0);

  Mat result - Mat::zeros(256*scale, 256*scale, CV_8UC1);
  for(int i = 0; i<255; i++)
  {
    float histValue = hist.at<float>(i, 0);
    float nextValue = hist.at<float>(i+1, 0);

    Point p1 = Point(i*scale, 256*scale);
    Point p2 = Point(i*scale + scale, 256*scale);
    Point p3 = Point(i*scale + scale, (256-nextValue*256/mx)*scale);
    Point p4 = Point(i*scale, (256-nextValue*256/mx)*scale);

    int numPoints = 5;
    Point points[] = {p1,p2,p3,p4,p1};
    fillConvexPoly(result, points, numPoints, Scalar::all(256), 0, 0);
  }

  return result;
}
