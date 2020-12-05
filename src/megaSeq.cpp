#include "plugin.hpp"

struct MegaSeq : Module
{
	enum ParamIds
	{
		ENUMS(CVSTEP_PARAM, 16),
		NUM_PARAMS
	};
	enum InputIds
	{
		FIRSTSTEPINPUT_INPUT,
		LASTSTEPINPUT_INPUT,
		STEPINPUT_INPUT,
		RESETINPUT_INPUT,
		PATTERNSELECTINPUT_INPUT,
		NUM_INPUTS
	};
	enum OutputIds
	{
		GATE1OUTPUT_OUTPUT,
		GATE2OUTPUT_OUTPUT,
		CVOUTPUT_OUTPUT,
		SLEWOUTPUT_OUTPUT,
		EOCOUTPUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds
	{
		ENUMS(CURRENTSTEP_LIGHT, 16),
		NUM_LIGHTS
	};

	MegaSeq()
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		for (int i = 0; i < 16; i++)
		{
			configParam(CVSTEP_PARAM + i, -10.f, 10.f, 0.f, "", " V");
		}
	}

	void process(const ProcessArgs &args) override
	{
	}
};

struct MegaSeqWidget : ModuleWidget
{
	MegaSeqWidget(MegaSeq *module)
	{
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/megaSeq.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		static const float knobPositionY[4] = {60.37, 78.135, 95.9, 113.664};
		static const float knobPositionX[4] = {36.76, 54.399, 72.038, 89.677};

		int inc = 0;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(knobPositionX[j], knobPositionY[i])), module, MegaSeq::CURRENTSTEP_LIGHT + inc));
				addParam(createParamCentered<ltmMediumKnob>(mm2px(Vec(knobPositionX[j], knobPositionY[i])), module, MegaSeq::CVSTEP_PARAM + inc));

				inc++;
			}
		}

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.402, 59.488)), module, MegaSeq::FIRSTSTEPINPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.686, 59.677)), module, MegaSeq::LASTSTEPINPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.26, 75.552)), module, MegaSeq::STEPINPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.449, 90.671)), module, MegaSeq::RESETINPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.449, 108.814)), module, MegaSeq::PATTERNSELECTINPUT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(112.479, 59.677)), module, MegaSeq::GATE1OUTPUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(112.857, 72.528)), module, MegaSeq::GATE2OUTPUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(112.668, 86.324)), module, MegaSeq::CVOUTPUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(113.046, 100.31)), module, MegaSeq::SLEWOUTPUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(113.235, 115.996)), module, MegaSeq::EOCOUTPUT_OUTPUT));
	}
};

Model *modelMegaSeq = createModel<MegaSeq, MegaSeqWidget>("megaSeq");