#pragma once
#include <rack.hpp>


using namespace rack;
struct ltmMediumKnob : SVGKnob
{
    ltmMediumKnob() {
        box.size = Vec(6,6);
        minAngle = -0.75*M_PI;
		maxAngle = 0.75*M_PI;
		setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmMediumKnob.svg"));
    }
    /* data */
};

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelMegaSeq;
// extern Model* modelMyModule;
