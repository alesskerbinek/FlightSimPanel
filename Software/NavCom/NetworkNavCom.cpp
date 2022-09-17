#include "NetworkNavCom.h"

// -------------------------------------------------------------------------

NetworkNavCom::NetworkNavCom() : Network()
{
    m_pModel = nullptr;
}

// -------------------------------------------------------------------------

void NetworkNavCom::Initialize()
{
    m_pModel = ModelNavCom::GetInstance();

    Network::Initialize();
}

//-----------------------------------------------------------------------------

void NetworkNavCom::GetNetworkParameters(types::NetworkParameters* pParam)
{
    if(m_pModel && pParam) {
//        pParam->m_uiUdpListenPort = m_pModel->GetSettings()->GetListenPort();
        pParam->m_uiUdpListenPort = 20229;

        pParam->m_uiUdpRemotePort = m_pModel->GetSettings()->GetRemotePort();
        pParam->m_uiUdpRemoteIP   = m_pModel->GetSettings()->GetRemoteIP();

//        strncpy(pParam->m_acWiFiSSID, m_pModel->GetSettings()->GetWiFiSSID(), sizeof(pParam->m_acWiFiSSID));
//        strncpy(pParam->m_acWiFiPass, m_pModel->GetSettings()->GetWiFiPass(), sizeof(pParam->m_acWiFiPass));
        strncpy(pParam->m_acWiFiSSID, "ZANDAnet", sizeof(pParam->m_acWiFiSSID));
        strncpy(pParam->m_acWiFiPass, "Kraigherjeva7", sizeof(pParam->m_acWiFiPass));
    }
}

// ----------------------------------------------------------------------------
