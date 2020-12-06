#include <stdio.h>

#include <cmath>
#include <iostream>

#include "plugin.hpp"
using namespace std;

char dbgBuffer[256];

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
    float step = 0.f;

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

    void setIndex(int index, int resetStep, int numSteps) {
        phase = 0.f;
        this->index = std::abs(index);

        if (this->index >= numSteps + 1) {
            this->index = resetStep;
        }
    }

    void process(const ProcessArgs &args) override {
        //  run

        // get value from params and inputs
        int firstStep = (int)clamp(roundf(params[FIRSTSTEP_PARAM].getValue() + (inputs[FIRSTSTEP_INPUT].getVoltage() * 1.5)), 0.f, 15.f);
        int lastStep = (int)clamp(roundf(params[LASTSTEP_PARAM].getValue() + (inputs[LASTSTEP_INPUT].getVoltage() * 1.5)), 0.f, 15.f);

        bool gateIn = false;
        if (running) {
            // si le step param est connecté
            if (inputs[STEP_INPUT].isConnected()) {
                if (clockTrigger.process(inputs[STEP_INPUT].getVoltage())) {
                    if (firstStep >= lastStep) {
                        if (index < lastStep + 1) {
                            index = firstStep + 1;
                        }
                        setIndex(index - 1, lastStep, firstStep);
                    } else {
                        setIndex(index + 1, firstStep, lastStep);
                    }
					gateIn = clockTrigger.isHigh();
                }
            }

            // float clocktime = std::pow(2.f, clockParams);
            // phase += clocktime * args.sampleTime;

            // if (phase >= 1.f) {
            //     // revert lecture
            //     if (firstStep >= lastStep) {
            //         if (index < lastStep + 1) {
            //             index = firstStep + 1;
            //         }
            //         setIndex(index - 1, lastStep, firstStep);
            //     } else {
            //         setIndex(index + 1, firstStep, lastStep);
            //     }
            // }
            // gateIn = (phase < 0.5f);
        }

        // select Param
        for (int i = 0; i < 16; i++) {
            if (gateTriggers[i].process(params[CVSTEP_PARAM + i].getValue())) {
                // affiche les parametres du step
            }
            outputs[GATE1_OUTPUT + i].setVoltage((running && gateIn && i == index && gates[i]) ? 10.f : 0.f);
            outputs[GATE2_OUTPUT + i].setVoltage((running && gateIn && i == index && gates[i]) ? 10.f : 0.f);

            lights[CURRENTSTEP_LIGHT + i].setSmoothBrightness(0.0, args.sampleTime * 256.f);
        }

        //  reset
        if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
            if (firstStep >= lastStep) {
                setIndex(lastStep, lastStep, firstStep);
            } else {
                setIndex(firstStep, firstStep, lastStep);
            }
        }

        // outputs
        outputs[CV_OUTPUT].setVoltage(params[CVSTEP_PARAM + index].getValue());

        outputs[GATE1_OUTPUT].setVoltage((gateIn && gates[index]) ? 10.f : 0.f);
        outputs[GATE2_OUTPUT].setVoltage((gateIn && gates[index]) ? 10.f : 0.f);
        lights[CURRENTSTEP_LIGHT + index].setSmoothBrightness(gateIn, args.sampleTime * 1024.f);
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

        addParam(createParamCentered<ltmSmallKnob>(mm2px(Vec(8.5, 60.)), module, MegaSeq::FIRSTSTEP_PARAM));
        addParam(createParamCentered<ltmSmallKnob>(mm2px(Vec(19.0, 60.)), module, MegaSeq::LASTSTEP_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.5, 68.)), module, MegaSeq::FIRSTSTEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.0, 68.)), module, MegaSeq::LASTSTEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.75, 90.)), module, MegaSeq::STEP_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.75, 105.)), module, MegaSeq::RESET_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.75, 120.)), module, MegaSeq::PATTERNSELECT_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(112.479, 59.677)), module, MegaSeq::GATE1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(112.857, 72.528)), module, MegaSeq::GATE2_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(112.668, 86.324)), module, MegaSeq::CV_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(113.046, 100.31)), module, MegaSeq::SLEW_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(113.235, 115.996)), module, MegaSeq::EOC_OUTPUT));
    }
};

Model *modelMegaSeq = createModel<MegaSeq, MegaSeqWidget>("megaSeq");