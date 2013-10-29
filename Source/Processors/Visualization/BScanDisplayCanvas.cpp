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



BScanScreenBuffer::BScanScreenBuffer(int nChans, int xSize, int ySize)
	: sC(nChans), sX(xSize), sY(ySize)
{
	allocatedData.calloc(nChans*xSize*ySize,sizeof(float));
	channelSize = sX*sY;
	for (int i = 0; i < nChans; i++)
	{
		indicesArray.add(0);
		lastIndexArray.add(0);
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
		lastIndexArray.add(0);
	}
}

void BScanScreenBuffer::clear()
{
	FloatVectorOperations::clear(allocatedData,sC*sX*sY);
	for (int i = 0; i < sC; i++)
	{
		indicesArray.set(i,0);
		lastIndexArray.set(i,0);
	}
}

int BScanScreenBuffer::getChannelIndex(int chan) const
{
	return indicesArray[chan];
}

void BScanScreenBuffer::addFromAudioSampleBuffer(AudioSampleBuffer& buffer, int channel, int startSample, int nFrames, int frameSize)
{
	float scaleFactor = frameSize / sY;
	float* channelData = allocatedData+channel*channelSize;
	int offset = lastIndexArray[channel]*sY;


	for (int i = 0; i < nFrames; i++)
	{
		if (lastIndexArray[channel] >= sX)
		{
			offset = 0;
			lastIndexArray.set(channel,0);
		}
		else
		{
			lastIndexArray.set(channel,lastIndexArray[channel]+1);
		}

		int bufOrig=startSample+i*frameSize;

		for (int j = 0; j < sY; j++)
		{
			*(channelData+offset) = *(buffer.getSampleData(channel,bufOrig)+roundToInt(j*scaleFactor));
			offset++;
		}

	}

	if (!cicling)
	{
		writtenBeforeCicling+=nFrames;
		if (writtenBeforeCicling > sX)
		{
			cicling=true;
		}
	}
	
	if (cicling)
	{
		indicesArray.set(channel,(indicesArray[channel]+nFrames) % sX);
	}
	
}
