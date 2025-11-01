#ifndef CVEDIT_FILTER_SORT_H
#define CVEDIT_FILTER_SORT_H
#include "filter.h"

#include <QMetaType>

enum class sort_mode { Luminosity, Red, Green, Blue, Hue};
Q_DECLARE_METATYPE (sort_mode);

enum class sort_scope { Global, Rows, Cols};
Q_DECLARE_METATYPE (sort_scope);

class filter_sort : public filter
{
public:

    const char * name () const { return "Sort"; }

    void apply (cv::Mat &mat) override
    {
        if (mat.empty ()) return;

        switch (mode_)
        {
            case sort_mode::Red:   sort_by_channel (mat, 2, 256); break;
            case sort_mode::Green: sort_by_channel (mat, 1, 256); break;
            case sort_mode::Blue:  sort_by_channel (mat, 0, 256); break;
            case sort_mode::Hue:   sort_by_hue (mat); break;
            case sort_mode::Luminosity: sort_by_luma(mat); break;
            default:
                break;
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
    sort_scope scope_ { sort_scope::Global };

    unsigned int chunk_ {0}; /// 0 == all, >0 = size of interval
    unsigned int stride_ {0}; /// 0 == non-overlapping, >0 = step

    void counting_scatter (const uint8_t * keys, int bins,
                           const cv::Mat & src, cv::Mat & dst) const;

    void sort_by_channel (cv::Mat & mat, int ch, int bins) const;
    void sort_by_luma (cv::Mat & mat) const;
    void sort_by_hue  (cv::Mat & mat) const;
};

#endif //CVEDIT_FILTER_SORT_H