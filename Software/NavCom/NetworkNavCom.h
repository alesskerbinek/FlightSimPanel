#ifndef NETWORKNAVCOM_H
#define NETWORKNAVCOM_H

#include "ModelNavCom.h"
#include "Network.h"

class NetworkNavCom : public Network
{
public:
    //! Public constructor.
    NetworkNavCom();

    //! Public destructor.
    ~NetworkNavCom() {}

    //! Initialization of members not available during construction.
    virtual void Initialize() override;

    //! Returns network parameters.
    virtual void GetNetworkParameters(types::NetworkParameters* pParam) override;

protected:

    //! Parses received UDP datagrams
    //virtual void ParseUDP(AsyncUDPPacket &packet);

    //! Parses COM1 & COM2 frequency
    virtual void ParseCOM(uint8_t* pBuffer);

    //! Parses NAV1 & NAV2 frequency
    virtual void ParseNAV(uint8_t* pBuffer);

    //! Parses NAV1 & NAV2 OBS
    virtual void ParseOBS(uint8_t* pBuffer);

private:
    //! Model pointer;
    ModelNavCom* m_pModel;

};

#endif // NETWORKNAVCOM_H
