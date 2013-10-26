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

DFTNode::DFTNode()
	: GenericProcessor("DFT Processor")
{
}

DFTNode::~DFTNode()
{
}

void DFTNode::setParameter(int parameterIndex, float newValue)
{
	
}

void DFTNode::updateSettings()
{
	/* Something on the lines of
	circularbuffer.Resize(getNumInputs(),M);
	*/

}

void DFTNode::process(AudioSampleBuffer& buffer,
                               MidiBuffer& events,
                               int& nSamples)
{


}