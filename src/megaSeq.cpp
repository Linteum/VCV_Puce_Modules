#include <stdio.h>

#include <cmath>
#include <iostream>

#include "plugin.hpp"
using namespace std;

char dbgBuffer[256];

struct MegaSeq : Module {
    enum Stage {
        GATE_STAGE,
        STOPPED_STAGE
    };

    struct Engine {
        bool firstStep = true;
        rack::dsp::PulseGenerator triggerOuptutPulseGen;
        Stage stage;
        float stageProgress;
        float delayLight;
        float gateLight;
    };

    Engine *_engine{};

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

    float clockParams = 2.f;
    bool running = true;
    dsp::SchmittTrigger clockTrigger;
    dsp::SchmittTrigger runningTrigger;
    dsp::SchmittTrigger resetTrigger;
    dsp::SchmittTrigger gateTriggers[16];

    /** Phase of internal LFO */
    float phase = 0.f;
    float step = 0.f;
    float stageProgress = 0.0f;
    int index = 0;
    bool gates[16] = {};

    MegaSeq() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        for (int i = 0; i < 16; i++) {
            configParam(CVSTEP_PARAM + i, -10.f, 10.f, 0.f, "", " V");
        }
        configParam(FIRSTSTEP_PARAM, 0.f, 15.f, 0.f, "firstStep", "", 0.f, 1.f, 1.f);
        configParam(LASTSTEP_PARAM, 0.f, 15.f, 15.f, "lastStep", "", 0.f, 1.f, 1.f);
        configParam(GATELENGTH1_PARAM, 0.0f, 1.0f, 0.1, "Gate", " s", 0.f, pow(1.f, 2) * 10);

        onReset();
    }

    void onReset() override {
        for (int i = 0; i < 16; i++) {
            gates[i] = true;
        }
        stageProgress = 0.0f;
    }

    void setIndex(int index, int resetStep, int numSteps) {
        phase = 0.f;
        this->index = std::abs(index);

        if (this->index >= numSteps + 1) {
            this->index = resetStep;
        }
    }

    bool stepStage(float length) {
        float t = length;
        t = pow(t, 2);
        t *= 10.f;
        _engine->stageProgress += APP->engine->getSampleTime();
        return _engine->stageProgress > t;
    }

    void process(const ProcessArgs &args) override {
        //  run
        Engine &e = *_engine;
        // get value from params and inputs
        int firstStep = (int)clamp(roundf(params[FIRSTSTEP_PARAM].getValue() + (inputs[FIRSTSTEP_INPUT].getVoltage() * 1.5)), 0.f, 15.f);
        int lastStep = (int)clamp(roundf(params[LASTSTEP_PARAM].getValue() + (inputs[LASTSTEP_INPUT].getVoltage() * 1.5)), 0.f, 15.f);
        float gateLength = params[GATELENGTH1_PARAM].getValue();

        gates[index] = false;
        bool complete = false;

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
                    e.stage = GATE_STAGE;
                    e.stageProgress = 0.0f;
                    // gateIn = clockTrigger.isHigh();

                } else {
                    switch (e.stage) {
                        case STOPPED_STAGE:
                            break;

                        case GATE_STAGE:
                            if (!stepStage(gateLength)) {
                                complete = true;
                                if (clockTrigger.isHigh()) {
                                    e.stage = GATE_STAGE;
                                    stageProgress = 0.f;
                                } else {
                                    e.stage = STOPPED_STAGE;
                                }

                            } else {
                                gates[index] = true;
                            }
                            break;
                    }
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
            outputs[GATE1_OUTPUT + i].setVoltage((running && gates[index] && i == index && gates[i]) ? 10.f : 0.f);
            outputs[GATE2_OUTPUT + i].setVoltage((running && gates[index] && i == index && gates[i]) ? 10.f : 0.f);

            lights[CURRENTSTEP_LIGHT + i].setSmoothBrightness(0.0, args.sampleTime);
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

        outputs[GATE1_OUTPUT].setVoltage((gates[index]) ? 10.f : 0.f);
        outputs[GATE2_OUTPUT].setVoltage((gates[index]) ? 10.f : 0.f);
        outputs[SLEW_OUTPUT].setVoltage(stepStage(gateLength));
        lights[CURRENTSTEP_LIGHT + index].setSmoothBrightness(gates[index] * 10, args.sampleTime);
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

        // GATELENGTH1_PARAM
        addParam(createParamCentered<ltmSmallKnob>(mm2px(Vec(13.75, 75.)), module, MegaSeq::GATELENGTH1_PARAM));
        addParam(createParamCentered<ltmSmallSnapKnob>(mm2px(Vec(8.5, 60.)), module, MegaSeq::FIRSTSTEP_PARAM));
        addParam(createParamCentered<ltmSmallSnapKnob>(mm2px(Vec(19.0, 60.)), module, MegaSeq::LASTSTEP_PARAM));

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