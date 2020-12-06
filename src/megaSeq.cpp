#include <cmath>
#include <iostream>

#include "plugin.hpp"
using namespace std;

struct MegaSeq : Module {
    enum ParamIds {
        ENUMS(CVSTEP_PARAM, 16),
        FIRSTSTEP_PARAM,
        LASTSTEP_PARAM,
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

    float clockParams = 2.f;
    bool running = true;
    dsp::SchmittTrigger clockTrigger;
    dsp::SchmittTrigger runningTrigger;
    dsp::SchmittTrigger resetTrigger;
    dsp::SchmittTrigger gateTriggers[16];

    /** Phase of internal LFO */
    float phase = 0.f;
    int index = 0;
    bool gates[16] = {};

    MegaSeq() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        for (int i = 0; i < 16; i++) {
            configParam(CVSTEP_PARAM + i, -10.f, 10.f, 0.f, "", " V");
        }
        configParam(FIRSTSTEP_PARAM, 0.f, 15.f, 0.f, "firstStep", "", 0.f, 1.f, 1.f);
        configParam(LASTSTEP_PARAM, 0.f, 15.f, 15.f, "lastStep", "", 0.f, 1.f, 1.f);

        onReset();
    }

    void onReset() override {
        for (int i = 0; i < 16; i++) {
            gates[i] = true;
        }
    }

    void setIndex(int index, int firstStep, int lastStep) {
        int numSteps = lastStep;
        phase = 0.f;
        this->index = index;
        if (this->index >= numSteps) {
            this->index = firstStep;
        }
    }

    void process(const ProcessArgs &args) override {
        //  run
        int firstStep = floorf(params[FIRSTSTEP_PARAM].getValue());
        int lastStep = floorf(params[LASTSTEP_PARAM].getValue() + 1);
        bool gateIn = false;
        if (running) {
            float clocktime = std::pow(2.f, clockParams);
            phase += clocktime * args.sampleTime;
            if (phase >= 1.f) {
                setIndex(index + 1, firstStep, lastStep);
            }
            gateIn = (phase < 0.5f);
        }

        // select Param
        for (int i = 0; i < 16; i++) {
            if (gateTriggers[i].process(params[CVSTEP_PARAM + i].getValue())) {
                // affiche les parametres du step
            }
            outputs[GATE1_OUTPUT + i].setVoltage((running && gateIn && i == index && gates[i]) ? 10.f : 0.f);
            outputs[GATE2_OUTPUT + i].setVoltage((running && gateIn && i == index && gates[i]) ? 10.f : 0.f);

            lights[CURRENTSTEP_LIGHT + i].setSmoothBrightness(0.0, args.sampleTime);
        }

        //  reset
        if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
            setIndex(firstStep, firstStep, lastStep);
        }

        // outputs
        outputs[CV_OUTPUT].setVoltage(params[CVSTEP_PARAM + index].getValue());

        outputs[GATE1_OUTPUT].setVoltage((gateIn && gates[index]) ? 10.f : 0.f);
        outputs[GATE2_OUTPUT].setVoltage((gateIn && gates[index]) ? 10.f : 0.f);
        lights[CURRENTSTEP_LIGHT + index].setSmoothBrightness(gateIn, args.sampleTime);
    }
};

struct MegaSeqWidget : ModuleWidget {
    MegaSeqWidget(MegaSeq *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/megaSeq.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        static const float knobPositionY[4] = {60.37, 78.135, 95.9, 113.664};
        static const float knobPositionX[4] = {36.76, 54.399, 72.038, 89.677};

        int inc = 0;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(knobPositionX[j], knobPositionY[i])), module, MegaSeq::CURRENTSTEP_LIGHT + inc));
                addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(knobPositionX[j], knobPositionY[i])), module, MegaSeq::CVSTEP_PARAM + inc));

                inc++;
            }
        }

        addParam(createParamCentered<ltmSmallKnob>(mm2px(Vec(8.6, 66.429)), module, MegaSeq::FIRSTSTEP_PARAM));
        addParam(createParamCentered<ltmSmallKnob>(mm2px(Vec(19.088, 66.429)), module, MegaSeq::LASTSTEP_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.6, 53.1)), module, MegaSeq::FIRSTSTEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.088, 53.1)), module, MegaSeq::LASTSTEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.7, 75.552)), module, MegaSeq::STEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.449, 90.671)), module, MegaSeq::RESET_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.449, 108.814)), module, MegaSeq::PATTERNSELECT_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(112.479, 59.677)), module, MegaSeq::GATE1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(112.857, 72.528)), module, MegaSeq::GATE2_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(112.668, 86.324)), module, MegaSeq::CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(113.046, 100.31)), module, MegaSeq::SLEW_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(113.235, 115.996)), module, MegaSeq::EOC_OUTPUT));
    }
};

Model *modelMegaSeq = createModel<MegaSeq, MegaSeqWidget>("megaSeq");