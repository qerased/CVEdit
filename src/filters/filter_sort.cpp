#include "filter_sort.h"

static void build_offsets (const int bins,
                           const std::vector<int> & counts,
                           std::vector<int> & offsets)
{
    int acc = 0;
    for (int i = 0; i < bins; i++)
    {
        offsets[i] = acc;
        acc += counts[i];
    }
}

static void counting_scatter_segment (
    const cv::Vec3b * src, const uint8_t * keys,
    int N, int bins,
    std::vector<int> & counts, std::vector<int> & offsets,
    cv::Vec3b * dst)
{
    std::fill (counts.begin (), counts.end (), 0);
    for (int i = 0; i < N; i++)
        ++counts[keys[i]];

    build_offsets (bins, counts, offsets);

    for (int i = 0; i < N; i++)
    {
        int pos = offsets[keys[i]]++;
        dst[pos] = src[i];
    }
}

void filter_sort::sort_global (cv::Mat & mat)
{
    const int pixels = mat.rows * mat.cols;
    std::vector<uint8_t> keys (pixels);

    if (mode_ == sort_mode::Hue)
    {
        cv::Mat hsv;
        cv::cvtColor (mat, hsv, cv::COLOR_BGR2HSV);
        const auto * hp = hsv.ptr<cv::Vec3b> (0);
        for (int i = 0; i < pixels; i++)
            keys[i] = hp[i][0];
    }
    else
    {
        const auto * p = mat.ptr<cv::Vec3b> (0);
        for (int i = 0; i < pixels; i++)
            keys[i] = pixel_key_bgr (p[i]);
    }

    cv::Mat out (mat.size (), mat.type ());
    std::vector<int> counts (key_bins ());
    std::vector<int> offsets ( key_bins ());

    counting_scatter_segment (
        mat.ptr<cv::Vec3b> (0), keys.data (), pixels,
        key_bins (), counts, offsets, out.ptr<cv::Vec3b> (0)
    );

    out.copyTo (mat);
}

/// temp
void filter_sort::sort_rows (cv::Mat &mat)
{
    sort_global (mat);
}

void filter_sort::sort_cols (cv::Mat &mat)
{
    sort_global (mat);
}