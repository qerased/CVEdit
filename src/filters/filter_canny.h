#ifndef CVEDIT_FILTER_CANNY_H
#define CVEDIT_FILTER_CANNY_H
#include "filter.h"
#include "filter_sort.h"

struct canny_params
{
    Q_GADGET
    Q_PROPERTY (double thresh1 MEMBER thresh1)
    Q_PROPERTY (double thresh2 MEMBER thresh2)
    Q_PROPERTY (bool replace MEMBER replace)
public:
    double thresh1{100};
    double thresh2{200};
    bool replace{false};
};

class filter_canny : public filter
{
public:
    const char * id () const override { return "canny"; }

    void apply (cv::Mat &mat) override
    {
        if (mat.empty ()) return;
        cv::Mat edges;
        cv::Canny (mat, edges, params_.thresh1, params_.thresh2);
        if (params_.replace)
            cv::cvtColor (edges, mat, cv::COLOR_GRAY2RGB);
        else
        {
            cv::cvtColor (edges, edges, cv::COLOR_GRAY2BGR);
            mat += edges;
        }
    }

    void set_thr1 (double new_thr1) { params_.thresh1 = new_thr1; }
    void set_thr2 (double new_thr2) { params_.thresh2 = new_thr2; }
    void set_replace (bool new_replace) { params_.replace = new_replace; }

    bool set_parameters (const QJsonObject &json) override
    {
        canny_params tmp = params_;
        bool ok = json_to_filter (&tmp, canny_params::staticMetaObject, json);
        if (ok)
            params_ = tmp;
        return ok;
    }

    QJsonObject parameters () const override
    {
        return filter_to_json (&params_, canny_params::staticMetaObject);
    }

private:
    canny_params params_;
};

#endif //CVEDIT_FILTER_CANNY_H