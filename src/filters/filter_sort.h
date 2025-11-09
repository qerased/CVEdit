#ifndef CVEDIT_FILTER_SORT_H
#define CVEDIT_FILTER_SORT_H
#include "filter.h"

#include <QMetaType>
#include <random>

namespace psort
{
    Q_NAMESPACE

    enum class sort_mode  { Luminosity, Red, Green, Blue, Hue };
    Q_ENUM_NS(sort_mode)

    enum class sort_scope { Global, Rows, Cols };
    Q_ENUM_NS(sort_scope)

    enum class sort_axis  { Horizontal, Vertical };
    Q_ENUM_NS(sort_axis)
}

Q_DECLARE_METATYPE (psort::sort_mode);
Q_DECLARE_METATYPE (psort::sort_scope);
Q_DECLARE_METATYPE (psort::sort_axis);

struct sort_params
{
    Q_GADGET
    Q_PROPERTY(psort::sort_mode  mode  MEMBER mode)
    Q_PROPERTY(psort::sort_scope scope MEMBER scope)
    Q_PROPERTY(psort::sort_axis  axis  MEMBER axis)

    Q_PROPERTY (unsigned int chunk MEMBER chunk)
    Q_PROPERTY (unsigned int stride MEMBER stride)

    Q_PROPERTY (bool use_random_mask MEMBER use_random_mask)
    Q_PROPERTY (double mask_prob MEMBER mask_prob)
    Q_PROPERTY (bool thresh_enabled MEMBER thresh_enabled)

    Q_PROPERTY (unsigned int thr_lo MEMBER thr_lo)
    Q_PROPERTY (unsigned int thr_hi MEMBER thr_hi)

    Q_PROPERTY (bool use_rand_chunk MEMBER use_rand_chunk)

public:
    psort::sort_mode mode{psort::sort_mode::Luminosity};
    psort::sort_scope scope{psort::sort_scope::Global};
    psort::sort_axis axis{psort::sort_axis::Horizontal};

    unsigned int chunk{0}; /// 0 == all, >0 = size of interval
    unsigned int stride{0}; /// 0 == non-overlapping, >0 = step

    bool use_random_mask{false};
    double mask_prob{0.};

    bool thresh_enabled{false};
    unsigned int thr_lo{40};
    unsigned int thr_hi{190};

    bool use_rand_chunk{false};
};

class filter_sort : public filter
{
public:

    const char * id () const override { return "pixel_sort"; }

    void apply (cv::Mat &mat) override
    {
        if (mat.empty ()) return;

        switch (params_.scope)
        {
            case psort::sort_scope::Global: sort_global (mat); break;
            case psort::sort_scope::Rows:   sort_rows (mat); break;
            case psort::sort_scope::Cols:   sort_cols (mat); break;
        }
    }

    void set_mode (const psort::sort_mode mode) { params_.mode = mode; }
    void set_chunk (const unsigned int chunk){ params_.chunk = chunk; }
    void set_stride (const unsigned int stride) { params_.stride = stride; }
    void set_scope (const psort::sort_scope scope) { params_.scope = scope; }
    void set_axis (const psort::sort_axis axis) { params_.axis = axis; }

    void set_random_mask_enabled (const bool enabled) { params_.use_random_mask = enabled; }
    void set_mask_prob (const double probability) { params_.mask_prob = probability; }

    void set_threshold_enabled (const bool enabled) { params_.thresh_enabled = enabled; }
    void set_threshold_lo (unsigned int lo)
    {
        params_.thr_lo = lo;
        if (params_.thr_lo > params_.thr_hi)
            std::swap (params_.thr_lo, params_.thr_hi);
    }
    void set_threshold_hi (unsigned int hi)
    {
        params_.thr_hi = hi;
        if (params_.thr_lo > params_.thr_hi)
            std::swap (params_.thr_lo, params_.thr_hi);
    }

    void enable_rand_chunk (const bool enable) { params_.use_rand_chunk = enable; }

    bool set_parameters (const QJsonObject & json) override
    {
        sort_params tmp = params_;
        bool ok = json_to_filter (&tmp, sort_params::staticMetaObject, json);
        if (ok)
        {
            params_ = tmp;
            if (params_.thr_lo > params_.thr_hi)
                std::swap (params_.thr_lo, params_.thr_hi);
        }
        return ok;
    }

    QJsonObject parameters () const override
    {
        return filter_to_json (&params_, sort_params::staticMetaObject);
    }
private:
    sort_params params_;
    std::mt19937 rng_;

    void sort_global (cv::Mat & mat);
    void sort_rows (cv::Mat & mat);
    void sort_cols (cv::Mat & mat);

    int key_bins () const { return params_.mode == psort::sort_mode::Hue ? 180 : 256; }
    uint8_t pixel_key_bgr (const cv::Vec3b & p) const
    {
        switch (params_.mode)
        {
            case psort::sort_mode::Red:   return p[2];
            case psort::sort_mode::Green: return p[1];
            case psort::sort_mode::Blue:  return p[0];
            case psort::sort_mode::Luminosity:
                return static_cast<uint8_t> (0.299 * p[2] + 0.587 * p[1] + 0.114 * p[0]);
            case psort::sort_mode::Hue:;
        }
        return 0;
    }
};

#endif //CVEDIT_FILTER_SORT_H