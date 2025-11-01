#ifndef CVEDIT_FILTER_SORT_H
#define CVEDIT_FILTER_SORT_H
#include "filter.h"

#include <QMetaType>

enum class sort_mode { Luminosity, Red, Green, Blue, Hue};
Q_DECLARE_METATYPE (sort_mode);

class filter_sort : public filter
{
public:

    const char * name () const { return "Sort"; }

    void apply (cv::Mat &mat) override
    {
        std::vector<cv::Vec3b> pixels;
        pixels.reserve (mat.cols * mat.rows);

        for (int row = 0; row < mat.rows; row++)
            for (int col = 0; col < mat.cols; col++)
                pixels.push_back (mat.at<cv::Vec3b> (row, col));

        std::ranges::sort (pixels, [this] (const cv::Vec3b &a, const cv::Vec3b &b)
        {
            switch (mode_)
            {
                case sort_mode::Red:
                    return a[2] < b[2];
                case sort_mode::Green:
                    return a[1] < b[1];
                case sort_mode::Blue:
                    return a[0] < b[0];
                case sort_mode::Hue:
                {
                    cv::Mat a_mat(1, 1, CV_8UC3, cv::Scalar(a[0], a[1], a[2]));
                    cv::Mat b_mat(1, 1, CV_8UC3, cv::Scalar(b[0], b[1], b[2]));
                    cv::Mat a_hsv, b_hsv;
                    cv::cvtColor (a_mat, a_hsv, cv::COLOR_BGR2HSV);
                    cv::cvtColor (b_mat, b_hsv, cv::COLOR_BGR2HSV);
                    return a_hsv.at<cv::Vec3b>(0, 0)[0] < b_hsv.at<cv::Vec3b>(0, 0)[0];
                }
                case sort_mode::Luminosity:
                default:
                    auto lum = [] (const cv::Vec3b & p)
                    {
                        return 0.299 * p[2] + 0.587 * p[1] + 0.114 * p[0];
                    };
                    return lum (a) < lum (b);
            }
        });

        int idx = 0;
        for (int row = 0; row < mat.rows; row++)
            for (int col = 0; col < mat.cols; col++)
                mat.at<cv::Vec3b> (row, col) = pixels[idx++];
    }

    void set_mode (const sort_mode mode)
    {
        mode_ = mode;
    }
private:
    sort_mode mode_ {sort_mode::Luminosity};
};

#endif //CVEDIT_FILTER_SORT_H