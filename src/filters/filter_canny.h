#ifndef CVEDIT_FILTER_CANNY_H
#define CVEDIT_FILTER_CANNY_H
#include "filter.h"


class filter_canny : public filter
{
public:
    const char * name () const override { return "Canny"; }

    void apply (cv::Mat &mat) override
    {
        if (mat.empty ()) return;
        cv::Mat edges;
        cv::Canny (mat, edges, thresh1, thresh2);
        cv::cvtColor (edges, mat, cv::COLOR_GRAY2BGR);
    }

    void set_thr1 (double new_thr1)
    {
        thresh1 = new_thr1;
    }

    void set_thr2 (double new_thr2)
    {
        thresh2 = new_thr2;
    }

private:
    double thresh1{100}, thresh2{200};
};

#endif //CVEDIT_FILTER_CANNY_H