#ifndef TRANSPORT_RX
#define TRANSPORT_RX

#include <omnetpp.h>
#include <string.h>

#include "packet_m.h"

// Porcentaje del buffer a partir del cual consideramos saturado al receptor
#define CAP 0.77        // random entre 0.7 y 0.8



using namespace omnetpp;

class TransportRx: public cSimpleModule {
private:
    cQueue bufferData;
    cMessage *endServiceEvent;

    cQueue bufferFeedback;
    cMessage *feedbackEvent;

    simtime_t serviceTime;

    cOutVector bufferSizeVector;
    cOutVector packetDropVector;
    double packetDrop;
    double feedbackCount;

public:
    TransportRx();
    virtual ~TransportRx();

protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);

    // Nuestra implementacion (agregados)
    virtual void sendFeedback();
    virtual void handleData(cMessage *pkt);
    virtual void handleEndService();
    virtual void handleFeedback();
};

Define_Module(TransportRx);

TransportRx::TransportRx() {
    endServiceEvent = NULL;
    feedbackEvent = NULL;
}

TransportRx::~TransportRx() {
    cancelAndDelete(endServiceEvent);
}

void TransportRx::initialize() {
    bufferData.setName("bufferData");
    bufferFeedback.setName("bufferFeedback");

    endServiceEvent = new cMessage("endService");
    feedbackEvent  = new cMessage("feedback");

    packetDropVector.setName("packetDropVector");
    bufferSizeVector.setName("bufferSizeVector");
    packetDrop = 0;
    feedbackCount = 0;

}

void TransportRx::finish() {
    recordScalar("Number of data packets dropped", packetDrop);
    recordScalar("Number of feedback packets lost", feedbackCount);
}

void TransportRx::sendFeedback() {
    // Creamos el pkt
    FeedbackPkt * pkt = new FeedbackPkt();
    pkt->setKind(2);
    pkt->setByteLength(20);

    // Lo metemos en la cola
    bufferFeedback.insert(pkt);
    if (!feedbackEvent->isScheduled()) {
        scheduleAt(simTime() + 0, feedbackEvent);
    }
}

void TransportRx::handleData(cMessage *pkt) {

    // Si no entra en el buffer lo descartamos
    if (bufferData.getLength() >= par("bufferSize").intValue()) {
        delete pkt;
        this->bubble("packet dropped");
        packetDrop++;
        packetDropVector.record(packetDrop);
    }

    //  Si entra en el buffer lo procesamos
    else {

        // avisarle al que me mando si estoy saturado
        if (bufferData.getLength() >= par("bufferSize").intValue() * CAP)
            sendFeedback();

        bufferData.insert(pkt);
        if (!endServiceEvent->isScheduled()) {
            scheduleAt(simTime() + 0, endServiceEvent);
        }

    }
}

void TransportRx::handleEndService() {
    // if packet in buffer, send next one
    if (!bufferData.isEmpty()) {
        // dequeue packet
        cPacket *pkt = (cPacket*) bufferData.pop();
        // send packet
        send(pkt, "toApp");
        // start new service
        serviceTime = pkt->getDuration();
        scheduleAt(simTime() + serviceTime, endServiceEvent);
    }
}


void TransportRx::handleFeedback() {
    // if packet in buffer, send next one
    if (!bufferFeedback.isEmpty()) {
        // dequeue packet
        FeedbackPkt *pkt = (FeedbackPkt*) bufferFeedback.pop();
        // send packet
        send(pkt, "toOut$o");
        EV << "Sending feedback\n";
        feedbackCount++;
        // start new service
        serviceTime = pkt->getDuration();
        scheduleAt(simTime() + serviceTime, feedbackEvent);
    }
}


void TransportRx::handleMessage(cMessage *msg) {
    bufferSizeVector.record(bufferData.getLength());
    if (msg == endServiceEvent) {
        handleEndService();
    }
    else if (msg == feedbackEvent) {
        handleFeedback();
    }
    else {
        handleData(msg);
    }
}


#endif /* TRANSPORT_TX */






































