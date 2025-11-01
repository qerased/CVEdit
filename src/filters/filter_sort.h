#ifndef CVEDIT_FILTER_SORT_H
#define CVEDIT_FILTER_SORT_H
#include "filter.h"

#include <QMetaType>

enum class sort_mode { Luminosity, Red, Green, Blue, Hue};
Q_DECLARE_METATYPE (sort_mode);

enum class sort_scope { Global, Rows, Cols};
Q_DECLARE_METATYPE (sort_scope);

enum class sort_axis { Horizontal, Vertical};
Q_DECLARE_METATYPE (sort_axis);

class filter_sort : public filter
{
public:

    const char * name () const { return "Sort"; }

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

    void set_mode (const sort_mode mode)
    {
        mode_ = mode;
    }

    void set_chunk (const unsigned int chunk)
    {
        chunk_ = chunk;
    }

    void set_stride (const unsigned int stride)
    {
        stride_ = stride;
    }

private:
    sort_mode mode_ {sort_mode::Luminosity};
    sort_scope scope_ { sort_scope::Rows };
    sort_axis axis_ { sort_axis::Vertical };

    unsigned int chunk_ {5}; /// 0 == all, >0 = size of interval
    unsigned int stride_ {10}; /// 0 == non-overlapping, >0 = step

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