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

#ifndef __DFTNODE_H_96561501__
#define __DFTNODE_H_96561501__

#include "../../JuceLibraryCode/JuceHeader.h"
#include "GenericProcessor.h"

class DFTNode : public GenericProcessor
{
public:

	DFTNode();

	~DFTNode();

	bool isSource()
	{
		return false;
	}

	bool isSink()
	{
		return false;
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

	/** Called when the signal chain changes

		Every time the chain is modified, as the number of input channels or other
		settings may change, this function is called to update the internal settings
		of the processor. Typical examples are bufer sizes. */
	void updateSettings();
	
private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DFTNode);

}



#endif  // __DFTNODE_H_96561501__
