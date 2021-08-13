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

Define_Module(Airspace);

void Airspace::initialize()
{
    airplane = new cMessage("new Airplane");

    airplane_counter = 0;
    interarrivalRNGIdx = par("interarrivalRNGIdx").intValue();
    landingRNGIdx = par("landingRNGIdx").intValue();
    parkingRNGIdx = par("parkingRNGIdx").intValue();
    takeoffRNGIdx = par("takeoffRNGIdx").intValue();
    interarrivalDistr = par("interarrivalDistr").stringValue();
    landingDistr = par("landingDistr").stringValue();
    parkingDistr = par("parkingDistr").stringValue();
    takeoffDistr = par("takeoffDistr").stringValue();
    interarrivalAvgRate = par("interarrivalAvgRate").doubleValue();
    landingAvgRate = par("landingAvgRate").doubleValue();
    parkingAvgRate = par("parkingAvgRate").doubleValue();
    takeoffAvgRate = par("takeoffAvgRate").doubleValue();

    if (interarrivalAvgRate <= 0 || landingAvgRate <= 0 || parkingAvgRate <= 0 || takeoffAvgRate <= 0)
        throw new cException("Rate value can't be <= 0");

    EV << "Mean interarrival time is: "<< 1 / interarrivalAvgRate << endl;
    // EV<<"The airplane will arrive in "<< (interarrivalTime/60) << "minutes (" << interarrivalTime<<" seconds)" << endl;
    scheduleNextPlane(airplane);
}

void Airspace::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()){   //a new airplane arrived

        EV << "Airplane " << airplane_counter << " arrived, ready for landing." << endl;

        // Notify the control tower that airplane is starting the landing

        Airplane *notify = spawnPlane();

        EV<<"Notifying the control tower about the landing." << endl;
        send(notify, "outCT");
        scheduleNextPlane(airplane);
    }
}

Airplane* Airspace::spawnPlane() {

    double landingTime, parkingAreaTime, takeOffTime;

    if (landingDistr.compare("exponential") == 0) {
        landingTime = exponential(1 / landingAvgRate, landingRNGIdx);
    } else {
        landingTime = 1 / landingAvgRate;
    }

    if (parkingDistr.compare("exponential") == 0) {
        parkingAreaTime = exponential(1 / parkingAvgRate, parkingRNGIdx);
    } else {
        parkingAreaTime = 1 / parkingAvgRate;
    }

    if (takeoffDistr.compare("exponential") == 0) {
        takeOffTime = exponential(1 / takeoffAvgRate, takeoffRNGIdx);
    } else {
        takeOffTime = 1 / takeoffAvgRate;
    }

    EV<< "The landing will take " << landingTime << "seconds (" << (landingTime/60) << " minutes)" << endl;
    EV<< "The time spent in the parking area will be " << parkingAreaTime << "seconds (" << (parkingAreaTime/60) << " minutes, " <<(parkingAreaTime/3600) << "hours)" << endl;
    EV<< "The take-off will take " << takeOffTime << "seconds (" << (takeOffTime/60) << " minutes)" << endl;

    Airplane *airplane = new Airplane("Airplane info");
    airplane->setLandingTime(landingTime);
    airplane->setParkingAreaTime(parkingAreaTime);
    airplane->setTakeOffTime(takeOffTime);
    airplane->setId(airplane_counter++);
    airplane->setTimeInsertedLQ(0);
    airplane->setTimeInsertedTQ(0);
    airplane->setSchedulingPriority(0);

    return airplane;

}

void Airspace::scheduleNextPlane(cMessage *plane) {

    double interval;

    if (interarrivalDistr.compare("exponential") == 0) {
        interval = exponential(1 / interarrivalAvgRate, interarrivalRNGIdx);
    } else {
        interval = 1 / interarrivalAvgRate;
    }

    scheduleAt(simTime() + interval, plane);

}
