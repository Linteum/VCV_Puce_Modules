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
struct ltmSmallSnapKnob : SVGKnob
{
    ltmSmallSnapKnob() {
        snap = true;
        smooth = false;
        box.size = Vec(6,6);
        minAngle = -0.75*M_PI;
		maxAngle = 0.75*M_PI;
		setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmSmallKnob.svg"));
    }

    
    /* data */
};

struct ltmSmallKnob : SVGKnob
{
    ltmSmallKnob() {
        box.size = Vec(6,6);
        minAngle = -0.75*M_PI;
		maxAngle = 0.75*M_PI;
		setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmSmallKnob.svg"));
    }

    
    /* data */
};
struct ltmInput : app::SvgPort
{
    ltmInput() {
		setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmInput.svg"));
    }

    
    /* data */
};
struct ltmOutput : app::SvgPort
{
    ltmOutput() {
		setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmOutput.svg"));
    }
    /* data */
};

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelMegaSeq;
// extern Model* modelMyModule;
