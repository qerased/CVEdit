#include "filter_sort.h"

void filter_sort::counting_scatter (
    const uint8_t * keys, int bins, const cv::Mat & src, cv::Mat & dst) const
{
    std::vector<int> counts (bins, 0);
    const int pixels = src.rows * src.cols;

    for (int i = 0; i < pixels; i++)
        ++counts[keys[i]];

    std::vector<int> offsets (bins, 0);
    int acc = 0;
    for (int i = 0; i < bins; i++)
    {
        int c = counts[i];
        offsets[i] = acc;
        acc += c;
    }

    const uint8_t * sp = src.ptr<uint8_t> (0);
    uint8_t * dp = dst.ptr<uint8_t> (0);

    for (int i = 0; i < pixels; i++)
    {
        int dst_idx = offsets[keys[i]]++;

        const int s = i * 3;
        const int d = dst_idx * 3;
        dp [d + 0] = sp [s + 0];
        dp [d + 1] = sp [s + 1];
        dp [d + 2] = sp [s + 2];
    }
}

void filter_sort::sort_by_channel (cv::Mat & mat, int ch, int bins) const
{
    const int pixels = mat.rows * mat.cols;
    std::vector<uint8_t> keys (pixels);

    const uint8_t * p = mat.ptr<uint8_t> (0);
    for (int i = 0; i < pixels; i++)
        keys[i] = p[i * 3 + ch];

    cv::Mat out (mat.size (), mat.type ());
    counting_scatter (keys.data (), bins, mat, out);
    out.copyTo (mat);
}

void filter_sort::sort_by_luma (cv::Mat& mat) const
{
    cv::Mat gray;
    cv::cvtColor (mat, gray, cv::COLOR_BGR2GRAY);

    cv::Mat out (mat.size (), mat.type ());
    counting_scatter (gray.ptr<uint8_t> (0), 256, mat, out);
    out.copyTo (mat);
}

void filter_sort::sort_by_hue (cv::Mat& mat) const
{
    cv::Mat hsv;
    cv::cvtColor (mat, hsv, cv::COLOR_BGR2HSV);

    const int pixels = mat.rows * mat.cols;
    std::vector<uint8_t> keys (pixels);

    const uint8_t* hp = hsv.ptr<uint8_t> (0);
    for (int i = 0; i < pixels; i++)
        keys[i] = hp[i * 3 + 0];

    cv::Mat out (mat.size (), mat.type ());
    counting_scatter (keys.data (), 180, mat, out);
    out.copyTo (mat);
}

