#ifndef CVEDIT_FILTER_SHAKE_H
#define CVEDIT_FILTER_SHAKE_H
#include "filter.h"

class filter_shake : public filter
{
public:
    const char *  name () const override { return "Glitch"; }

    void apply (cv::Mat & mat) override {};

    void apply (cv::Mat &mat, frame_info & fi) override
    {
        if (mat.empty()) return;

        cv::Mat mapx (mat.size(), CV_32FC1);
        cv::Mat mapy (mat.size(), CV_32FC1);

        for (int row = 0; row < mat.rows; row++)
        {
            for (int col = 0; col < mat.cols; col++)
            {
                mapx.at<float> (row, col) = col + 10 * std::sin (row / 20 + (fi.tick_num_ % 3));
                mapy.at<float> (row, col) = row;
            }
        }

        cv::remap (mat, mat, mapx, mapy, cv::INTER_LINEAR, cv::BORDER_WRAP);
    }

};

#endif //CVEDIT_FILTER_SHAKE_H