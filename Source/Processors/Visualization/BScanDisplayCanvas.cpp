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

#include "BScanDisplayCanvas.h"

BScanDisplayCanvas::BScanDisplayCanvas(BScanDisplayNode* n)
	: processor(n), lastIndex(0), refillBuffer(false), nColumns(2),
	channelHeight(100)
{

	displayBuffer = processor->getDisplayBufferAddress();
	nChans = processor->getNumInputChannels();
	
	viewport = new Viewport();
	bScanDisplay = new BScanDisplay(this, viewport);

	viewport->setViewedComponent(bScanDisplay);
	viewport->setScrollBarsShown(true,false);

	scrollBarThickness = viewport->getScrollBarThickness();

	addAndMakeVisible(viewport);

	screenBuffer = new BScanScreenBuffer();

	heights.add("50");
	heights.add("75");
	heights.add("100");
	heights.add("125");
	heights.add("150");

	columns.add("1");
	columns.add("2");
	columns.add("3");

	heightSelector = new ComboBox("Height");
	heightSelector->addItemList(heights,1);
	heightSelector->setSelectedId(3,false);
	heightSelector->addListener(this);
	addAndMakeVisible(heightSelector);

	columnsSelector = new ComboBox("Columns");
	columnsSelector->addItemList(columns,1);
	columnsSelector->setSelectedId(2,false);
	columnsSelector->addListener(this);
	addAndMakeVisible(columnsSelector);
}

BScanDisplayCanvas::~BScanDisplayCanvas() {}

void BScanDisplayCanvas::resized()
{
	int nRows = ceil(nChans/nColumns);

	viewport->setBounds(0,0,getWidth(),getHeight()-30);
	bScanDisplay->setBounds(0,0,getWidth()-scrollBarThickness, channelHeight*nRows+50*(nRows-1));
	
	heightSelector->setBounds(5,getHeight()-30,100,25);
	columnsSelector->setBounds(175,getHeight()-30,100,25);

	channelWidth = floor((bScanDisplay->getWidth() - 50*(nRows-1))/nRows);

	resizeScreenBuffer();
}

void BScanDisplayCanvas::beginAnimation()
{
	lastIndex=0;
	screenBuffer->clear();

	running=true;
	startCallbacks();
}

void BScanDisplayCanvas::endAnimation()
{
	stopCallbacks();
	running=false;
}

void BScanDisplayCanvas::update()
{
	nChans = jmax(processor->getNumInputs(),1);

	resized();

}

void BScanDisplayCanvas::resizeScreenBuffer()
{
	screenBuffer->resize(nChans,channelWidth,channelHeight);
	if (running)
	{
		refillBuffer = true;
	}
}

void BScanDisplayCanvas::updateScreenBuffer()
{
	int to, from;

	int currentIndex = processor->getFrameIndex();
	int frameSize = processor->getFrameSize();

	int maxFrames = displayBuffer->getNumSamples() / frameSize;

	if (refillBuffer)
	{
		from = currentIndex - channelWidth;
		if (from < 0)
		{
			from = maxFrames+from;
		}

		to = currentIndex;
	}
	else
	{
		from = lastIndex;
		to = currentIndex;
	}

	if (from < to)
	{

		for (int channel=0; channel < nChans; channel++)
		{
			screenBuffer->addFromAudioSampleBuffer(displayBuffer,channel,from,to,frameSize);
		}
	}
	else
	{
		for (int channel=0; channel < nChans; channel++)
		{
			screenBuffer->addFromAudioSampleBuffer(displayBuffer,channel,to,maxFrames,frameSize);
			screenBuffer->addFromAudioSampleBuffer(displayBuffer,channel,0,from,frameSize);
		}
	}

	lastIndex = currentIndex;
	
}

BScanScreenBuffer* BScanDisplayCanvas::getScreenBuffer()
{
	return screenBuffer;
}

void BScanDisplayCanvas::refreshState()
{
	//refillBuffer = true;
}

void BScanDisplayCanvas::paint(Graphics& g)
{
	g.fillAll(Colours::darkgrey);
}

void BScanDisplayCanvas::refresh()
{
	updateScreenBuffer();
	bScanDisplay->refresh();
}

void BScanDisplayCanvas::comboBoxChanged(ComboBox* cb)
{
	if (cb == heightSelector)
	{
		channelHeight = heights[cb->getSelectedId()-1].getIntValue();
		resized();
	}
	else if (cb == columnsSelector)
	{
		nColumns = columns[cb->getSelectedId()-1].getIntValue();
		resized();
	}
}


BScanScreenBuffer::BScanScreenBuffer() {}

BScanScreenBuffer::BScanScreenBuffer(int nChans, int xSize, int ySize)
	: sC(nChans), sX(xSize), sY(ySize)
{
	allocatedData.calloc(nChans*xSize*ySize,sizeof(float));
	channelSize = sX*sY;
	for (int i = 0; i < nChans; i++)
	{
		indicesArray.add(0);
	}
}

BScanScreenBuffer::~BScanScreenBuffer()
{
}

float* BScanScreenBuffer::getPointer(int chan, int frame, int pos) const
{
	return allocatedData+chan*channelSize+frame*sX+sY;
}

float BScanScreenBuffer::getPoint(int chan, int frame, int pos) const
{
	return *(allocatedData+chan*channelSize+frame*sX+sY);
}

void BScanScreenBuffer::resize(int nChans, int xSize, int ySize)
{
	sC = nChans;
	sX = xSize;
	sY = ySize;
	channelSize = sX*sY;

	allocatedData.calloc(nChans*xSize*ySize,sizeof(float));
	
	indicesArray.clear();
	for (int i = 0; i < nChans; i++)
	{
		indicesArray.add(0);
	}
}

void BScanScreenBuffer::clear()
{
	FloatVectorOperations::clear(allocatedData,sC*sX*sY);
	for (int i = 0; i < sC; i++)
	{
		indicesArray.set(i,0);
	}
}

int BScanScreenBuffer::getChannelIndex(int chan) const
{
	return indicesArray[chan];
}

void BScanScreenBuffer::addFromAudioSampleBuffer(AudioSampleBuffer *buffer, int channel, int startSample, int nFrames, int frameSize)
{
	float scaleFactor = frameSize / sY;
	float* channelData = allocatedData+channel*channelSize;
	int offset = indicesArray[channel]*sY;


	for (int i = 0; i < nFrames; i++)
	{
		int bufOrig=startSample+i*frameSize;

		for (int j = 0; j < sY; j++)
		{
			*(channelData+offset) = *(buffer->getSampleData(channel,bufOrig)+roundToInt(j*scaleFactor));
			offset++;
		}

		if (indicesArray[channel] >= sX)
		{
			offset = 0;
			indicesArray.set(channel,0);
		}
		else
		{
			indicesArray.set(channel,indicesArray[channel]+1);
		}

	}
	
}
