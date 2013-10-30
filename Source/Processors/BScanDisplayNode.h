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

#ifndef __BSCANDISPLAYNODE_H_3D2C11CC__
#define __BSCANDISPLAYNODE_H_3D2C11CC__

#include "../../JuceLibraryCode/JuceHeader.h"
#include "GenericProcessor.h"

class BScanDisplayNode : public GenericProcessor
{
public:

	BScanDisplayNode();
	
	~BScanDisplayNode();

	bool isSource()
	{
		return false;
	}

	bool isSink()
	{
		return true;
	}

	/** Defines the functionality of the processor.

        The process method is called every time a new data buffer is available.

        Processors can either use this method to add new data, manipulate existing
        data, or send data to an external target (such as a display or other hardware).

        Continuous signals arrive in the "buffer" variable, event data (such as TTLs
        and spikes) is contained in the "events" variable, and "nSamples" holds the
        number of continous samples in the current buffer (which may differ from the
        size of the buffer).
         */
    void process(AudioSampleBuffer& buffer, MidiBuffer& events, int& nSamples);

    /** Any variables used by the "process" function _must_ be modified only through
        this method while data acquisition is active. If they are modified in any
        other way, the application will crash.  */
    void setParameter(int parameterIndex, float newValue);

	void updateSettings();

	bool enable();
	bool disable();

	int getFrameIndex();
	int getFrameSize();

	AudioSampleBuffer* getDisplayBufferAddress();
	
private:

	ScopedPointer<AudioSampleBuffer> displayBuffer;
	int frameIndex;

	int frameSize;

	bool resizeBuffer();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BScanDisplayNode);

};



#endif  // __BSCANDISPLAYNODE_H_3D2C11CC__
