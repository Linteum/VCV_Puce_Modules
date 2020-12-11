#include "megaSeq.hpp"

#include <stdio.h>

#include <cmath>
#include <iostream>

#include "plugin.cpp"

using namespace std;

char dbgBuffer[256];
int _channels;

void MegaSeq::Engine::reset() {
    triggerOuptutPulseGen.process(10.0);
    stage = STOPPED_STAGE;
    stageProgress = 0.0;
    delayLight = 0.0;
    gateLight = 0.0;
    index = 0;

    phase = 0.f;
    step = 0.f;
    for (int i = 0; i < 16; i++) {
        gates[0][i] = true;
        gates[1][i] = true;
    }
}

void MegaSeq::onReset() {
    for (int c = 0; c < _channels; ++c) {
        _engines[c]->reset();
    }
}

void MegaSeq::setIndex(int c, int index, int resetStep, int numSteps) {
    phase = 0.f;
    index = std::abs(index);

    if (index >= numSteps + 1) {
        _engines[c]->index = resetStep;
    }
}

bool MegaSeq::stepStage(int c, Param &knob) {
    float t = knob.value;
    t = pow(t, 2);
    t *= 10.f;
    _engines[c]->stageProgress += APP->engine->getSampleTime();
    return _engines[c]->stageProgress > t;
}

void MegaSeq::processChannel(const ProcessArgs &args, int c) {
    //  run
    Engine &e = *_engines[c];
    // Param gateLengths[2];
    // get value from params and inputs

    // je les passe en poly ?
    int firstStep = (int)clamp(roundf(params[FIRSTSTEP_PARAM].getValue() + (inputs[FIRSTSTEP_INPUT].getVoltage() * 1.5)), 0.f, 15.f);
    int lastStep = (int)clamp(roundf(params[LASTSTEP_PARAM].getValue() + (inputs[LASTSTEP_INPUT].getVoltage() * 1.5)), 0.f, 15.f);
    Param gateLength = params[GATELENGTH1_PARAM];
    // gateLengths[2] = {params[GATELENGTH1_PARAM],params[GATELENGTH2_PARAM] };

    e.gates[0][e.index] = false;
    e.gates[1][e.index] = false;

    bool complete = false;

    if (running) {
        // si le step param est connecté
        if (inputs[STEP_INPUT].isConnected()) {
            if (clockTrigger.process(inputs[STEP_INPUT].getPolyVoltage(c))) {
                if (firstStep >= lastStep) {
                    if (e.index < lastStep + 1) {
                        e.index = firstStep + 1;
                    }
                    setIndex(c, e.index - 1, lastStep, firstStep);
                } else {
                    setIndex(c, e.index + 1, firstStep, lastStep);
                }
                e.stage = GATE_STAGE;
                e.stageProgress = 0.0f;
                // gateIn = clockTrigger.isHigh();

            } else {
                switch (e.stage) {
                    case STOPPED_STAGE:
                        break;

                    case GATE_STAGE:
                        for (int i = 0; i < 2; i++) {
                            if (stepStage(c, gateLength)) {
                                complete = true;
                                if (clockTrigger.isHigh()) {
                                    e.stage = GATE_STAGE;
                                    e.stageProgress = 0.f;
                                } else {
                                    e.stage = STOPPED_STAGE;
                                }

                            } else {
                                e.gates[0][e.index] = true;
                                e.gates[1][e.index] = true;
                            }
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
        outputs[GATE1_OUTPUT + i].setVoltage((running && e.gates[e.index] && i == e.index && e.gates[i]) ? 10.f : 0.f);
        outputs[GATE2_OUTPUT + i].setVoltage((running && e.gates[e.index] && i == e.index && e.gates[i]) ? 10.f : 0.f);

        lights[CURRENTSTEP_LIGHT + i].setSmoothBrightness(0.0, args.sampleTime);
    }

    //  reset
    if (resetTrigger.process(inputs[RESET_INPUT].getVoltage())) {
        if (firstStep >= lastStep) {
            setIndex(c, lastStep, lastStep, firstStep);
        } else {
            setIndex(c, firstStep, firstStep, lastStep);
        }
    }

    // outputs
    outputs[CV_OUTPUT].setVoltage(params[CVSTEP_PARAM + e.index].getValue());

    outputs[GATE1_OUTPUT].setVoltage((e.gates[0][e.index]) ? 10.f : 0.f, c);
    outputs[GATE2_OUTPUT].setVoltage((e.gates[1][e.index]) ? 10.f : 0.f, c);
    outputs[SLEW_OUTPUT].setVoltage(stepStage(c, gateLength), c);
    lights[CURRENTSTEP_LIGHT + e.index].setSmoothBrightness(e.gates[1][e.index] * 10, args.sampleTime);
}

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