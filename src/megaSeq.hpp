#pragma once

#include "param_quantities.hpp"

namespace ltm {
struct MegaSeq : Module {
    enum Stage {
        GATE_STAGE,
        STOPPED_STAGE
    };

    enum ParamIds {
        ENUMS(CVSTEP_PARAM, 16),
        FIRSTSTEP_PARAM,
        LASTSTEP_PARAM,
        GATELENGTH1_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        FIRSTSTEP_INPUT,
        LASTSTEP_INPUT,
        STEP_INPUT,
        RESET_INPUT,
        PATTERNSELECT_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        GATE1_OUTPUT,
        GATE2_OUTPUT,
        CV_OUTPUT,
        SLEW_OUTPUT,
        EOC_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        ENUMS(CURRENTSTEP_LIGHT, 16),
        NUM_LIGHTS
    };

    MegaSeq() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        for (int i = 0; i < 16; i++) {
            configParam(CVSTEP_PARAM + i, -10.f, 10.f, 0.f, "", " V");
        }
        configParam(FIRSTSTEP_PARAM, 0.f, 15.f, 0.f, "firstStep", "", 0.f, 1.f, 1.f);
        configParam(LASTSTEP_PARAM, 0.f, 15.f, 15.f, "lastStep", "", 0.f, 1.f, 1.f);
        configParam<bogaudio::EnvelopeSegmentParamQuantity>(GATELENGTH1_PARAM, 0.0f, 1.0f, 0.31623f, "Gate", " s");

        onReset();
    }

    void onReset() override;
    void setIndex(int index, int resetStep, int numSteps);
    bool stepStage(float length);
    void processChannel(const ProcessArgs& args) override;

};
}  // namespace ltm

struct ltmMediumKnob : SVGKnob {
    ltmMediumKnob() {
        box.size = Vec(6, 6);
        minAngle = -0.75 * M_PI;
        maxAngle = 0.75 * M_PI;
        setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmMediumKnob.svg"));
    }

    /* data */
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

    /* data */
};

struct ltmSmallKnob : SVGKnob {
    ltmSmallKnob() {
        box.size = Vec(6, 6);
        minAngle = -0.75 * M_PI;
        maxAngle = 0.75 * M_PI;
        setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmSmallKnob.svg"));
    }

    /* data */
};
struct ltmInput : app::SvgPort {
    ltmInput() {
        setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmInput.svg"));
    }

    /* data */
};
struct ltmOutput : app::SvgPort {
    ltmOutput() {
        setSvg(APP->window->loadSvg("C:/Users/Admin/code_projects/GIT/puce/res/ComponentLibrary/ltmOutput.svg"));
    }
    /* data */
};