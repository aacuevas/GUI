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
	channelHeight(100), channelWidth(100)
{

	displayBuffer = processor->getDisplayBufferAddress();
	nChans = processor->getNumInputChannels();

	std::cout << "creating BScanCanvas for " << nChans << "channels" <<std::endl;
	
	screenBuffer = new BScanScreenBuffer();
	viewport = new Viewport();
	bScanDisplay = new BScanDisplay(this, viewport);

	viewport->setViewedComponent(bScanDisplay);
	viewport->setScrollBarsShown(true,false);

	scrollBarThickness = viewport->getScrollBarThickness();

	addAndMakeVisible(viewport);

	

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

	bScanDisplay->updateSettings(channelHeight, nColumns);
	bScanDisplay->setNumChannels(nChans);
}

BScanDisplayCanvas::~BScanDisplayCanvas() {
bScanDisplay = nullptr; //For some reason if ww don't delete this before the viewport the program crashes
}

void BScanDisplayCanvas::resized()
{
	int nRows = ceil((float)nChans/nColumns);

	viewport->setBounds(0,0,getWidth(),getHeight()-30);
	bScanDisplay->setBounds(0,0,getWidth()-scrollBarThickness, channelHeight*nRows+50*(nRows-1)+10);
	
	heightSelector->setBounds(5,getHeight()-30,100,25);
	columnsSelector->setBounds(175,getHeight()-30,100,25);

	channelWidth = bScanDisplay->getChannelWidth();
	
	if (channelWidth <= 0) //To avoid problems on initialization
	{
		channelWidth = 100;
	}

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

	bScanDisplay->setNumChannels(nChans);
	resized();

}

void BScanDisplayCanvas::resizeScreenBuffer()
{
	std::cout << "BScan: Screenbuffer resize. nChans: " << nChans << " W: " << channelWidth << " H: " << channelHeight << std::endl;
	screenBuffer->resize(nChans,channelWidth,channelHeight);
	if (running)
	{
		refillBuffer = true;
	}
}

