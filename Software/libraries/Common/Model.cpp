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

