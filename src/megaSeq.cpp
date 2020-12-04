#include "plugin.hpp"


struct MegaSeq : Module {
	enum ParamIds {
		CVPARAM1_PARAM,
		CVPARAM2_PARAM,
		CVPARAM3_PARAM,
		CVPARAM4_PARAM,
		CVPARAM5_PARAM,
		CVPARAM6_PARAM,
		CVPARAM7_PARAM,
		CVPARAM8_PARAM,
		CVPARAM9_PARAM,
		CVPARAM10_PARAM,
		CVPARAM11_PARAM,
		CVPARAM12_PARAM,
		CVPARAM13_PARAM,
		CVPARAM14_PARAM,
		CVPARAM15_PARAM,
		CVPARAM16_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		FIRSTSTEPINPUT_INPUT,
		LASTSTEPINPUT_INPUT,
		STEPINPUT_INPUT,
		RESETINPUT_INPUT,
		PATTERNSELECTINPUT_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		GATE1OUTPUT_OUTPUT,
		GATE2OUTPUT_OUTPUT,
		CVOUTPUT_OUTPUT,
		SLEWOUTPUT_OUTPUT,
		EOCOUTPUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		SEQLIGHT1_LIGHT,
		SEQLIGHT2_LIGHT,
		SEQLIGHT3_LIGHT,
		SEQLIGHT4_LIGHT,
		SEQLIGHT5_LIGHT,
		SEQLIGHT6_LIGHT,
		SEQLIGHT7_LIGHT,
		SEQLIGHT8_LIGHT,
		SEQLIGHT9_LIGHT,
		SEQLIGHT10_LIGHT,
		SEQLIGHT11_LIGHT,
		SEQLIGHT12_LIGHT,
		SEQLIGHT13_LIGHT,
		SEQLIGHT14_LIGHT,
		SEQLIGHT15_LIGHT,
		SEQLIGHT16_LIGHT,
		NUM_LIGHTS
	};

	MegaSeq() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CVPARAM1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM4_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM5_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM6_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM7_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM8_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM9_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM10_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM11_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM12_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM13_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM14_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM15_PARAM, 0.f, 1.f, 0.f, "");
		configParam(CVPARAM16_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct MegaSeqWidget : ModuleWidget {
	MegaSeqWidget(MegaSeq* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/megaSeq.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(36.76, 60.37)), module, MegaSeq::SEQLIGHT1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(54.399, 60.37)), module, MegaSeq::SEQLIGHT2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.038, 60.37)), module, MegaSeq::SEQLIGHT3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(89.677, 60.37)), module, MegaSeq::SEQLIGHT4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(36.76, 78.135)), module, MegaSeq::SEQLIGHT5_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(54.399, 78.135)), module, MegaSeq::SEQLIGHT6_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.038, 78.135)), module, MegaSeq::SEQLIGHT7_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(89.677, 78.135)), module, MegaSeq::SEQLIGHT8_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(36.76, 95.9)), module, MegaSeq::SEQLIGHT9_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(54.399, 95.9)), module, MegaSeq::SEQLIGHT10_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.038, 95.9)), module, MegaSeq::SEQLIGHT11_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(89.677, 95.9)), module, MegaSeq::SEQLIGHT12_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(36.76, 113.664)), module, MegaSeq::SEQLIGHT13_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(54.399, 113.664)), module, MegaSeq::SEQLIGHT14_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.038, 113.664)), module, MegaSeq::SEQLIGHT15_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(89.677, 113.664)), module, MegaSeq::SEQLIGHT16_LIGHT));

		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(36.76, 60.37)), module, MegaSeq::CVPARAM1_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(54.399, 60.37)), module, MegaSeq::CVPARAM2_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(72.038, 60.37)), module, MegaSeq::CVPARAM3_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(89.677, 60.37)), module, MegaSeq::CVPARAM4_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(36.76, 78.135)), module, MegaSeq::CVPARAM5_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(54.399, 78.135)), module, MegaSeq::CVPARAM6_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(72.038, 78.135)), module, MegaSeq::CVPARAM7_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(89.677, 78.135)), module, MegaSeq::CVPARAM8_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(36.76, 95.9)), module, MegaSeq::CVPARAM9_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(54.399, 95.9)), module, MegaSeq::CVPARAM10_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(72.038, 95.9)), module, MegaSeq::CVPARAM11_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(89.677, 95.9)), module, MegaSeq::CVPARAM12_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(36.76, 113.664)), module, MegaSeq::CVPARAM13_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(54.399, 113.664)), module, MegaSeq::CVPARAM14_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(72.038, 113.664)), module, MegaSeq::CVPARAM15_PARAM));
		addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(89.677, 113.664)), module, MegaSeq::CVPARAM16_PARAM));

		addInput(createInputCentered<ltmOutput>(mm2px(Vec(7.402, 59.488)), module, MegaSeq::FIRSTSTEPINPUT_INPUT));
		addInput(createInputCentered<ltmOutput>(mm2px(Vec(19.686, 59.677)), module, MegaSeq::LASTSTEPINPUT_INPUT));
		addInput(createInputCentered<ltmOutput>(mm2px(Vec(13.26, 75.552)), module, MegaSeq::STEPINPUT_INPUT));
		addInput(createInputCentered<ltmOutput>(mm2px(Vec(13.449, 90.671)), module, MegaSeq::RESETINPUT_INPUT));
		addInput(createInputCentered<ltmOutput>(mm2px(Vec(13.449, 108.814)), module, MegaSeq::PATTERNSELECTINPUT_INPUT));

		addOutput(createOutputCentered<ltmOutput>(mm2px(Vec(112.479, 59.677)), module, MegaSeq::GATE1OUTPUT_OUTPUT));
		addOutput(createOutputCentered<ltmOutput>(mm2px(Vec(112.857, 72.528)), module, MegaSeq::GATE2OUTPUT_OUTPUT));
		addOutput(createOutputCentered<ltmOutput>(mm2px(Vec(112.668, 86.324)), module, MegaSeq::CVOUTPUT_OUTPUT));
		addOutput(createOutputCentered<ltmOutput>(mm2px(Vec(113.046, 100.31)), module, MegaSeq::SLEWOUTPUT_OUTPUT));
		addOutput(createOutputCentered<ltmOutput>(mm2px(Vec(113.235, 115.996)), module, MegaSeq::EOCOUTPUT_OUTPUT));

		
	}
};


Model* modelMegaSeq = createModel<MegaSeq, MegaSeqWidget>("megaSeq");