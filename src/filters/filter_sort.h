#ifndef CVEDIT_FILTER_SORT_H
#define CVEDIT_FILTER_SORT_H
#include "filter.h"

#include <QMetaType>

enum class sort_mode { Luminosity, Red, Green, Blue, Hue};
Q_DECLARE_METATYPE (sort_mode);

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
            case sort_mode::Hue:
            case sort_mode::Luminosity:
            default:
                break;
        }
    }

    void set_mode (const sort_mode mode)
    {
        mode_ = mode;
    }
private:
    sort_mode mode_ {sort_mode::Luminosity};

    void counting_scatter (const uint8_t * keys, int bins,
                           const cv::Mat & src, cv::Mat & dst) const;

    void sort_by_channel (cv::Mat & mat, int ch, int bins) const;
};

#endif //CVEDIT_FILTER_SORT_H