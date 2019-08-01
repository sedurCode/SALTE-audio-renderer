#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioSetup.h"

//==============================================================================
AudioSetup::AudioSetup()
	: audioSetupComp(deviceManager,
		0,     // minimum input channels
		256,   // maximum input channels
		0,     // minimum output channels
		256,   // maximum output channels
		false, // ability to select midi inputs
		false, // ability to select midi output device
		false, // treat channels as stereo pairs
		false) // hide advanced options
{
	addAndMakeVisible(audioSetupComp);
	addAndMakeVisible(diagnosticsBox);

	diagnosticsBox.setMultiLine(true);
	diagnosticsBox.setReturnKeyStartsNewLine(true);
	diagnosticsBox.setReadOnly(true);
	diagnosticsBox.setScrollbarsShown(true);
	diagnosticsBox.setCaretVisible(false);
	diagnosticsBox.setPopupMenuEnabled(true);
	diagnosticsBox.setColour(TextEditor::backgroundColourId, Colour(0x32ffffff));
	diagnosticsBox.setColour(TextEditor::outlineColourId, Colour(0x1c000000));
	diagnosticsBox.setColour(TextEditor::shadowColourId, Colour(0x16000000));

	cpuUsageLabel.setText("CPU Usage", dontSendNotification);
	cpuUsageText.setJustificationType(Justification::right);
	addAndMakeVisible(&cpuUsageLabel);
	addAndMakeVisible(&cpuUsageText);

	setSize(760, 360);

	//setAudioChannels(2, 2);
	deviceManager.addChangeListener(this);

	startTimer(50);
}

AudioSetup::~AudioSetup()
{
	deviceManager.removeChangeListener(this);

}

void AudioSetup::paint (Graphics& g)
{
	g.setColour(Colours::grey);
	g.fillRect(getLocalBounds().removeFromRight(proportionOfWidth(0.4f)));
}

void AudioSetup::resized()
{
	auto rect = getLocalBounds();

	audioSetupComp.setBounds(rect.removeFromLeft(proportionOfWidth(0.6f)));
	rect.reduce(10, 10);

	auto topLine(rect.removeFromTop(20));
	cpuUsageLabel.setBounds(topLine.removeFromLeft(topLine.getWidth() / 2));
	cpuUsageText.setBounds(topLine);
	rect.removeFromTop(20);

	diagnosticsBox.setBounds(rect);

}

void AudioSetup::changeListenerCallback(ChangeBroadcaster*)
{
	dumpDeviceInfo();
}

String AudioSetup::getListOfActiveBits(const BigInteger& b)
{
	StringArray bits;

	for (auto i = 0; i <= b.getHighestBit(); ++i)
		if (b[i])
			bits.add(String(i));

	return bits.joinIntoString(", ");
}

void AudioSetup::timerCallback()
{
	auto cpu = deviceManager.getCpuUsage() * 100;
	cpuUsageText.setText(String(cpu, 6) + " %", dontSendNotification);
}

void AudioSetup::dumpDeviceInfo()
{
	logMessage("--------------------------------------");
	logMessage("Current audio device type: " + (deviceManager.getCurrentDeviceTypeObject() != nullptr
		? deviceManager.getCurrentDeviceTypeObject()->getTypeName()
		: "<none>"));

	if (auto * device = deviceManager.getCurrentAudioDevice())
	{
		logMessage("Current audio device: " + device->getName().quoted());
		logMessage("Sample rate: " + String(device->getCurrentSampleRate()) + " Hz");
		logMessage("Block size: " + String(device->getCurrentBufferSizeSamples()) + " samples");
		logMessage("Bit depth: " + String(device->getCurrentBitDepth()));
		logMessage("Input channel names: " + device->getInputChannelNames().joinIntoString(", "));
		logMessage("Active input channels: " + getListOfActiveBits(device->getActiveInputChannels()));
		logMessage("Output channel names: " + device->getOutputChannelNames().joinIntoString(", "));
		logMessage("Active output channels: " + getListOfActiveBits(device->getActiveOutputChannels()));
	}
	else
	{
		logMessage("No audio device open");
	}
}

void AudioSetup::logMessage(const String& m)
{
	diagnosticsBox.moveCaretToEnd();
	diagnosticsBox.insertTextAtCaret(m + newLine);
}
