#ifndef CONFIGPORTNAVCOM_H_
#define CONFIGPORTNAVCOM_H_

#include <stdint.h>
#include <stddef.h>
#include "ModelNavCom.h"
#include "ConfigPort.h"

class ConfigPortNavCom : public ConfigPort
{
public:
	//! Constructor
    ConfigPortNavCom();
	
	//! Destructor
    virtual ~ConfigPortNavCom() {}

    //! Initialization of members not available during construction.
    virtual void Initialize() override;

    //! Main state machine loop.
    virtual void Process() override;
	
protected:
    //! Returns serial number, type etc.
    virtual void GetProductionParameters(types::ProductionParameters* sParam) override;

    //! Sets serial number, type etc.
    virtual bool SetProductionParameters(types::ProductionParameters* sParam) override;

    //! Returns system parameters
    virtual void GetSystemParameters(types::SystemParameters* pParam) override;

    //! Performs Factory Reset
    virtual bool FactoryReset() override;

    //! Returns ssid, passphrase, ip, ports, etc.
    virtual void GetNetworkParameters(types::NetworkParameters* sParam) override;

    //! Sets ssid, passphrase, ip, ports, etc.
    virtual bool SetNetworkParameters(types::NetworkParameters* sParam) override;

protected:
    //! Model pointer;
    ModelNavCom* m_pModel;
};

#endif /* CONFIGNAVCOM_H_ */
