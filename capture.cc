// example of using libpixyusb to grab a 1280x40 block (maximum camera
// resolution)

#include <fcntl.h>
#include <iostream>
#include <mach/clock.h>
#include <mach/mach.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "opencv2/core/mat.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "pixy.h"

void current_utc_time(struct timespec *ts) {
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  ts->tv_sec = mts.tv_sec;
  ts->tv_nsec = mts.tv_nsec;
}

int main(int argc, char *argv[]) {
  int pixy_init_status;

  // Connect to Pixy //
  pixy_init_status = pixy_init();
  printf("initialized Pixy - %d\n", pixy_init_status);
  if (pixy_init_status != 0) {
    pixy_error(pixy_init_status);
    return pixy_init_status;
  }

  unsigned char current_frame[72000];  // ~largest possible
  unsigned char *pixels;     // returned pointer to video frame buffer
  int32_t response, fourcc;
  int8_t renderflags;
  int return_value, res;
  uint16_t width, height;
  uint32_t numPixels;

  // stop blob processing
  return_value = pixy_command("stop", END_OUT_ARGS, &response, END_IN_ARGS);
  printf("STOP returned %d response %d\n", return_value, response);

  struct timespec start;
  current_utc_time(&start);

  response = 0;
  return_value =
      pixy_command("cam_getFrame",  // String id for remote procedure
                   0x01, 0x21,      //
                   0x02, 0,         // xoffset
                   0x02, 0,         // yoffset
                   0x02, 320,       // width
                   0x02, 200,       // height
                   0,               // separator
                   &response,       // pointer to mem address for return value
                   &fourcc,  // contrary to docs, the next 5 args are needed
                   &renderflags, &width, &height, &numPixels,
                   &pixels,  // pointer to mem address for returned frame
                   0);

  struct timespec end;
  current_utc_time(&end);

  printf("getFrame returned %d response %d\n", return_value, response);
  printf("returned w %d h %d npix %d\n", width, height, numPixels);
  printf("took %f ms\n",
         static_cast<double>(end.tv_nsec - start.tv_nsec) / 1000000);

  // quit now if not successful:
  if (return_value != 0) {
    fprintf(stderr, "pixy_command returned %d; exiting\n", return_value);
    exit(1);
  }

  cv::Mat bayer_image = cv::Mat(200, 320, CV_8UC1, pixels).clone();
  cv::Mat rgb_image;

  cv::cvtColor(bayer_image, rgb_image, CV_BayerBG2RGB);

  cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
  cv::imshow("Display window", rgb_image);

  printf("waiting...\n");

  cv::waitKey(0);
}
