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

#include "ControlTower.h"


Define_Module(ControlTower);

void ControlTower::initialize()
{
    landing_queue = new cQueue("landing_queue");
    take_off_queue = new cQueue("take_off_queue");
    runway = nullptr;
    ok_received = false;
    planes_served = 0;
    statistic = new cMessage("stat");
    statistic->setSchedulingPriority(5);
    scheduleAt(simTime() + 300, statistic);
    LandingQueueTimeSignal = registerSignal("LandingQueueTime");    // Time spent in the landing queue
    TakeoffQueueTimeSignal = registerSignal("TakeoffQueueTime");    // Time spent in the takeoff queue
    ThroughputSignal = registerSignal("Throughput");                // Throughput (# planes that have left the airport / time)
    ResponseTimeSignal = registerSignal("ResponseTime");            // Response time (for how long a plane has been in the system)
    LQ_length_signal = registerSignal("LQ_length");
    TQ_length_signal = registerSignal("TQ_length");
}

void ControlTower::handleMessage(cMessage *msg){

    EV<< "Message is: " << msg->getName()<<endl;

    if(msg->isSelfMessage()){
        if(strcmp(msg->getName(), "done") == 0){
            EV << "Simulation ended." << endl;
        }
        if(strcmp(msg->getName(), "stat") == 0) {
            emit(LQ_length_signal, landing_queue->getLength());
            emit(TQ_length_signal, take_off_queue->getLength());
            scheduleAt(simTime() + 300, statistic);
        }
        if(strcmp(msg->getName(), "landing") == 0){       //Landing time expired, plane in the parking area

            Airplane *plane = check_and_cast<Airplane*>(runway);
            EV<< runway->getName()<< endl;
            EV<< "Plane " << plane->getId() << " landed !!\n Going to the parking area" << endl;

            runway = nullptr;           // Free the runway
            ok_received = false;
            cMessage *newmsg = new cMessage("check_queue"); //Sending a self-message to check the queues.
            newmsg->setSchedulingPriority(4);
            scheduleAt(simTime() , newmsg);
            send(plane, "outPA");       //Sending the "plane" to the parking area
            delete msg;

        } else{
            if(strcmp(msg->getName(), "check_queue") == 0){     //Check the queue

                if(!take_off_queue->isEmpty() && !ok_received){                 //First: check the take-off queue!

                    runway = (Airplane*)take_off_queue->pop();  //If there's some plane waiting for take off, pop the first one and put it on the runway
                    EV <<"Plane " << runway->getId() << " start take-off" << endl;
                    if(runway != nullptr)
                       EV << "Runway occupied until " << simTime() + runway->getTakeOffTime() <<" for take-off" << endl;

                    cMessage *take_off = new cMessage("take-off");
                    take_off->setSchedulingPriority(1);
                    scheduleAt(simTime() + runway->getTakeOffTime() , take_off);    //Only after the take-off we can check the queues again
                    emit(TakeoffQueueTimeSignal, simTime().dbl() - runway->getTimeInsertedTQ());   //statistic

                } else{
                    if(!landing_queue->isEmpty() && !ok_received){              //Only if the take-off queue is not empty

                        runway = (Airplane*)landing_queue->pop(); //If there's some plane waiting for landing, pop the first one and put it on the runway
                        EV<<"Plane " << runway->getId() << " start landing" << endl;
                        if(runway != nullptr)
                            EV << "Runway occupied until " << simTime() + runway->getLandingTime()<<" for landing" << endl;

                        cMessage *landing = new cMessage("landing");    //Only after the landing we can check the queues again
                        landing->setSchedulingPriority(3);
                        scheduleAt(simTime() + runway->getLandingTime() , landing);
                        emit(LandingQueueTimeSignal, simTime().dbl() - runway->getTimeInsertedLQ());   //statistic
                    }
                }
                delete msg;
            } else{
                if(strcmp(msg->getName(), "take-off") == 0){    //Take-off time expired, plane left the airport
                    EV<<"Plane " << runway->getId() << " took-off." << endl;

                    emit(ResponseTimeSignal, simTime().dbl() - runway->getTimeOfArrival());

                    delete runway;
                    runway = nullptr;
                    ok_received = false;
                    planes_served++;
                    emit(ThroughputSignal, planes_served / simTime().dbl());

                    cMessage *newmsg = new cMessage("check_queue"); //Sending a self-message to check the queues.
                    newmsg->setSchedulingPriority(4);
                    scheduleAt(simTime() , newmsg);
                    delete msg;
                }
            }
        }

    } else{

        if(strcmp(msg->getArrivalGate()->getName(), "inAS") == 0){  //If they are info on a new plane

            Airplane *info = check_and_cast<Airplane*>(msg);
            double landingTime = info->getLandingTime();
            EV<<"Here's the Control Tower !!  the landing time is: " << landingTime <<"seconds." << endl;

            if(can_land_immediately()){ //the plane can land immediately, do not put it in the queue

                runway = info;  //Put the plane on the runway
                ok_received = true;
                EV<< "OK, Plane " << runway->getId() << " start landing (can land immediately)" << endl;
                EV << "Runway occupied until " << simTime() + runway->getLandingTime() << " seconds" << endl;

                cMessage *landing = new cMessage("landing");    //ma
                landing->setSchedulingPriority(3);
                scheduleAt(simTime() + runway->getLandingTime() , landing);
                emit(LandingQueueTimeSignal, 0);    // The plane does not spend any time in the queue

            } else {

                info->setTimeInsertedLQ(simTime().dbl());
                landing_queue->insert(info);       //qui ci potrebbe essere l'errore, inserisco il messaggio con nome "Airplane info" (guarda Airspace.cc)
                EV << "Plane " << info->getId() << " inserted in the landing queue, wait for OK!" << endl;

            }
            //scheduleAt(simTime() , newmsg);     //questo non credo vada messo quiiii
            //scheduleAt(simTime() + landingTime, msg);

        } else {                                //Plane ended its parking area time

            Airplane *info = check_and_cast<Airplane*>(msg);
            double takeOffTime = info->getTakeOffTime();
            EV<<"Here's the Control Tower !! Info about plane " << info->getId() << " received, the take-off time is: " << takeOffTime <<"seconds." << endl;

            if(can_take_off_immediately()){   //Runway unoccupied and no others planes queueing for take-off -> Plane can start take-off

                runway = info;
                ok_received = true;
                EV<< "OK, Plane " << runway->getId() << " start take-off" << endl;
                EV << "Runway occupied until " << simTime() + runway->getTakeOffTime() << " seconds" << endl;

                cMessage *take_off = new cMessage("take-off");
                take_off->setSchedulingPriority(1);

                scheduleAt(simTime() + runway->getTakeOffTime() , take_off);
                emit(TakeoffQueueTimeSignal, 0);

            } else{ // Runway occupied and/or other planes queued for take-off

                info->setTimeInsertedTQ(simTime().dbl());
                take_off_queue->insert(info);
                EV << "Plane inserted in the take off queue, wait for OK!" << endl;
//                cMessage *newmsg = new cMessage("check_queue");
//                scheduleAt(simTime() , newmsg);
            }
        }
    }
}

