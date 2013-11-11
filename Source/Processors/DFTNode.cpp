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

#include "DFTNode.h"

DFTNode::DFTNode(int nFFT, int overlap)
	: GenericProcessor("DFT Processor"), nSamplesInOverlapBuffer(0)
{
    FFTsize  = nFFT;
    nOverlap = overlap;
    step     = FFTsize - nOverlap;
    
    int nChannels = getNumInputs();
    overlapBuffer = new AudioSampleBuffer(nChannels, FFTsize);
    setPlayConfigDetails(nChannels, nChannels, getSampleRate(), getBlockSize());
}

DFTNode::~DFTNode()
{
    delete overlapBuffer;
}

void DFTNode::updateSettings()
{
	for (int i=0; i < channels.size(); i++)
	{
		channels[i]->isWindowedChannel = true;
		channels[i]->windowLength      = FFTsize; //put actual length here
	}

    overlapBuffer = new AudioSampleBuffer(channels.size(), FFTsize);
}


void DFTNode::setParameter(int parameterIndex, float newValue)
{
	
}


void DFTNode::process(AudioSampleBuffer& buffer,
                               MidiBuffer& events,
                               int& nSamples)
{
    int availableSamples = nSamples + nSamplesInOverlapBuffer;
  
    // If available samples not sufficient to process a DFT, just add the buffer sample to the overlap buffer
    if (availableSamples < FFTsize)  {
        for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
            overlapBuffer->copyFrom(channel, nSamplesInOverlapBuffer, buffer, channel, 0, nSamples);
        }
        nSamplesInOverlapBuffer += nSamples;

        buffer.setSize(buffer.getNumChannels(), 0);
        nSamples = 0;
        // need to deal with events buffer as well.
        return;
    }

    // Put in one function
    int maxIndex = (availableSamples - FFTsize + 1);
    int nSteps   =  maxIndex / step;
    if (nSteps && (maxIndex % step))
        nSteps += 1;
    
    int nOutputSamples = nSteps * FFTsize;
    int nSamplesToKeep = availableSamples - (nSteps * step + FFTsize - nOverlap);

    // First solution according to the KISS (keep it simle stupid principle)
    //    just create a buffer, concatenate overlap and buffer to it, then proceed
    AudioSampleBuffer *computingBuffer = new AudioSampleBuffer(buffer.getNumChannels(), availableSamples);
    for (int channel = 0; channel < buffer.getNumChannels(); channel ++) {
        computingBuffer->copyFrom(channel, 0, *overlapBuffer, channel, 0, nSamplesInOverlapBuffer);
        computingBuffer->copyFrom(channel, 0, buffer, channel, 0, nSamples);
    }
    
    buffer.setSize(buffer.getNumChannels(), nOutputSamples);
    nSamples = nOutputSamples;

    // Step through and DFT the samples
    float *DFTin  = new float[FFTsize];
    float *DFTout = new float[FFTsize];

    float *currentSample;
    // Fix for the overlap buffer and then continue on the new data.
    for (int channel = 0; channel < getNumInputs(); channel++)  {
        currentSample = computingBuffer->getSampleData(channel);
        for (int currentStep = 0; currentStep < nSteps; currentStep++, currentSample += step) {
            // Window the DFTin - the memcpy is just for testing purposes
            memccpy(DFTin, currentSample, FFTsize, sizeof(float));
            // DFT DFTin -> DFTout - the memcpy is just for testing purposes
            memccpy(DFTout, DFTin, FFTsize, sizeof(float));
            
            buffer.copyFrom(channel, currentStep * FFTsize, DFTout, FFTsize);
        }
        overlapBuffer->copyFrom(channel, 0, *computingBuffer, channel, availableSamples - nSamplesToKeep, nSamplesToKeep);
    }
}