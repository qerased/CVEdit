#ifndef CVEDIT_FILTER_SORT_H
#define CVEDIT_FILTER_SORT_H
#include "filter.h"

#include <QMetaType>
#include <random>

enum class sort_mode { Luminosity, Red, Green, Blue, Hue};
Q_DECLARE_METATYPE (sort_mode);

enum class sort_scope { Global, Rows, Cols};
Q_DECLARE_METATYPE (sort_scope);

enum class sort_axis { Horizontal, Vertical};
Q_DECLARE_METATYPE (sort_axis);

class filter_sort : public filter
{
public:

    const char * id () const { return "sort"; }

    void apply (cv::Mat &mat) override
    {
        if (mat.empty ()) return;

        switch (scope_)
        {
            case sort_scope::Global: sort_global (mat); break;
            case sort_scope::Rows:   sort_rows (mat); break;
            case sort_scope::Cols:   sort_cols (mat); break;
        }
    }

    void set_mode (const sort_mode mode) { mode_ = mode; }
    void set_chunk (const unsigned int chunk){ chunk_ = chunk; }
    void set_stride (const unsigned int stride) { stride_ = stride; }
    void set_scope (const sort_scope scope) { scope_ = scope; }
    void set_axis (const sort_axis axis) { axis_ = axis; }

    void set_random_mask_enabled (const bool enabled) { use_random_mask_ = enabled; }
    void set_mask_prob (const double probability) { mask_prob_ = probability; }

    void set_threshold_enabled (const bool enabled) { thr_enabled_ = enabled; }
    void set_threshold_lo (unsigned int lo)
    {
        thr_lo_ = lo;
        if (thr_lo_ > thr_hi_) std::swap (thr_lo_, thr_hi_);
    }
    void set_threshold_hi (unsigned int hi)
    {
        thr_hi_ = hi;
        if (thr_lo_ > thr_hi_) std::swap (thr_lo_, thr_hi_);
    }

    void enable_rand_chunk (const bool enable) { use_rand_chunk_ = enable; }

private:
    sort_mode mode_ {sort_mode::Luminosity};
    sort_scope scope_ { sort_scope::Global };
    sort_axis axis_ { sort_axis::Horizontal };

    unsigned int chunk_ {0}; /// 0 == all, >0 = size of interval
    unsigned int stride_ {0}; /// 0 == non-overlapping, >0 = step

    bool use_random_mask_{false};
    double mask_prob_{0.0};
    std::mt19937 rng_;

    bool thr_enabled_{false};
    unsigned int thr_lo_{40}, thr_hi_{190};

    bool use_rand_chunk_{false};

    void sort_global (cv::Mat & mat);
    void sort_rows (cv::Mat & mat);
    void sort_cols (cv::Mat & mat);

    int key_bins () const { return mode_ == sort_mode::Hue ? 180 : 256; }
    uint8_t pixel_key_bgr (const cv::Vec3b & p) const
    {
        switch (mode_)
        {
            case sort_mode::Red:   return p[2];
            case sort_mode::Green: return p[1];
            case sort_mode::Blue:  return p[0];
            case sort_mode::Luminosity:
                return static_cast<uint8_t> (0.299 * p[2] + 0.587 * p[1] + 0.114 * p[0]);
            case sort_mode::Hue:;
        }
        return 0;
    }
};

#endif //CVEDIT_FILTER_SORT_H