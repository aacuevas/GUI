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

class BScanChannelDisplay;
class BScanDisplay;

class BScanDisplayCanvas : public Visualizer
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
	void setParameter(int, float);
	void setParameter(int, int, int, float) {}

private:
	ScopedPointer<BScanDisplay> display;
	int frameSize();

	AudioSampleBuffer *displayBuffer;
	ScopedPointer<BScanScreenBuffer> screenBuffer;

	BScanDisplayNode *processor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BScanDisplayCanvas);
};

class BScanDisplay : public Component
{
public:

	BScanDisplay();
	~BScanDisplay();

private:

};

class BScanChannelDisplay : public Component
{
public:
	BScanChannelDisplay();
	~BScanChannelDisplay();

	void paint(Graphics &g);


};

class BScanScreenBuffer
{
public:
	BScanScreenBuffer(int nChans, int xSize, int ySize);
	~BScanScreenBuffer();

	float* getPointer(int chan, int frame, int pos) const;
	float getPoint(int chan, int frame, int pos) const;
	int getChannelIndex(int chan) const;

	void addFromAudioSampleBuffer(AudioSampleBuffer& buffer, int channel, int startSample, int nFrames, int frameSize);
	void resize(int nChans, int xSize, int ySize);
	void clear();

private:

	HeapBlock<float> allocatedData;
	Array<int> indicesArray;

	int sC, sX, sY;
	int channelSize;

}

#endif  // __BSCANDISPLAYCANVAS_H_367A3CBA__
