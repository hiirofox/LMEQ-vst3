/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
LmeqAudioProcessor::LmeqAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{

	LMEQInit(&eq, MaxEQNodeNum);
	linearEQ.init(&eq);
	startTimerHz(30);//用来更新linearEQ的
}

void LmeqAudioProcessor::timerCallback()
{
	//linearEQ.updata();
}

juce::AudioProcessorValueTreeState::ParameterLayout LmeqAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;
	layout.add(std::make_unique<juce::AudioParameterFloat>("gain", "gain", -20, 20, 0));
	//layout.add(std::make_unique<juce::AudioParameterFloat>("reso", "reso", 0, 50, 0.707));
	//layout.add(std::make_unique<juce::AudioParameterFloat>("gain", "gain", -4, 4, 0.0));//增益，对数
	return layout;
}

LmeqAudioProcessor::~LmeqAudioProcessor()
{
}

//==============================================================================
const juce::String LmeqAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool LmeqAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool LmeqAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool LmeqAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double LmeqAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int LmeqAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int LmeqAudioProcessor::getCurrentProgram()
{
	return 0;
}

void LmeqAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String LmeqAudioProcessor::getProgramName(int index)
{
	return "IIRPitchShifter";
}

void LmeqAudioProcessor::changeProgramName(int index, const juce::String& newName)
{

}

//==============================================================================
void LmeqAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
}

void LmeqAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LmeqAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif


void LmeqAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

	const int numSamples = buffer.getNumSamples();
	float* wavbufl = buffer.getWritePointer(0);
	float* wavbufr = buffer.getWritePointer(1);
	const float* recbufl = buffer.getReadPointer(0);
	const float* recbufr = buffer.getReadPointer(1);

	float SampleRate = getSampleRate();

	float gain = *Params.getRawParameterValue("gain");
	gain = expf(gain * 0.23);//0.23是:exp(kx)，其中k为dB应用于exp的系数

	if (isEnableLinear == 0)
	{
		for (int i = 0; i < numSamples; ++i)
		{
			StereoSignal vin = StereoSignal{ recbufl[i],recbufr[i] };
			//StereoSignal vin = StereoSignal{ (float)(rand() % 10000) / 50000.0f * (rand() % 2 ? 1 : -1),(float)(rand() % 10000) / 50000.0f * (rand() % 2 ? 1 : -1) };
			StereoSignal vout = LMEQProcessStereo(&eq, vin);
			wavbufl[i] = vout.l * gain;
			wavbufr[i] = vout.r * gain;
		}
	}
	else
	{
		linearEQ.proc(recbufl, recbufr, wavbufl, wavbufr, numSamples);
	}


	for (int i = 0; fftPos < MaxFFTLength && i < numSamples; ++fftPos, ++i)//往fft里面填充数据
	{
		fftData[fftPos] = (wavbufl[i] + wavbufr[i]) * 0.5;
	}

	procLoopCount++;
	if (procLoopCount >= 8)//每执行8次proc更新一次fir
	{
		procLoopCount = 0;
		if (isEnableLinear)
		{
			linearEQ.updata();
		}
	}
}

//==============================================================================
bool LmeqAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LmeqAudioProcessor::createEditor()
{
	return new LmeqAudioProcessorEditor(*this);

	//return new juce::GenericAudioProcessorEditor(*this);//自动绘制(调试)
}

//==============================================================================

void LmeqAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.

	// 创建一个 XML 节点
	juce::XmlElement xml("LMEQ_Settings");

	juce::MemoryBlock eqDataBlock;
	eqDataBlock.append(&eq, sizeof(LMEQ));
	juce::String base64Data = eqDataBlock.toBase64Encoding();
	xml.setAttribute("EQ_Data", base64Data);//添加eq数据

	auto state = Params.copyState();
	xml.setAttribute("Knob_Data", state.toXmlString());//添加旋钮数据

	juce::String xmlString = xml.toString();
	destData.append(xmlString.toRawUTF8(), xmlString.getNumBytesAsUTF8());
}

void LmeqAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	  // 将 data 转换为字符串以解析 XML
	juce::String xmlString(static_cast<const char*>(data), sizeInBytes);

	// 解析 XML
	std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(xmlString));
	if (xml == nullptr || !xml->hasTagName("LMEQ_Settings"))
	{
		DBG("Error: Unable to load XML settings");
		return;
	}
	juce::String base64Data = xml->getStringAttribute("EQ_Data");
	juce::MemoryBlock eqDataBlock;
	eqDataBlock.fromBase64Encoding(base64Data);
	std::memcpy(&eq, eqDataBlock.getData(), sizeof(LMEQ));

	auto KnobDataXML = xml->getStringAttribute("Knob_Data");
	Params.replaceState(juce::ValueTree::fromXml(KnobDataXML));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new LmeqAudioProcessor();
}
