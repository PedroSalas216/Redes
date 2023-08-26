#ifndef QUEUE
#define QUEUE

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Queue: public cSimpleModule {
private:
    cQueue buffer;
    cMessage *endServiceEvent;
    simtime_t serviceTime;

    cOutVector bufferSizeVector;
    cOutVector packetDropVector;

    double packetDrop;

public:
    Queue();
    virtual ~Queue();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Queue);

Queue::Queue() {
    endServiceEvent = NULL;
}

Queue::~Queue() {
    cancelAndDelete(endServiceEvent);
}

void Queue::initialize() {
    buffer.setName("buffer");
    endServiceEvent = new cMessage("endService");
    packetDropVector.setName("packetDropVector");
    bufferSizeVector.setName("bufferSizeVector");
    packetDrop = 0;

}

void Queue::finish() {
    recordScalar("Number of packets lost", packetDrop);
}

void Queue::handleMessage(cMessage *msg) {

    // if msg is signaling an endServiceEvent
    if (msg == endServiceEvent) {
        // if packet in buffer, send next one
        if (!buffer.isEmpty()) {
            // dequeue packet
            cPacket *pkt = (cPacket*) buffer.pop();
            // send packet
            send(pkt, "out");
            // start new service
            serviceTime = pkt->getDuration();
            scheduleAt(simTime() + serviceTime, endServiceEvent);
        }
    } else { // if msg is a data packet
        if (buffer.getLength() >= par("bufferSize").intValue()) {

            delete msg;
            this->bubble("packet dropped");

            packetDrop++;
            packetDropVector.record(packetDrop);
        } else {
            buffer.insert(msg);
            bufferSizeVector.record(buffer.getLength());
            if (!endServiceEvent->isScheduled()) {
                scheduleAt(simTime() + 0, endServiceEvent);
            }
        }
    }
}

#endif /* QUEUE */
