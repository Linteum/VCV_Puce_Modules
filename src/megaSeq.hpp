#pragma once
#include <cmath>

#include "plugin.hpp"

template <int scale>
struct ScaledSquaringParamQuantity : ParamQuantity {
    float getDisplayValue() override {
        float v = getValue();
        if (!module) {
            return v;
        }

        float vv = v * v;
        vv *= (float)scale;
        vv += displayOffset;
        if (v < 0.0f) {
            return -vv;
        }
        return vv;
    }

    void setDisplayValue(float displayValue) override {
        if (!module) {
            return;
        }
        displayValue -= displayOffset;
        float v = fabsf(displayValue) / (float)abs(scale);
        v = powf(v, 0.5f);
        if (displayValue < 0.0f && scale > 0) {
            setValue(-v);
        } else {
            setValue(v);
        }
    }
};

typedef ScaledSquaringParamQuantity<1> OneXSquaringParamQuantity;

typedef ScaledSquaringParamQuantity<10> TenXSquaringParamQuantity;

typedef TenXSquaringParamQuantity EnvelopeSegmentParamQuantity;

extern Model* modelMegaSeq;

struct MegaSeq : Module {
    enum Stage {
        GATE_STAGE,
        STOPPED_STAGE
    };

    struct Engine {
        Stage stage;
        int firstStep;
        int lastStep;

        dsp::SchmittTrigger trigger;
        dsp::PulseGenerator triggerOuptutPulseGen;
        float stageProgress;
        float delayLight;
        float gateLight;
        float phase;
        float step;
        int index;
        bool gates1[16];
        bool gates2[16];
        void reset();
    };

    Engine* _engines[16];
    float phase;
    bool running = true;
    int genIndex;

    enum ParamIds {
        ENUMS(CVSTEP_PARAM, 16),
        FIRSTSTEP_PARAM,
        LASTSTEP_PARAM,
        GATELENGTH1_PARAM,
        GATELENGTH2_PARAM,
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

    dsp::SchmittTrigger clockTrigger;
    dsp::SchmittTrigger runningTrigger;
    dsp::SchmittTrigger resetTrigger;
    dsp::SchmittTrigger gateTriggers[16];

    
    
    MegaSeq() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        for (int i = 0; i < 16; i++) {
            configParam(CVSTEP_PARAM + i, -10.f, 10.f, 0.f, "", " V");
        }
        configParam(FIRSTSTEP_PARAM, 0.f, 15.f, 0.f, "firstStep", "", 0.f, 1.f, 1.f);
        configParam(LASTSTEP_PARAM, 0.f, 15.f, 15.f, "lastStep", "", 0.f, 1.f, 1.f);
        configParam<EnvelopeSegmentParamQuantity>(GATELENGTH1_PARAM, 0.0f, 1.0f, 0.31623f, "Gate", " s");
    }

    virtual ~MegaSeq();

    int channels();
    void addChannel(int c);
    void removeChannel(int c);

    void reset();
    void onReset() override;
    void setIndex(int c, int index, int resetStep, int numSteps);
    bool stepStage(int c, Param& knob);
    void processChannel(const ProcessArgs& args);
};

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