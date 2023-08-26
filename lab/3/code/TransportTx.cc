#ifndef TRANSPORT_TX
#define TRANSPORT_TX

#include <omnetpp.h>
#include <string.h>

using namespace omnetpp;

class TransportTx: public cSimpleModule {
private:
    cQueue bufferData;
    cMessage *endServiceEvent;

    cMessage * startServiceEvent;
    bool canSend;

    simtime_t serviceTime;

    cOutVector bufferSizeVector;
    cOutVector packetDropVector;

    double packetDrop;
    double feedbacktCount;

public:
    TransportTx();
    virtual ~TransportTx();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);

    virtual void handlePkt(cMessage *msg);
};

Define_Module(TransportTx);

TransportTx::TransportTx() {
    endServiceEvent = NULL;
    startServiceEvent = NULL;
}

TransportTx::~TransportTx() {
    cancelAndDelete(endServiceEvent);
}

void TransportTx::initialize() {
    bufferData.setName("bufferData");

    endServiceEvent = new cMessage("endService");
    startServiceEvent = new cMessage("startService");

    packetDropVector.setName("packetDropVector");
    bufferSizeVector.setName("bufferSizeVector");

    packetDrop = 0;
    feedbacktCount = 0;

    canSend = true;

}

void TransportTx::finish() {
    recordScalar("Number of packets lost", packetDrop);
    recordScalar("Number of feedback packets", feedbacktCount);
}

void TransportTx::handlePkt(cMessage *msg) {

    // Si no entra en el buffer lo descartamos
    if (bufferData.getLength() >= par("bufferSize").intValue()) {
        delete msg;
        this->bubble("packet dropped");
        packetDrop++;
        packetDropVector.record(packetDrop);
    }
    // Si entra en el buffer lo guardo y lo scheduleo
    else {
        bufferData.insert(msg);
        if (!endServiceEvent->isScheduled()) {
            scheduleAt(simTime() + 0, endServiceEvent);
        }
    }
}



void TransportTx::handleMessage(cMessage *msg) {

    bufferSizeVector.record(bufferData.getLength());

    if (msg->getKind() == 2) {
        feedbacktCount++;
        EV << "CONGESTION! WAITING!";
        canSend = false;

        if (!startServiceEvent->isScheduled()) {
            scheduleAt(simTime() + par("waitTime"), startServiceEvent);
        }
        delete msg;

    } else {

        if (msg == startServiceEvent) {
            canSend = true;

        } else if (msg == endServiceEvent) {

            if (canSend && !bufferData.isEmpty()) {
                // dequeue packet
                cPacket *pkt = (cPacket*) bufferData.pop();
                if(canSend){
                    // send packet
                    send(pkt, "toOut$o");
                }
                // start new service
                serviceTime = pkt->getDuration();
                if (!endServiceEvent->isScheduled()) {
                    scheduleAt(simTime() + serviceTime, endServiceEvent);

                }
            }

        } else {
            handlePkt(msg);
        }
    }
}
#endif /* TRANSPORT_TX */
























