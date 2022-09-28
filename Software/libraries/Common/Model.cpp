#include "Model.h"

Model* Model::s_pInstance = NULL;

// ----------------------------------------------------------------------------

Model::Model()
{
    Initialize();
}

//-----------------------------------------------------------------------------

void Model::Initialize()
{
    m_uiLoopCnt = 0;
}

//-----------------------------------------------------------------------------

Model* Model::GetInstance()
{
    if(s_pInstance==NULL)
            s_pInstance = new Model();

    return s_pInstance;
}

// -------------------------------------------------------------------------

void Model::AddToTxQueue(xplane::UdpDatagram data)
{
    m_qTX.push(data);
}

// -------------------------------------------------------------------------

xplane::UdpDatagram Model::GetFromTxQueue()
{
    xplane::UdpDatagram sDatagram = m_qTX.front();
    m_qTX.pop();

    return sDatagram;
}

// -------------------------------------------------------------------------
