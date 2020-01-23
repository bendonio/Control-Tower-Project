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
    LandingQueueTimeSignal = registerSignal("LandingQueueTime");
}

void ControlTower::handleMessage(cMessage *msg){

    EV<< "Message is: " << msg->getName()<<endl;

    if(msg->isSelfMessage()){
        if(strcmp(msg->getName(), "landing") == 0){       //Landing time expired, plane in the parking area

            Airplane *plane = check_and_cast<Airplane*>(runway);
            EV<< runway->getName()<< endl;
            EV<< "Plane " << plane->getId() << " landed !!\n Going to the parking area\n";

            cMessage *newmsg = new cMessage("check_queue"); //Sending a self-message to check the queues.
            scheduleAt(simTime() , newmsg);
            send(plane, "outPA");       //Sending the "plane" to the parking area
            runway = nullptr;           // Free the runway

        } else{
            if(strcmp(msg->getName(), "check_queue") == 0){     //Check the queue

                if(!take_off_queue->isEmpty()){                 //First: check the take-off queue!

                    runway = (Airplane*)take_off_queue->pop();  //If there's some plane waiting for take off, pop the first one and put it on the runway
                    EV <<"Plane " << runway->getId() << " start take-off\n";
                    if(runway != nullptr)
                       EV << "Runway occupied until " << simTime() + runway->getTakeOffTime() <<" for take-off\n";

                    cMessage *take_off = new cMessage("take-off");
                    scheduleAt(simTime() + runway->getTakeOffTime() , take_off);    //Only after the take-off we can check the queues again

                } else{
                    if(!landing_queue->isEmpty()){              //Only if the take-off queue is empty

                        runway = (Airplane*)landing_queue->pop(); //If there's some plane waiting for landing, pop the first one and put it on the runway
                        EV<<"Plane " << runway->getId() << " start landing\n";
                        if(runway != nullptr)
                            EV << "Runway occupied until " << simTime() + runway->getLandingTime()<<" for landing\n";

                        cMessage *landing = new cMessage("landing");    //Only after the landing we can check the queues again
                        scheduleAt(simTime() + runway->getLandingTime() , landing);
                        emit(LandingQueueTimeSignal, simTime().dbl() - runway->getTimeInsertedLQ());   //statistic
                    }
                }
            } else{
                if(strcmp(msg->getName(), "take-off") == 0){    //Take-off time expired, plane left the airport
                    EV<<"Plane " << runway->getId() << " took-off.\n";
                    delete runway;
                    runway = nullptr;

                    cMessage *newmsg = new cMessage("check_queue"); //Sending a self-message to check the queues.
                    scheduleAt(simTime() , newmsg);
                }
            }
        }

    } else{

        if(strcmp(msg->getArrivalGate()->getName(), "inAS") == 0){  //If they are info on a new plane

            Airplane *info = check_and_cast<Airplane*>(msg);
            double landingTime = info->getLandingTime();
            EV<<"Here's the Control Tower !!  the landing time is: " << landingTime <<"seconds.\n";

            info->setTimeInsertedLQ(simTime().dbl());
            bool land = check_for_landing();
            if(land == true){ //the plane can land immediately, do not put it in the queue

                runway = info;  //Put the plane on the runway
                EV<< "OK, Plane " << runway->getId() << " start landing\n";
                EV << "Runway occupied until " << simTime() + runway->getLandingTime() << " seconds\n";

                cMessage *landing = new cMessage("landing");    //ma
                scheduleAt(simTime() + runway->getLandingTime() , landing);
                emit(LandingQueueTimeSignal, simTime().dbl() - runway->getTimeInsertedLQ());

            } else{

                landing_queue->insert(info);       //qui ci potrebbe essere l'errore, inserisco il messaggio con nome "Airplane info" (guarda Airspace.cc)
                EV << "Plane " << info->getId() << " inserted in the landing queue, wait for OK!\n";

            }
            //scheduleAt(simTime() , newmsg);     //questo non credo vada messo quiiii
            //scheduleAt(simTime() + landingTime, msg);

        } else {                                //Plane ended its parking area time

            Airplane *info = check_and_cast<Airplane*>(msg);
            double takeOffTime = info->getTakeOffTime();
            EV<<"Here's the Control Tower !! Info about plane " << info->getId() << " received, the take-off time is: " << takeOffTime <<"seconds.\n";

            bool take_off = check_for_take_off();
            if(take_off == true){   //Runway unoccupied and no others planes queueing for take-off -> Plane can start take-off

                runway = info;
                EV<< "OK, Plane " << runway->getId() << " start take-off\n";
                EV << "Runway occupied until " << simTime() + runway->getTakeOffTime() << " seconds\n";

                cMessage *take_off = new cMessage("take-off");

                scheduleAt(simTime() + runway->getTakeOffTime() , take_off);

            } else{ // Runway occupied or/& other planes queueing for take-off

                take_off_queue->insert(info);
                EV << "Plane inserted in the take off queue, wait for OK!\n";
//                cMessage *newmsg = new cMessage("check_queue");
//                scheduleAt(simTime() , newmsg);
            }
        }
    }
}

bool ControlTower :: check_for_landing(){
    if((runway == nullptr) && landing_queue->isEmpty() && take_off_queue->isEmpty() ){

        EV << "OK, the plane can land\n";
        return true;

    } else{
        EV << "Runway occupied or other planes waiting, the plane has to wait\n";
        return false;
    }
}

bool ControlTower :: check_for_take_off(){
    if((runway == nullptr) && take_off_queue->isEmpty()){

        EV << "OK, the plane can take-off\n";
        return true;

    } else{
        EV << "Runway occupied or other planes waiting, the plane has to wait\n";
        return false;
    }
}
