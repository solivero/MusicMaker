/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <cmath>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public AudioAppComponent,
                               public Slider::Listener
{
public:
    //==============================================================================
    MainContentComponent() :
        currentSampleRate(44100),
        frequency(0),
        phase(0),
        level(0.0),
        tableSize(4096),
        sinewave(tableSize),
        sawtooth(tableSize),
        squarewave(tableSize)
    {
        setSize (800, 600);

        // specify the number of input and output channels that we want to open
        setAudioChannels (0, 1);

        addAndMakeVisible(levelSlider);
        levelSlider.setRange(0.0, 0.25);
        levelSlider.setValue(0.02);
        levelSlider.setTextValueSuffix(" not db");
        levelSlider.addListener(this);

        addAndMakeVisible(levelLabel);
        levelLabel.setText("Level", dontSendNotification);
        levelLabel.attachToComponent(&levelSlider, true);

        addAndMakeVisible(frequencySlider);
        frequencySlider.setRange(50.0, 5000.0);
        frequencySlider.setValue(440.0);
        frequencySlider.setSkewFactorFromMidPoint (500.0);
        frequencySlider.setTextValueSuffix(" hz");
        frequencySlider.addListener(this);

        addAndMakeVisible(frequencyLabel);
        frequencyLabel.setText("Frequency", dontSendNotification);
        frequencyLabel.attachToComponent(&frequencySlider, true);

        float delta = 2.0 / tableSize;
        for (int i = 0; i < tableSize; i++) {
            sinewave[i] = std::sin(i * 2.0 * double_Pi / tableSize);
            sawtooth[i] = -1.0 + delta*i;
            squarewave[i] = i > (tableSize / 2) ? 1.0 : -1.0;
        }
        currentTable = squarewave;
    }

    ~MainContentComponent()
    {
        shutdownAudio();
    }

    //==============================================================================

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        // This function will be called when the audio device is started, or when
        // its settings (i.e. sample rate, block size, etc) are changed.

        // You can use this function to initialise any resources you might need,
        // but be careful - it will be called on the audio thread, not the GUI thread.

        currentSampleRate = sampleRate;
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {

        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) {
            float* const buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
 
            for (int sample = 0; sample < bufferToFill.numSamples; ++sample) {
                phase = (phase + frequency) % currentSampleRate;
                double currentSample = ((double) phase * tableSize) / currentSampleRate;
                double fractpart, intpart;
                fractpart = std::modf(currentSample , &intpart);
                double interpolation = fractpart * (squarewave.at(((int) intpart + 1) % tableSize) - squarewave[intpart]);
                float output = squarewave[intpart] + interpolation; 
                buffer[sample] = output * level;
            }
        }
    }

    void releaseResources() override
    {
        // This will be called when the audio device stops, or when it is being
        // restarted due to a setting change.

        // For more details, see the help for AudioProcessor::releaseResources()
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        //g.fillAll (Colours::black);

        const int sliderLeft = 120;
        levelSlider.setBounds (sliderLeft, 50, getWidth() - sliderLeft - 10, 20);
        frequencySlider.setBounds (sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
    }

    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
    }

    void sliderValueChanged(Slider* slider) override
    {
        if (slider == &frequencySlider) {
            frequency = (int) frequencySlider.getValue();
        } else if (slider == &levelSlider) {
            level = (float) levelSlider.getValue();
        }

    }


private:
    Random random;
    Slider levelSlider;
    Label levelLabel;
    Slider frequencySlider;
    Label frequencyLabel;

    float level;
	int tableSize, phase, frequency, currentSampleRate; 

    typedef std::vector<float> wavetable;
    wavetable sinewave;
    wavetable sawtooth;
    wavetable squarewave;

    wavetable currentTable;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }


#endif  // MAINCOMPONENT_H_INCLUDED
