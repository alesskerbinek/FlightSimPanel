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

private:
    //! Model pointer;
    ModelNavCom* m_pModel;

};

#endif // NETWORKNAVCOM_H
