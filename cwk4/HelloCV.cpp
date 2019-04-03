#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

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

  Mat image;
  image = imread(argv[1], CV_LOAD_IMAGE_COLOR);

  if(! image.data )                              // Check for invalid input
  {
    printf("Could not open or find the image\n");
    return -1;
  }

  namedWindow("Display window", CV_WINDOW_AUTOSIZE);// Create a window for display.
  imshow("Display window", image);                   // Show our image inside it.

  waitKey(0);
  return 0;
}
