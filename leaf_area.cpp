#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

using namespace std;
using namespace cv;

Mat src, gray_img, edges, dst;
int thresh = 100;
RNG rng(12345);
const char *window_name = "Edges";
int lowThreshold = 0;
const int max_lowThreshold = 100;
const int _ratio = 3;
const int kernel_size = 3;

void show_wait_destroy(const char *winname, const Mat& img);
void canny_threshold(int, void *);
void thresh_cb(int, void *);

void help()
{
    cout << "Usage: ./leaf-area [image.[jpg|png]]" << endl;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        help();
        return -1;
    }

    src = imread(argv[1], IMREAD_COLOR);

    if (src.empty()) {
        cout << "The image" << argv[1] << " could not be loaded." << endl;
        return -1;
    }

    cvtColor(src, gray_img, COLOR_BGR2GRAY);
    namedWindow(window_name, WINDOW_AUTOSIZE);
    createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, canny_threshold);
    canny_threshold(0, nullptr);
    waitKey();

    return 0;
}

void canny_threshold(int, void *)
{
    /*
    * Blurring the image might reduce or eliminate noise, which can
    * be inconvenient if we're trying to measure leaf nerves.
    * The resulting blur level is proportional to the Size() of the kernel;
    * adjust if necessary.
    */
    blur(gray_img, edges, Size(1,1));
    Canny(edges, edges, lowThreshold, lowThreshold*_ratio, kernel_size);
    dst = Scalar::all(0);
    src.copyTo(dst, edges);
    imshow(window_name, dst);
}

void thresh_cb(int, void *)
{
    Mat canny_output;
    Canny(gray_img, canny_output, thresh, thresh * 2);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> hull(contours.size());
    for (size_t i = 0; i < contours.size(); i++)
        convexHull(contours[i], hull[i]);

    Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);

    for (size_t i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256));
        drawContours(drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0);
        drawContours(drawing, hull, (int)i, color);
    }

    imshow("Contours", drawing);
}

/**
 * Helper function that renders an image
 *
 * @param winname Name of the window
 * @param img Image to be rendered
 */
void show_wait_destroy(const char *winname, const Mat& img)
{
    imshow(winname, img);
    moveWindow(winname, 500, 0);
    waitKey();
    destroyWindow(winname);
}
