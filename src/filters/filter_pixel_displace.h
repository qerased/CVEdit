#ifndef CVEDIT_FILTER_PIXEL_DISPLACE_H
#define CVEDIT_FILTER_PIXEL_DISPLACE_H
#include "filter.h"

class filter_pixel_displace : public filter
{
public:

    const char * name () const { return "pixel displace"; }

    void apply (cv::Mat & mat) override
    {
        frame_info fake{0};
        apply (mat, fake);
    }

    void apply (cv::Mat &mat, frame_info &fi) override
    {
        if (mat.empty ()) return;

        cv::Mat src = mat.clone ();
        cv::RNG rng (static_cast<uint64>(0x1234ABCD15ULL) ^ static_cast<uint64>(fi.tick_num_ / speed_) * 12345678ULL);

        for (int y = 0; y < mat.rows; y += bh_)
        {
            for (int x = 0; x < mat.cols; x += bh_)
            {
                int w = std::min (bw_, mat.cols - x);
                int h = std::min (bh_, mat.rows - y);

                cv::Rect r (x, y, w, h);

                int dx = rng.uniform (-shift_, shift_ + 1);
                int dy = rng.uniform (-shift_, shift_ + 1);

                int nx = std::max (0, std::min (mat.cols - w, x + dx));
                int ny = std::max (0, std::min (mat.rows - h, y + dy));

                src (r).copyTo (mat (cv::Rect (nx, ny, w, h)));
            }
        }
    }

    void set_block_w (int neww) { bw_ = neww; }
    void set_block_h (int newh) { bh_ = newh; }
    void set_max_shift (int shift) { shift_ = shift; }
    void set_speed (int speed) { speed_ = speed; }
private:
    int bw_{32};
    int bh_{16};
    int shift_{12};
    int speed_{4};
};

#endif //CVEDIT_FILTER_PIXEL_DISPLACE_H