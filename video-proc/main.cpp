#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/stitching.hpp>

cv::Stitcher::Mode mode = cv::Stitcher::PANORAMA;

void usage(char *app_name)
{
    std::cout << "Usage: " << app_name << " video_capture_name" << std::endl;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    cv::VideoCapture input_video(argv[1]);
    if (!input_video.isOpened()) {
        std::cout << "Could not load the video" << std::endl;
        return 2;
    }

    std::vector<cv::Mat> frames;
    cv::Mat frame;
    int millisecond = 5000;
    cv::Mat pano;
    cv::Ptr<cv::Stitcher> stitcher = cv::Stitcher::create(mode);

    /*
     * Selects the next frame, currently using a time-based offset,
     * and push it onto a vector for later stitching
     */
    for (int i = 0; i < 10; i++) {
//    while (true) {
        input_video.set(cv::CAP_PROP_POS_MSEC, millisecond);
        millisecond += 400;
        input_video >> frame;

        if (frame.empty())
            break;

        cv::rotate(frame, frame, cv::ROTATE_180);
        frames.push_back(frame.clone());
    }

#ifdef SHOW_IMGS
    const char *winname = "Current Frame";
    cv::namedWindow(winname, cv::WINDOW_AUTOSIZE);

    for (const auto& f : frames) {
        cv::imshow(winname, f);
        cv::waitKey(-1);
    }
#endif

    cv::Stitcher::Status status = stitcher->stitch(frames, pano);

    if (status != cv::Stitcher::Status::OK) {
        std::cerr << "Error stitching images: " << (int)status;
        return -1;
    }

#ifdef SHOW_IMGS
    cv::imshow("result", pano);
    cv::waitKey(-1);
#endif
#ifdef SAVE_IMG
    cv::imwrite("result.jpg", pano);
#endif

    return 0;
}
