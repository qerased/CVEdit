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

    if (params_.mode == sort_mode::Hue)
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

    const unsigned int chunk = params_.chunk > 0 ? params_.chunk : w;
    const unsigned int stride = params_.stride > 0 && params_.chunk > 0 ? params_.stride : w;

    std::vector<uint8_t> keys;
    keys.reserve (chunk);
    std::vector<cv::Vec3b> buf;
    buf.reserve (chunk);

    const int dx = params_.axis == sort_axis::Vertical ? stride : 1;
    const int dy = params_.axis == sort_axis::Vertical ? 1 : stride;

    std::uniform_int_distribution<int> chunk_jitter (std::max (8u, params_.chunk / 2), std::max (params_.chunk * 3/2, 10u));
    const unsigned int xlen = params_.axis == sort_axis::Vertical ? (params_.use_rand_chunk ? chunk_jitter(rng_) : chunk) : w;

    const unsigned int yloop = params_.axis == sort_axis::Vertical ? 1 : chunk;

    const bool use_mask = params_.use_random_mask && (params_.stride > 0);
    const double auto_p = (params_.stride > 0) ? (1.0 / static_cast<double> (params_.stride)) : 1.0;
    const double pick_p = (params_.mask_prob > 0.0 && params_.mask_prob <= 1.0) ? params_.mask_prob : auto_p;
    std::bernoulli_distribution pick_dist (pick_p);

    auto should_sort_key = [&] (unsigned int k)
    {
        if (!params_.thresh_enabled) return true;
        return (k < params_.thr_lo) || (k > params_.thr_hi);
    };

    for (int y = 0; y < h; y += dy)
    {
        if (use_mask && !pick_dist (rng_))
            continue;

        for (int y0 = y; y0 < std::min (y + yloop, h); y0++)
        {
            auto * row = mat.ptr<cv::Vec3b> (y0);
            for (int x = 0; x < w; x += dx)
            {
                const int len = std::min (xlen, w - x);
                keys.resize (len);
                buf.resize (len);

                if (params_.mode == sort_mode::Hue)
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

                if (!params_.thresh_enabled)
                {
                    std::vector<cv::Vec3b> out (len);
                    counting_scatter_segment (buf.data (), keys.data (), len,
                                              bins, counts, offsets, out.data ());

                    for (int i = 0; i < len; i++)
                        row[x + i] = out[i];
                }
                else
                {
                    int i = 0;
                    while (i < len)
                    {
                        if (!should_sort_key(keys[i]))
                        {
                            i++;
                            continue;
                        }

                        int j = i + 1;
                        while (j < len && should_sort_key(keys[j])) j++;

                        const int segN = j - i;
                        std::vector<cv::Vec3b> segOut (segN);
                        counting_scatter_segment (&buf[i], &keys[i], segN,
                                                  bins, counts, offsets, segOut.data ());
                        for (int t = 0; t < segN; ++t)
                            row[x + i + t] = segOut[t];
                        i = j;
                    }
                }

                if ((params_.axis == sort_axis::Vertical && chunk == 0) ||
                     params_.axis == sort_axis::Horizontal)
                    break;
            }
        }
    }
}

void filter_sort::sort_cols (cv::Mat & mat)
{
    cv::Mat rot;
    cv::rotate (mat, rot, cv::ROTATE_90_CLOCKWISE);
    auto save = params_.axis;
    params_.axis = params_.axis == sort_axis::Vertical ? sort_axis::Horizontal : sort_axis::Vertical;
    sort_rows (rot);
    params_.axis = save;
    cv::rotate (rot, mat, cv::ROTATE_90_COUNTERCLOCKWISE);
}