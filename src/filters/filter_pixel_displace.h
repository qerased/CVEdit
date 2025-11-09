#ifndef CVEDIT_FILTER_PIXEL_DISPLACE_H
#define CVEDIT_FILTER_PIXEL_DISPLACE_H
#include "filter.h"

struct pixel_displace_params
{
    Q_GADGET
    Q_PROPERTY (int bw MEMBER bw)
    Q_PROPERTY (int bh MEMBER bh)
    Q_PROPERTY (int shift MEMBER shift)
    Q_PROPERTY (int speed MEMBER speed)

public:
    int bw{32};
    int bh{16};
    int shift{12};
    int speed{4};
};

class filter_pixel_displace : public filter
{
public:

    const char * id () const override { return "pixel_displace"; }

    void apply (cv::Mat & mat) override
    {
        frame_info fake{0};
        apply (mat, fake);
    }

    void apply (cv::Mat &mat, frame_info &fi) override
    {
        if (mat.empty ()) return;

        cv::Mat src = mat.clone ();
        cv::RNG rng (static_cast<uint64>(0x1234ABCD15ULL) ^ static_cast<uint64>(fi.tick_num_ / params_.speed) * 12345678ULL);

        for (int y = 0; y < mat.rows; y += params_.bh)
        {
            for (int x = 0; x < mat.cols; x += params_.bh)
            {
                int w = std::min (params_.bw, mat.cols - x);
                int h = std::min (params_.bh, mat.rows - y);

                cv::Rect r (x, y, w, h);

                int dx = rng.uniform (-params_.shift, params_.shift + 1);
                int dy = rng.uniform (-params_.shift, params_.shift + 1);

                int nx = std::max (0, std::min (mat.cols - w, x + dx));
                int ny = std::max (0, std::min (mat.rows - h, y + dy));

                src (r).copyTo (mat (cv::Rect (nx, ny, w, h)));
            }
        }
    }

    void set_block_w (int neww) { params_.bw = neww; }
    void set_block_h (int newh) { params_.bh = newh; }
    void set_max_shift (int shift) { params_.shift = shift; }
    void set_speed (int speed) { params_.speed = speed; }

    bool set_parameters (const QJsonObject &json) override
    {
        pixel_displace_params tmp = params_;
        bool ok = json_to_filter (&tmp, pixel_displace_params::staticMetaObject, json);
        if (ok)
        {
            params_ = tmp;
            params_.bw = std::max(1, params_.bw);
            params_.bh = std::max(1, params_.bh);
            params_.shift = std::max(0, params_.shift);
            params_.speed = std::max(1, params_.speed);
        }
        return ok;
    }

    QJsonObject parameters() const override
    {
        return filter_to_json (&params_, pixel_displace_params::staticMetaObject);
    }

private:
    pixel_displace_params params_;
};

#endif //CVEDIT_FILTER_PIXEL_DISPLACE_H