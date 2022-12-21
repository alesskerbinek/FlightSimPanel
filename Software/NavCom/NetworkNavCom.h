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
    //! Parses DREF message. Reimplement in inherited class.
    virtual void ParseDREF(uint8_t* pBuffer) override;

    //! Parses COM1 & COM2 frequency DATA message.
    virtual void ParseCOM(uint8_t* pBuffer) override;

    //! Parses NAV1 & NAV2 frequency DATA message.
    virtual void ParseNAV(uint8_t* pBuffer) override;

    //! Parses NAV1 & NAV2 OBS DATA message.
    virtual void ParseOBS(uint8_t* pBuffer) override;

    //! Parses TRANSPONDER DATA message.
    virtual void ParseXPDR(uint8_t* pBuffer) override;

    //! Parses ADF DATA message.
    virtual void ParseADF(uint8_t* pBuffer) override;

    /**
     * @brief Function checks whether there are messages in Tx queue that need to be sent.
     * Sends the oldest datagram in the queue (FIFO).
     */
    virtual void ProcessTxQueue() override;

private:
    //! Model pointer;
    ModelNavCom* m_pModel;

};

#endif // NETWORKNAVCOM_H
