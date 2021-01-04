#include <cmath>
#include <rack.hpp>

using namespace rack;

extern Plugin* pluginInstance;

extern Model* modelMegaSeq;

// custom svgs

struct ltmMediumKnob : SVGKnob {
    ltmMediumKnob() {
        box.size = Vec(6, 6);
        minAngle = -0.75 * M_PI;
        maxAngle = 0.75 * M_PI;
        setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmMediumKnob.svg"));
    }
};
struct ltmSmallSnapKnob : SVGKnob {
    ltmSmallSnapKnob() {
        snap = true;
        smooth = false;
        box.size = Vec(6, 6);
        minAngle = -0.75 * M_PI;
        maxAngle = 0.75 * M_PI;
        setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmSmallKnob.svg"));
    }
};

struct ltmSmallKnob : SVGKnob {
    ltmSmallKnob() {
        box.size = Vec(6, 6);
        minAngle = -0.75 * M_PI;
        maxAngle = 0.75 * M_PI;
        setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmSmallKnob.svg"));
    }
};
struct ltmInput : app::SvgPort {
    ltmInput() {
        setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmInput.svg"));
    }
};
struct ltmOutput : app::SvgPort {
    ltmOutput() {
        setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmOutput.svg"));
    }
};

