#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "AmbisonicRotation.h"
#include "PlayerThumbnail.h"

class StimulusPlayer    :   public Component,
                            public AudioSource,
                            public ChangeBroadcaster,
							private ChangeListener,
                            private Button::Listener,
							private Slider::Listener,
                            private Timer
{
public:

	enum TransportState
	{
		Stopping,
		Stopped,
		Starting,
		Playing,
		Pausing,
		Paused
	};

	StringArray TransportStateString
	{
		"Stopping",
		"Stopped",
		"Starting",
		"Playing",
		"Pausing",
		"Paused"
	};

    StimulusPlayer();
    ~StimulusPlayer();
    
    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    void paint (Graphics&) override;
    void resized() override;

	void changeListenerCallback(ChangeBroadcaster* source) override;
	void changeState(TransportState newState);
	void buttonClicked(Button* buttonThatWasClicked) override;
	void sliderValueChanged(Slider* slider) override;
	void timerCallback() override;

	// exposing some playback transport functionality
	void play();
	void pause();
	void stop();
	int getNumberOfChannels();
	void setGain(float gainInDB);
	void loop(bool looping);
	bool getLoopingState();
	bool checkPlaybackStatus();
	bool checkLoopStatus();
	double getPlaybackHeadPosition();
	void setPlaybackHeadPosition(double time);
	void setPlaybackOffsets(double beg, double end);
	double getPlaybackStartOffset();
	double getPlaybackEndOffset();

	void loadFileIntoTransport(String fullPath);
	void unloadFileFromTransport();

	void cacheAudioFile(String filepath);
	void clearAudioFileCache();


	void setShowTest(bool shouldShow);

	// log window message
    String currentMessage;

	bool cachingLock;

private:
	TransportState state;

	TimeSliceThread readAheadThread;
	// Cache arrays
	OwnedArray<AudioFormatReaderSource> audioFileSourceArray;
	OwnedArray<AudioTransportSource> transportSourceArray;
	Array<String> fileNameArray;
	Array<int> numChArray;

	int m_samplesPerBlockExpected;
	double m_sampleRate;
	
	AudioFormatManager formatManager;
	File currentlyLoadedFile;

	AmbisonicRotation ar;

	bool loopingEnabled = false;

	double begOffsetTime = 0.0f, endOffsetTime = 0.0f;

	int loadedFileChannelCount = 0;
	int currentTSIndex;

	// METHODS
	void browseForFile();
	void sendMsgToLogWindow(String message);
	String returnHHMMSS(double lengthInSeconds);

	TextButton openButton, playButton, stopButton, loopButton;
	Label loadedFileName, playbackHeadPosition;

	Slider yawSlider, pitchSlider, rollSlider, gainSlider;
	Label yawSliderLabel, pitchSliderLabel, rollSliderLabel, gainSliderLabel;
	Slider transportSlider;

	PlayerThumbnail pt;

	bool m_shouldShowTest;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StimulusPlayer)
};
