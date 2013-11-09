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

#include "BScanDisplayNode.h"

BScanDisplayNode::BScanDisplayNode()
	: GenericProcessor("B-Scan Visualizer"),
	frameIndex(0), frameSize(1024)
{
	displayBuffer = new AudioSampleBuffer(8,1024);
}

BScanDisplayNode::~BScanDisplayNode()
{
}

AudioProcessorEditor* BScanDisplayNode::createEditor()
{
	editor = new BScanDisplayEditor(this,true);
	return editor;
}

void BScanDisplayNode::setParameter(int parameterIndex, float newValue)
{
	
}

void BScanDisplayNode::updateSettings()
{

}

bool BScanDisplayNode::resizeBuffer()
{
	frameSize = channels[0]->windowLength; //This should be more sophisticates. And perhaps actually check which channels are windowed and only use them.
	int nInputs = getNumInputs();

	if (frameSize > 0 && nInputs > 0)
	{
		maxFrames=2000; //Let's keep 2000 frames for now (enouth for a 1080p display). We can make this dynamic later
		displayBuffer->setSize(nInputs,frameSize*maxFrames,false,true); 
		frameIndex = 0;
		return true;
	}
	else
	{
		return false;
	}
}

bool BScanDisplayNode::enable()
{
	if (resizeBuffer())
    {
		BScanDisplayEditor* editor = (BScanDisplayEditor*) getEditor();
        editor->enable();
        return true;
    }
    else
    {
        return false;
    }
}

bool BScanDisplayNode::disable()
{
	BScanDisplayEditor *editor= (BScanDisplayEditor*) getEditor();
	editor->disable();
	return true;
}

void BScanDisplayNode::process(AudioSampleBuffer& buffer,
                               MidiBuffer& events,
                               int& nSamples)
{
	int samplesIndex = frameIndex*frameSize;

	int numFrames = (nSamples / frameSize); //nSamples should be a multiple of frameSize, but just in case, this will trim any excess
	int samplesToRead = numFrames*frameSize;

	int samplesLeft = displayBuffer->getNumSamples() - samplesIndex;

	if (samplesToRead < samplesLeft)
	{

		for (int chan = 0; chan < buffer.getNumChannels(); chan++)
		{
			displayBuffer->copyFrom(chan,
									samplesIndex,
									buffer,
									chan,
									0,
									samplesToRead);
		}
		frameIndex+=numFrames;
		if (frameIndex >= maxFrames)
		{
			frameIndex=0;
		}
	}
	else
	{
		int extraSamples = samplesToRead - samplesLeft;

		for (int chan = 0; chan < buffer.getNumChannels(); chan++)
		{
			displayBuffer->copyFrom(chan,
									samplesIndex,
									buffer,
									chan,
									0,
									samplesLeft);
			
			displayBuffer->copyFrom(chan,
									0,
									buffer,
									chan,
									samplesLeft,
									extraSamples);

		}

		frameIndex = extraSamples/frameSize;
	}



}

int BScanDisplayNode::getFrameIndex()
{
	return frameIndex;
}

int BScanDisplayNode::getFrameSize()
{
	return frameSize;
}

AudioSampleBuffer* BScanDisplayNode::getDisplayBufferAddress()
{
	return displayBuffer;
}