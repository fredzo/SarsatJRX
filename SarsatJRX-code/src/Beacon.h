#ifndef BEACON_H
#define BEACON_H

#include <Arduino.h>
#include <Country.h>
#include <Location.h>

// Enable BCH debuging
// #define DEBUG_BCH

class Beacon
{
    public:
        enum class FrameMode { NORMAL, SELF_TEST, UNKNOWN };
        class Protocol
        {
            public :
            enum class Type { STANDARD_LOCATION, NATIONAL_LOCATION, RLS_LOCATION, ELT_DT_LOCATION, USER, SPARE, UNKNOWN };
            Type type;
            bool isUser() const { return (type == Type::USER); };
            bool isNational() const { return (type == Type::NATIONAL_LOCATION); };
            bool isStandard() const { return (type == Type::STANDARD_LOCATION); };
            bool isRlsOrElt() const { return (type == Type::RLS_LOCATION || type == Type::ELT_DT_LOCATION); };
            bool isUnknown() const { return (type == Type::UNKNOWN); };
            String getTypeName(bool longFrame) const;
            Protocol(Type type) : type(type) {}
            static const Protocol
            // User
            USER_EPIRB_MARITIME, USER_EPIRB_RADIO, USER_ELT, USER_SERIAL, USER_TEST, USER_ORB, USER_NAT, USER_2G,
            // Standard
            STD_EPIRB, STD_ELT_24, STD_ELT_SERIAL, STD_ELT_AIRCRAFT, STD_EPIRB_SERIAL, STD_PLB_SERIAL, STD_SHIP, STD_TEST,
            // National
            NAT_ELT, NAT_EPIRB, NAT_PLB, NAT_TEST,
            // RLS / ELT / Spare
            RLS, ELT_DT, SPARE, UNKNOWN;
        };
        static const byte SIZE = 18;
        bool longFrame;
        FrameMode frameMode;
        long protocolCode;
        const Protocol* protocol = &Protocol::UNKNOWN;
        Country country;
        byte frame[SIZE];
        Location location;
        uint64_t identifier;
        Beacon(byte frameBuffer[]);
        String getProtocolName();
        String getProtocolDesciption();
        uint32_t bch1;
        uint32_t computedBch1;
        bool isBch1Valid();
        uint32_t bch2;
        uint32_t computedBch2;
        bool isBch2Valid();
    private:
        void parseFrame();
        void parseProtocol();
};
#endif 