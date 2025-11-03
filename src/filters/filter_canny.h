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
        if (replace)
            cv::cvtColor (edges, mat, cv::COLOR_GRAY2RGB);
        else
        {
            cv::cvtColor (edges, edges, cv::COLOR_GRAY2BGR);
            mat += edges;
        }
    }

    void set_thr1 (double new_thr1)
    {
        thresh1 = new_thr1;
    }

    void set_thr2 (double new_thr2)
    {
        thresh2 = new_thr2;
    }

    void set_replace (bool new_replace)
    {
        replace = new_replace;
    }

private:
    double thresh1{100}, thresh2{200};
    bool replace {false};
};

#endif //CVEDIT_FILTER_CANNY_H