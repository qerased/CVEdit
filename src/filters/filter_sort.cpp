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

void filter_sort::sort_rows (cv::Mat &mat)
{
    const int bins = key_bins ();
    std::vector<int> counts  (bins);
    std::vector<int> offsets (bins);

    const unsigned int w = mat.cols;
    const unsigned int h = mat.rows;

    const unsigned int chunk = chunk_ > 0 ? chunk_ : w;
    const unsigned int stride = stride_ > 0 ? stride_ : w;

    std::vector<uint8_t> keys;
    keys.reserve (chunk);
    std::vector<cv::Vec3b> buf;
    buf.reserve (chunk);

    for (int y = 0; y < h; y++)
    {
        auto * row = mat.ptr<cv::Vec3b> (y);
        for (int x = 0; x < w; x += stride)
        {
            const int len = std::min (chunk, w - x);
            keys.resize (len);
            buf.resize (len);

            if (mode_ == sort_mode::Hue)
            {
                cv::Mat row_mat (1, w, CV_8UC3, row);
                cv::Mat hsv_row;
                cv::cvtColor (row_mat, hsv_row, cv::COLOR_BGR2HSV);
                auto * hsvp = hsv_row.ptr<cv::Vec3b> (0) + x;
                for (int i = 0; i < len; i++)
                {
                    buf[i] = row[x + i];
                    keys[i] = hsvp[i][0];
                }
            }
            else
            {
                for (int i = 0; i < len; i++)
                {
                    const auto p = row[x + i];
                    buf[i] = p;
                    keys[i] = pixel_key_bgr (p);
                }
            }

            std::vector<cv::Vec3b> out (len);
            counting_scatter_segment (buf.data (), keys.data (), len,
                                      bins, counts, offsets, out.data ());

            for (int i = 0; i < len; i++)
                row[x + i] = out[i];

            if (stride == 0) break;
        }
    }
}

/// temp
void filter_sort::sort_cols (cv::Mat &mat)
{
    sort_global (mat);
}