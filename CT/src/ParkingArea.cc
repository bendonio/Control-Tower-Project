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

#include "ParkingArea.h"
#include "Airplane_m.h"

Define_Module(ParkingArea);

void ParkingArea::initialize()
{
    numPlane = 0;
    numPlaneSignal = registerSignal("numPlane");
    //stat = new cMessage("stat");
    //scheduleAt(simTime() + 30, stat);
}

void ParkingArea::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()){

        Airplane *info = check_and_cast<Airplane*>(msg);
        int id = info->getId();
        EV<<"Airplane " << id << " have to take off now.\n";
        EV<<"Notifying the control tower about the take-off.\n";

        numPlane--;
        msg->setSchedulingPriority(0);
        send(msg, "outCT");
        emit(numPlaneSignal, numPlane);

    } else{
        Airplane *info = check_and_cast<Airplane*>(msg);
        int id = info->getId();
        double parkingAreaTime = info->getParkingAreaTime();

        EV<<"Airplane "<< id<< " arrived in the parking area.\n";
        EV<<"It will stay here for " << parkingAreaTime << " seconds (" << parkingAreaTime/60 << " minutes, " << parkingAreaTime/3600 << " hours).\n";

        numPlane++;
        scheduleAt(simTime() + parkingAreaTime , msg);
        emit(numPlaneSignal, numPlane);


    }
}