/*
 * The plane can land if the runway is free, the take_off queue is empty, and either the landing_queue is empty, or the plane that
 * requested the take_off is the first in the queue.
 */
bool ControlTower::can_land_immediately(){
    if((runway == nullptr) && take_off_queue->isEmpty() && landing_queue->isEmpty() && !ok_received){

        EV << "OK, the plane can land" << endl;
        return true;

    } else{
        EV << "Runway occupied or other planes waiting, the plane has to wait" << endl;
        return false;
    }
}

/*
 * The plane can take off if the runway is free and it's the first in the take off queue.
 */
bool ControlTower::can_take_off_immediately(){
    if((runway == nullptr) && take_off_queue->isEmpty() && !ok_received){

        EV << "OK, the plane can take-off" << endl;
        return true;

    } else{
        EV << "Runway occupied or other planes waiting, the plane has to wait" << endl;
        return false;
    }
}

void ControlTower::finish() {
    if (runway != nullptr)
        recordScalar("#runway", 1);
    else
        recordScalar("#runway", 0);
    recordScalar("#served", planes_served);
    recordScalar("#landing_queue", landing_queue->getLength());
    recordScalar("#takeoff_queue", take_off_queue->getLength());
    delete landing_queue;
    delete take_off_queue;
    delete runway;
    runway = nullptr;
}