void BScanDisplayCanvas::updateScreenBuffer()
{
	int  startFrame,framesToRead,extraFrames;

	int currentIndex = processor->getFrameIndex();
	int frameSize = processor->getFrameSize();

	int maxFrames = displayBuffer->getNumSamples() / frameSize;
	int framesLeft = maxFrames - lastIndex;

	if (refillBuffer)
	{
		refillBuffer = false;
		startFrame = currentIndex - channelWidth;
		if (startFrame < 0)
		{
			startFrame = maxFrames+startFrame;
		}
		framesToRead = channelWidth;
	}
	else
	{
		startFrame = lastIndex;
		if (lastIndex < currentIndex)
		{
			framesToRead = currentIndex-lastIndex;
		}
		else
		{
			framesToRead=framesLeft+currentIndex;
		}
	}

	if (framesToRead < framesLeft)
	{
		for (int channel=0; channel < nChans; channel++)
		{
			screenBuffer->addFromAudioSampleBuffer(displayBuffer,channel,startFrame*frameSize,framesToRead,frameSize);
		}
	}
	else
	{
		extraFrames = framesToRead-framesLeft;
		for (int channel=0; channel < nChans; channel++)
		{
			screenBuffer->addFromAudioSampleBuffer(displayBuffer,channel,startFrame*frameSize,framesLeft,frameSize);
			screenBuffer->addFromAudioSampleBuffer(displayBuffer,channel,0,extraFrames,frameSize);
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
	}
	else if (cb == columnsSelector)
	{
		nColumns = columns[cb->getSelectedId()-1].getIntValue();
	}
	bScanDisplay->updateSettings(channelHeight, nColumns);
	resized();
}

BScanDisplay::BScanDisplay(BScanDisplayCanvas *c, Viewport *v)
	: canvas(c), viewport(v), channelHeight(100), nColumns(1), channelWidth(100)
{
}

BScanDisplay::~BScanDisplay()
{
	deleteAllChildren();
}

void BScanDisplay::updateSettings(int chanHeight, int nCols)
{
	channelHeight = chanHeight;
	nColumns = nCols;

	updateChannelWidth();
}

void BScanDisplay::updateChannelWidth()
{

	channelWidth = floor((getWidth() -10 - 50*(nColumns-1))/nColumns);
}

int BScanDisplay::getChannelWidth()
{
	return channelWidth;
}

void BScanDisplay::setNumChannels(int n)
{
	nChans = n;

	deleteAllChildren();

	channelArray.clear();

	for (int i = 0 ; i < nChans; i++)
	{
		BScanChannelDisplay *chan = new BScanChannelDisplay(canvas, this, i);

		addAndMakeVisible(chan);
		channelArray.add(chan);
	}
	resized();

}

void BScanDisplay::resized()
{
	int xPos, yPos, col, row;

	updateChannelWidth();

	for (int i = 0; i < nChans; i++)
	{
		row = i/nColumns;
		col = i%nColumns;

		xPos=10+(col)*(channelWidth+50);
		yPos=10+(row)*(channelHeight+50);

		channelArray[i]->setBounds(xPos,yPos,channelWidth,channelHeight);
	}
	refresh();
}

void BScanDisplay::paint(Graphics &g)
{
}

void BScanDisplay::refresh()
{
	int topBorder = viewport->getViewPositionY();
	int bottomBorder = viewport->getViewHeight() + topBorder;

	for (int i = 0; i < nChans; i++)
	{
		int componentTop = channelArray[i]->getY();
		int componentBottom = channelArray[i]->getHeight() + componentTop;

		if ((topBorder <= componentBottom && bottomBorder >= componentTop))
		{
			channelArray[i]->repaint();
		}

	}
}


BScanChannelDisplay::BScanChannelDisplay(BScanDisplayCanvas *c, BScanDisplay *d, int nc)
	: canvas(c), display(d), chan(nc)
{}

BScanChannelDisplay::~BScanChannelDisplay()
{
}



void BScanChannelDisplay::paint(Graphics &g)
{
	BScanScreenBuffer *buffer = canvas->getScreenBuffer();
	Image* data = buffer->getPointer(chan);
	int index = buffer->getChannelIndex(chan);
	int nFrames = getWidth();
	int frameSize = getHeight();

	int x=0;
	int y;
	
	g.drawImage(*data,0,0,nFrames-index,frameSize,index,0,nFrames-index,frameSize);
	g.drawImage(*data,nFrames-index,0,index,frameSize,0,0,index,frameSize);

}


BScanScreenBuffer::BScanScreenBuffer() {}

BScanScreenBuffer::BScanScreenBuffer(int nChans, int xSize, int ySize)
	: sC(nChans), sX(xSize), sY(ySize)
{
	buffer.clear();
	channelSize = sX*sY;
	for (int i = 0; i < nChans; i++)
	{
		Image* im = new Image(Image::PixelFormat::RGB,sX,sY,false);
		im->clear(Rectangle<int>(sX,sY),colorFromNormalizedPower(0));
		buffer.add(im);
		indicesArray.add(0);
	}
}

BScanScreenBuffer::~BScanScreenBuffer()
{
}

Image* BScanScreenBuffer::getPointer(int chan) const
{
	return buffer[chan];
}



void BScanScreenBuffer::resize(int nChans, int xSize, int ySize)
{
	sC = nChans;
	sX = xSize;
	sY = ySize;
	channelSize = sX*sY;

	buffer.clear();
	
	indicesArray.clear();
	for (int i = 0; i < nChans; i++)
	{
		Image* im = new Image(Image::PixelFormat::RGB,sX,sY,false);
		im->clear(Rectangle<int>(sX,sY),colorFromNormalizedPower(0));
		buffer.add(im);
		indicesArray.add(0);
	}
}

void BScanScreenBuffer::clear()
{
	for (int i = 0; i < sC; i++)
	{
		buffer[i]->clear(Rectangle<int>(sX,sY),colorFromNormalizedPower(0));
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
	Image* channelData = this->buffer[channel];
	int posX = indicesArray[channel];


	for (int i = 0; i < nFrames; i++)
	{
		int bufOrig=startSample+i*frameSize;

		for (int j = 0; j < sY; j++)
		{
			channelData->setPixelAt(posX,j,colorFromNormalizedPower(
				*(buffer->getSampleData(channel,bufOrig)+roundToInt(j*scaleFactor))));
			
		}
		posX++;
		if (posX >= sX)
		{
			posX = 0;
		}

	}
	indicesArray.set(channel,posX);
	
}

Colour BScanScreenBuffer::colorFromNormalizedPower(float pow)
{
	int r,g,b;

	if (pow < 0.125)
	{
		r=0;
		g=0;
		b=127+1020*pow; //pow/0.125*255
	}
	else if (pow < 0.375)
	{
		r=0;
		g=(pow-0.125)*1020;
		b=255;
	}
	else if (pow < 0.625)
	{
		r=(pow-0.375)*1020;
		g=255;
		b=255-(pow-0.375)*1020;
	}
	else if (pow < 0.875)
	{
		r=255;
		g=255-(pow-0.625)*1020;
		b=0;
	}
	else
	{
		r=255-(pow-0.875)*1020;
		g=0;
		b=0;
	}

	return Colour(r,g,b);
}