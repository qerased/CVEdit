#include "main_window.h"

void main_window::bind_toggle (QCheckBox *& chk, filter * f)
{
    connect(chk, &QCheckBox::toggled, this,
            [this, f] (bool on) {
                f->set_enabled (on);
                if (!is_live_) reprocess_and_show ();
            });
}

void main_window::bind_chkbox (QCheckBox *& chk, std::function<void(bool)> trigger)
{
    connect(chk, &QCheckBox::stateChanged, this,
            [this, trigger] (bool on) {
                trigger (on);
                if (!is_live_) reprocess_and_show ();
            });
}

void main_window::bind_slider (QSlider *& s, std::function<void(int)> setter)
{
    connect(s, &QSlider::valueChanged, this,
            [this, setter] (int v) {
                setter (v);
                if (!is_live_) reprocess_and_show ();
            });
}

void main_window::bind_spin (QSpinBox *& s, std::function<void(int)> setter)
{
    connect(s, &QSpinBox::valueChanged, this,
        [this, setter](int v)
        {
            setter (v);
            if (!is_live_) reprocess_and_show ();
        });
}

void main_window::bind_combo (QComboBox *& c, std::function<void(const QVariant &)> setter)
{
    connect (c, qOverload<int> (&QComboBox::currentIndexChanged), this,
        [this, c, setter] (int idx)
        {
            setter (c->itemData (idx));
            if (!is_live_) reprocess_and_show ();
        });
}

QHBoxLayout * main_window::get_chk_ord_layout (QGroupBox *& box, filter * f, QCheckBox *& chk, QSpinBox *& spin)
{
    auto * gen_layout = new QHBoxLayout ();

    chk = new QCheckBox ("Enable", box);
    chk->setChecked (false);
    bind_toggle (chk, f);

    gen_layout->addWidget (chk);

    auto * ord_layout = new QHBoxLayout ();
    ord_layout->addWidget (new QLabel ("Order :", box));
    spin = new QSpinBox (box);
    spin->setRange (0, 300);
    spin->setValue (0);
    bind_spin (spin, [fil = f] (int o) { fil->set_num_order (o); });

    ord_layout->addWidget (spin);

    gen_layout->addLayout (ord_layout);
    return gen_layout;
}

QHBoxLayout * main_window::get_slider (
    QGroupBox *& box, QString label, QSlider *& slider,
    int base_val, int min_val, int max_val,
    std::function<void(int)> slider_f)
{
    QHBoxLayout * layout = new QHBoxLayout ();
    layout->addWidget (new QLabel (label, box));

    slider = new QSlider (Qt::Horizontal, box);
    slider->setValue (base_val);
    slider->setMinimum (min_val);
    slider->setMaximum (max_val);
    bind_slider (slider, slider_f);
    layout->addWidget (slider);

    return layout;
}

QHBoxLayout * main_window::get_spin (
    QGroupBox *& box, QString label, QSpinBox *& spin,
    int base_val, int min_val, int max_val,
    std::function<void(int)> spin_f)
{
    auto * layout = new QHBoxLayout ();
    layout->addWidget (new QLabel (label, box));

    spin = new QSpinBox (box);
    spin->setRange (min_val, max_val);
    spin->setValue (base_val);
    layout->addWidget (spin);

    bind_spin (spin, spin_f);

    return layout;
}

QHBoxLayout * main_window::get_checkbox (
    QGroupBox *& box, QString label,
    QCheckBox *& chk,
    std::function<void(bool)> trigger)
{
    auto * layout = new QHBoxLayout ();

    chk = new QCheckBox (label, box);
    bind_chkbox (chk, trigger);
    layout->addWidget (chk);

    return layout;
}

