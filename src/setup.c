#include "setup.h"
#include "config.h"
#include "colordefaults.h"

void setup_run() {
    cfg cf = cfg_open("colors","w");
    cfg_clear(cf);
    cfg_writeprop(cf, "colors_enabled", COLORSTATE_DEFAULT);
    cfg_writeprop(cf, "color_bg", BG_DEFAULT);
    cfg_writeprop(cf, "color_axes", AXES_DEFAULT);
    cfg_writeprop(cf, "color_curve1", CURVE1_DEFAULT);
    cfg_writeprop(cf, "color_curve2", CURVE2_DEFAULT);
    cfg_writeprop(cf, "color_curve3", CURVE3_DEFAULT);
    cfg_free(cf);
}
