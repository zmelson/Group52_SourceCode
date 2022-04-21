#pragma once
using namespace std;

class flightNode {
public:
    string date;
    int carrierAirline;
    string tailNum;
    int carrierFlight;
    int originAirport;
    int originAirportSeq;
    int originCity;
    string origin;

    int destAirport;
    int destAirportSeq;
    int destCity;
    string dest;

    int estDepart;
    int actDepart;
    int departDelay;

    int arrTime;
    int arrDelay;

    bool operator==(const flightNode& rhs) const{
        if (date != rhs.date || carrierAirline != rhs.carrierAirline
        || tailNum != rhs.tailNum || carrierFlight != rhs.carrierFlight
        || originAirport != rhs.originAirport || originAirportSeq != originAirportSeq
        || originCity != rhs.originCity || origin != rhs.origin
        || destAirport != rhs.destAirport || destAirportSeq != rhs.destAirportSeq
        || destCity != rhs.destCity || dest != rhs.dest
        || estDepart != rhs.estDepart || actDepart != rhs.actDepart
        || departDelay != rhs.departDelay || arrTime != rhs.arrTime
        || arrDelay != rhs.arrDelay)
            return false;
        return true;
    }
};