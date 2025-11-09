#ifndef CVEDIT_FILTER_KUWAHARA_H
#define CVEDIT_FILTER_KUWAHARA_H
#include "filter.h"

struct kuwahara_params
{
    Q_GADGET
    Q_PROPERTY (int k_size)

public:
    int k_size{3}; /// min k = 3 and must be odd
};

class filter_kuwahara : public filter
{
public:
    const char * id () const override { return "kuwahara"; }

    void apply (cv::Mat & mat) override
    {
        if (mat.empty ()) return;

        const int rows = mat.rows;
        const int cols = mat.cols;

        const int r = params_.k_size / 2;
        const int q = r + 1;

        cv::Mat hsv;
        cv::cvtColor (mat, hsv, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> hsv_split;
        cv::split (hsv, hsv_split);
        cv::Mat v = hsv_split[2];

        cv::Mat bgr[3];
        cv::split (mat, bgr);

        cv::Mat iv, iv2;
        cv::integral (v, iv, iv2, CV_64F);

        cv::Mat iR, iG, iB;
        cv::integral (bgr[2], iR, CV_64F);
        cv::integral (bgr[1], iG, CV_64F);
        cv::integral (bgr[0], iB, CV_64F);

        auto sum_rect = [](const cv::Mat & ii, int x0, int y0, int x1, int y1)
        {
            const double * p00 = ii.ptr<double> (y0);
            double a = ii.at<double> (y0 + 0, x0 + 0);
            double b = ii.at<double> (y0 + 0, x1 + 1);
            double c = ii.at<double> (y1 + 1, x0 + 0);
            double d = ii.at<double> (y1 + 1, x1 + 1);

            return a - b - c + d;
        };

        auto clamp_rect = [rows, cols](int & x0, int & y0, int & x1, int & y1)
        {
            x0 = std::max (0, std::min (x0, cols - 1));
            x1 = std::max (0, std::min (x1, cols - 1));
            y0 = std::max (0, std::min (y0, rows - 1));
            y1 = std::max (0, std::min (y1, rows - 1));

            if (x1 < x0) std::swap (x0, x1);
            if (y1 < y0) std::swap (y0, y1);
        };

        cv::Mat out (rows, cols, CV_8UC3);

        for (int y = 0; y < rows; y++)
        {
            int y0 = y - r, y1 = y, y2 = y + r;

            for (int x = 0; x < cols; x++)
            {
                int x0 = x - r, x1 = x, x2 = x + r;

                /// NW: [x0..x1] x [y0..y1]
                /// NW: [x1..x2] x [y0..y1]
                /// NW: [x0..x1] x [y1..y2]
                /// NW: [x1..x2] x [y1..y2]
                int rx0[4] = { x0, x1, x0, x1};
                int rx1[4] = { x1, x2, x1, x2};
                int ry0[4] = { y0, y0, y1, y1};
                int ry1[4] = { y1, y2, y1, y2};

                double best_var = std::numeric_limits<double>::infinity ();
                int bx0{}, by0{}, bx1{}, by1{};
                int best_area{1};

                for (int i = 0; i < 4; i++)
                {
                    int xx0 = rx0[i]; int yy0 = ry0[i];
                    int xx1 = rx1[i]; int yy1 = ry1[i];

                    clamp_rect (xx0, yy0, xx1, yy1);
                    const int area = (xx1 - xx0 + 1) * (yy1 - yy0 + 1);

                    const double s  = sum_rect (iv,  xx0, yy0, xx1, yy1);
                    const double s2 = sum_rect (iv2, xx0, yy0, xx1, yy1);
                    const double mean = s / area;
                    double var = s2 / area - mean * mean;

                    if (var < 0.) var = 0.;

                    if (var < best_var)
                    {
                        best_var = var;
                        bx0 = xx0; by0 = yy0;
                        bx1 = xx1; by1 = yy1;
                        best_area = area;
                    }
                }

                const double sr = sum_rect (iR, bx0, by0, bx1, by1);
                const double sg = sum_rect (iG, bx0, by0, bx1, by1);
                const double sb = sum_rect (iB, bx0, by0, bx1, by1);

                cv::Vec3b & dst = out.at<cv::Vec3b> (y, x);
                dst[2] = static_cast<uchar> (std::clamp (sr / best_area, 0., 255.));
                dst[1] = static_cast<uchar> (std::clamp (sg / best_area, 0., 255.));
                dst[0] = static_cast<uchar> (std::clamp (sb / best_area, 0., 255.));
            }
        }

        out.copyTo (mat);
    }

    void set_k_size (int k)
    {
        params_.k_size = std::max (3, k - !(k % 2));
    }

    bool set_parameters (const QJsonObject &json) override
    {
        kuwahara_params tmp = params_;
        bool ok = json_to_filter (&tmp, kuwahara_params::staticMetaObject, json);
        if (ok)
        {
            params_ = tmp;
            params_.k_size = std::max (3, params_.k_size - !(params_.k_size % 2));
        }
        return ok;
    }

    QJsonObject parameters() const override
    {
        return filter_to_json (&params_, kuwahara_params::staticMetaObject);
    }

private:
    kuwahara_params params_;
};

#endif //CVEDIT_FILTER_KUWAHARA_H