#include <stdio.h>

#include "plugin.hpp"

using namespace std;

char dbgBuffer[256];
int _channels = 0;

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

struct MegaSeq : Module {
    enum Stage {
        GATE_STAGE,
        STOPPED_STAGE
    };

    // define an agine and initialise it
    struct Engine {
        MegaSeq::Stage stage;
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

        void reset() {
            trigger.reset();
            triggerOuptutPulseGen.process(10.0);
            stage = STOPPED_STAGE;
            stageProgress = 0.0;
            delayLight = 0.0;
            gateLight = 0.0;
            index = 0;
            firstStep = 0;
            lastStep = 0;

            phase = 0.f;
            step = 0.f;
            for (int i = 0; i < 16; i++) {
                gates1[i] = true;
                gates2[i] = true;
            }
        };
    } ;

    Engine * _engines[16];
    float phase;
    bool running = true;
    int genIndex = 0;
    

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
        _channels = channels();
        reset();
        
    }
    /* data */

    void reset() {
        for (int c = 0; c < _channels; ++c) {
            _engines[c]->reset();
        }
    }

    int channels() {
        return inputs[STEP_INPUT].getChannels();
    }

    void addChannel(int c) {
        _engines[c] = new Engine();
        _engines[c]->reset();
    }

    void removeChannel(int c) {
        delete _engines[c];
        _engines[c] = NULL;
    }
    void setIndex(int c, int index, int resetStep, int numSteps) {
        phase = 0.f;
        index = std::abs(index);

        if (index >= numSteps + 1) {
            _engines[c]->index = resetStep;
        }
    }

    bool stepStage(int c, Param &knob) {
        float t = knob.value;
        t = pow(t, 2);
        t *= 10.f;
        _engines[c]->stageProgress += APP->engine->getSampleTime();
        return _engines[c]->stageProgress > t;
    }

    void process(const ProcessArgs &args) override {
        //  run
        _channels = inputs[STEP_INPUT].getChannels();
        for (int c = 0; c < _channels; c++) {
            outputs[EOC_OUTPUT].setVoltage(params[GATELENGTH1_PARAM].getValue());
            Engine &e = *this->_engines[c];

            // if (c == 0) {
            //     this->genIndex = e.index;
            // }

            // Param gateLengths[2];
            // get value from params and inputs

            // je les passe en poly ?
            e.firstStep = (int)clamp(roundf(params[FIRSTSTEP_PARAM].getValue() + (inputs[FIRSTSTEP_INPUT].getVoltage(c) * 1.5)), 0.f, 15.f);
            e.lastStep = (int)clamp(roundf(params[LASTSTEP_PARAM].getValue() + (inputs[LASTSTEP_INPUT].getVoltage(c) * 1.5)), 0.f, 15.f);
            Param gateLength = params[GATELENGTH1_PARAM];
            // gateLengths[2] = {params[GATELENGTH1_PARAM],params[GATELENGTH2_PARAM] };

            e.gates1[e.index] = false;
            e.gates2[e.index] = false;

            // bool complete = false;

            if (running) {
                // si le step param est connecté
                if (inputs[STEP_INPUT].isConnected()) {
                    if (e.trigger.process(inputs[STEP_INPUT].getPolyVoltage(c))) {
                        cout << "gate value for 1r st channel" << e.gates1[0] << endl;
                        if (e.firstStep >= e.lastStep) {
                            if (e.index < e.lastStep + 1) {
                                e.index = e.firstStep + 1;
                            }
                            setIndex(c, e.index - 1, e.lastStep, e.firstStep);
                        } else {
                            setIndex(c, e.index + 1, e.firstStep, e.lastStep);
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
                                        // complete = true;
                                        if (e.trigger.isHigh()) {
                                            e.stage = GATE_STAGE;
                                            e.stageProgress = 0.f;
                                        } else {
                                            e.stage = STOPPED_STAGE;
                                        }

                                    } else {
                                        e.gates1[e.index] = true;
                                        e.gates2[e.index] = true;
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

            outputs[GATE1_OUTPUT].setChannels(_channels);
            outputs[GATE2_OUTPUT].setChannels(_channels);
            outputs[CV_OUTPUT].setChannels(_channels);

            // select Param
            for (int i = 0; i < 16; i++) {
                if (gateTriggers[i].process(params[CVSTEP_PARAM + i].getValue())) {
                    // affiche les parametres du step
                }
                outputs[GATE1_OUTPUT].setVoltage((running && e.gates1[e.index] && i == e.index && e.gates1[i]) ? 10.f : 0.f, c);
                outputs[GATE2_OUTPUT].setVoltage((running && e.gates2[e.index] && i == e.index && e.gates2[i]) ? 10.f : 0.f, c);

                lights[CURRENTSTEP_LIGHT + i].setSmoothBrightness(0.0, args.sampleTime);
            }

            //  reset
            if (resetTrigger.process(inputs[RESET_INPUT].getVoltage(c))) {
                if (e.firstStep >= e.lastStep) {
                    setIndex(c, e.lastStep, e.lastStep, e.firstStep);
                } else {
                    setIndex(c, e.firstStep, e.firstStep, e.lastStep);
                }
            }

            // outputs
            outputs[CV_OUTPUT].setVoltage(params[CVSTEP_PARAM + e.index].getValue(), c);

            outputs[GATE1_OUTPUT].setVoltage((e.gates1[e.index]) ? 10.f : 0.f, c);
            outputs[GATE2_OUTPUT].setVoltage((e.gates1[e.index]) ? 10.f : 0.f, c);
            // outputs[SLEW_OUTPUT].setVoltage(stepStage(c, gateLength), c);
            lights[CURRENTSTEP_LIGHT + genIndex].setSmoothBrightness((genIndex == e.index && c == 0) ? e.gates1[e.index] * 10 : 0.f, args.sampleTime);
        }
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