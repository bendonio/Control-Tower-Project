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

#include "Airspace.h"
#include "Airplane_m.h"

Define_Module(Airspace);

void Airspace::initialize()
{
    airplane = new cMessage("new Airplane");

    airplane_counter = 0;
    interarrivalDistr = par("interarrivalDistr").stringValue();
    interarrivalTime = par("interarrivalTime").doubleValue();
    EV<< "Interarrival time is: "<< interarrivalTime << endl;
    EV<<"The airplane will arrive in "<< (interarrivalTime/60) << "minutes (" << interarrivalTime<<" seconds)\n";
    scheduleNextPlane(airplane);
}

void Airspace::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()){   //a new airplane arrived

        EV<<"Airplane " <<airplane_counter<<" arrived, ready for landing.\n";

        // Notify the control tower that airplane is starting the landing//

        double landingTime = uniform(0, 600);   // time for landing (seconds)
        double parkingAreaTime = uniform(1000, 5400);  //time spent in parking area (seconds)
        double takeOffTime = uniform(0, 600);   //time for take-off (seconds)

        EV<< "The landing will take " << landingTime << "seconds (" << (landingTime/60) << " minutes)\n";
        EV<< "The time spent in the parking area will be " << parkingAreaTime << "seconds (" << (parkingAreaTime/60) << " minutes, " <<(parkingAreaTime/3600) << "hours)\n";
        EV<< "The take-off will take " << takeOffTime << "seconds (" << (takeOffTime/60) << " minutes)\n";

        Airplane *notify= new Airplane("Airplane info");
        notify->setLandingTime(landingTime);
        notify->setParkingAreaTime(parkingAreaTime);
        notify->setTakeOffTime(takeOffTime);
        notify->setId(airplane_counter++);
        notify->setTimeInsertedLQ(0);
        notify->setTimeInsertedTQ(0);

        EV<<"Notifying the control tower about the landing.\n";
        send(notify, "outCT");
        scheduleNextPlane(airplane);
    }
}

void Airspace::scheduleNextPlane(cMessage *plane) {

    double interval;

    if (interarrivalDistr.compare("constant") == 0) {
        interval = interarrivalTime;
    }
    else if (interarrivalDistr.compare("exponential") == 0) {
        interval = exponential(interarrivalTime);
    }

    scheduleAt(simTime() + interval, plane);

}
