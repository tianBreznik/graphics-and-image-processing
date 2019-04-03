#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdbool.h>
#include <math.h>


using namespace std;
using namespace cv;


int slider = 9;
Mat image;
Mat image_blurred;

Mat drawHist(Mat hist, int scale)
{
  double mx = 0;
  minMaxLoc(hist, 0, &mx, 0, 0);

  Mat result = Mat::zeros(256*scale, 256*scale, CV_8UC1);
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

void on_trackbar(int, void *) {

  slider = slider % 2 == 0 ? slider + 1 : slider;
  GaussianBlur(image, image_blurred, Size(slider, slider),0);

  imshow("Blurred image", image_blurred);

  Mat temp_diff = image - image_blurred + 128;
  int histSize = 256;
  float range[] = { 0, 256 };
  const float* histRange = { range };
  Mat newHist;
  bool uniform = true, accumulate = false;
  int channels = 1;
  const int* nr_channel = &channels;
  calcHist(&temp_diff, 1, nr_channel, cv::Mat(), newHist, 1, &histSize, &histRange, uniform, accumulate);
  Mat hist = drawHist(newHist, 25);
  Scalar mean, stddev;
  meanStdDev(newHist, mean, stddev);
  double deviation = sqrt(stddev[0]/255.0);
  char display_text[50];
  snprintf(display_text, 15, "%f", deviation);
  printf("%s\n", display_text);
  imwrite( "hist.jpg", hist);
  imwrite( "hist.jpg", hist);
  imshow("Histogram", hist);

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

  image = imread(argv[1], CV_LOAD_IMAGE_COLOR);

  if(! image.data )                             
  {
    printf("Could not open or find the image\n");
    return -1;
  }

  Mat grayscale;
  cvtColor(image, grayscale, CV_BGR2GRAY);
  imwrite("gray_image.jpg", grayscale);
  namedWindow("Original image",CV_WINDOW_AUTOSIZE/2);
  namedWindow("Blurred image", CV_WINDOW_AUTOSIZE/2);
  imshow("Original image", image);
  createTrackbar("Kernel Size", "Blurred image", &slider, 41, on_trackbar);
  namedWindow("Histogram", (CV_WINDOW_AUTOSIZE*3)/4);

  on_trackbar( slider, 0 );

  waitKey(0);
  return 0;
}
