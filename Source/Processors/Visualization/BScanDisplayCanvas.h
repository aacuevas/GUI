/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2013 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __BSCANDISPLAYCANVAS_H_367A3CBA__
#define __BSCANDISPLAYCANVAS_H_367A3CBA__

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../BScanDisplayNode.h"
#include "Visualizer.h"

class BScanDisplayNode;

class BScanChannelDisplay;
class BScanDisplay;
class BScanScreenBuffer;

class BScanDisplayCanvas : public Visualizer,
	public ComboBox::Listener
{
public:

	BScanDisplayCanvas(BScanDisplayNode* n);
	~BScanDisplayCanvas();

	void paint(Graphics &g);

	void refreshState();
	void update();
	void refresh();
	void beginAnimation();
	void endAnimation();
	void setParameter(int, float) {};
	void setParameter(int, int, int, float) {}

	void resized();
	void comboBoxChanged(ComboBox* cb);

	BScanScreenBuffer* getScreenBuffer();

private:

	void updateScreenBuffer();
	void resizeScreenBuffer();

	ScopedPointer<BScanDisplay> bScanDisplay;
	ScopedPointer<Viewport> viewport;

	AudioSampleBuffer *displayBuffer;
	ScopedPointer<BScanScreenBuffer> screenBuffer;

	BScanDisplayNode *processor;

	ScopedPointer<ComboBox> heightSelector;
	ScopedPointer<ComboBox> columnsSelector;

	StringArray heights;
	StringArray columns;

	int nChans;
	int nColumns, channelHeight, channelWidth;
	int scrollBarThickness;

	int lastIndex;

	bool running;

	bool refillBuffer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BScanDisplayCanvas);
};

class BScanDisplay : public Component
{
public:

	BScanDisplay(BScanDisplayCanvas*, Viewport*);
	~BScanDisplay();
	void refresh();
	void resized();

	void setNumChannels(int n);
	void updateSettings(int chanHeight, int nCols);
	int getChannelWidth();

	void paint(Graphics &g);

private:

	void updateChannelWidth();

	int channelHeight, channelWidth;
	int nColumns;
	int nChans;

	BScanDisplayCanvas *canvas;
	Viewport *viewport;

	Array<BScanChannelDisplay*> channelArray;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BScanDisplay);

};

class BScanChannelDisplay : public Component
{
public:
	BScanChannelDisplay(BScanDisplayCanvas*, BScanDisplay*, int);
	~BScanChannelDisplay();

	void paint(Graphics &g);

private:

	BScanDisplayCanvas* canvas;
	BScanDisplay* display;

	int chan;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BScanChannelDisplay);

};

class BScanScreenBuffer
{
public:
	BScanScreenBuffer();
	BScanScreenBuffer(int nChans, int xSize, int ySize);
	~BScanScreenBuffer();

	Image* getPointer(int chan) const;
	int getChannelIndex(int chan) const;

	void addFromAudioSampleBuffer(AudioSampleBuffer *buffer, int channel, int startSample, int nFrames, int frameSize);
	void resize(int nChans, int xSize, int ySize);
	void clear();

private:
	Colour colorFromNormalizedPower(float pow);

	OwnedArray<Image> buffer;
	Array<int> indicesArray;

	int sC, sX, sY;
	int channelSize;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BScanScreenBuffer);

};

#endif  // __BSCANDISPLAYCANVAS_H_367A3CBA__
