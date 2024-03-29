//
// Generated file, do not edit! Created by nedtool 5.6 from Airplane.msg.
//

#ifndef __AIRPLANE_M_H
#define __AIRPLANE_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>Airplane.msg:19</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * message Airplane
 * {
 *     int id;
 *     double landingTime;
 *     double parkingAreaTime;
 *     double takeOffTime;
 *     double timeInsertedLQ;
 *     double timeInsertedTQ;
 *     double timeOfArrival;
 * }
 * </pre>
 */
class Airplane : public ::omnetpp::cMessage
{
  protected:
    int id;
    double landingTime;
    double parkingAreaTime;
    double takeOffTime;
    double timeInsertedLQ;
    double timeInsertedTQ;
    double timeOfArrival;

  private:
    void copy(const Airplane& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Airplane&);

  public:
    Airplane(const char *name=nullptr, short kind=0);
    Airplane(const Airplane& other);
    virtual ~Airplane();
    Airplane& operator=(const Airplane& other);
    virtual Airplane *dup() const override {return new Airplane(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual int getId() const;
    virtual void setId(int id);
    virtual double getLandingTime() const;
    virtual void setLandingTime(double landingTime);
    virtual double getParkingAreaTime() const;
    virtual void setParkingAreaTime(double parkingAreaTime);
    virtual double getTakeOffTime() const;
    virtual void setTakeOffTime(double takeOffTime);
    virtual double getTimeInsertedLQ() const;
    virtual void setTimeInsertedLQ(double timeInsertedLQ);
    virtual double getTimeInsertedTQ() const;
    virtual void setTimeInsertedTQ(double timeInsertedTQ);
    virtual double getTimeOfArrival() const;
    virtual void setTimeOfArrival(double timeOfArrival);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Airplane& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Airplane& obj) {obj.parsimUnpack(b);}


#endif // ifndef __AIRPLANE_M_H

