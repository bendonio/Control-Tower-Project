//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __CONTROLTOWER_CONTROLTOWER_H_
#define __CONTROLTOWER_CONTROLTOWER_H_

#include <omnetpp.h>
#include <string>
#include <omnetpp/cqueue.h>
#include "Airplane_m.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class ControlTower : public cSimpleModule{
private:
    cQueue *landing_queue;
    cQueue *take_off_queue;
    Airplane *runway;
    int planes_served;
    simsignal_t LandingQueueTimeSignal;
    simsignal_t TakeoffQueueTimeSignal;
    simsignal_t ThroughputSignal;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual bool can_land_immediately();
    virtual bool can_take_off_immediately();
    virtual void finish();
};

#endif
