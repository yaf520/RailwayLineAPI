//
//  RailwayAPI.cpp
//  RailwayAPI
//
//  Created by 易傲飞 on 2023/11/3.
//

#include <iostream>
#include "RailwayAPI.hpp"
#include "HorizontalCurve.hpp"
#include "VerticalCurve.hpp"

using namespace std;

RailwayAPI::RailwayAPI()
{
    m_pHorCurve = new (nothrow) HorizontalCurve;
    m_pVerCurve = new (nothrow) VerticalCurve(m_pHorCurve);
}
RailwayAPI::~RailwayAPI()
{
    if (m_pHorCurve)
        delete m_pHorCurve;
    
    if (m_pVerCurve)
        delete m_pVerCurve;
}

void RailwayAPI::SetData(const tagJDInfo* pJDInfo, uint32_t jdCount, const tagDLInfo* pDLCount, uint32_t dlCount, const tagSlopeInfo* pSlopeInfo, uint32_t slopeCount)
{
    m_pHorCurve->SetJDData(pJDInfo, jdCount);
    m_pHorCurve->SetDLData(pDLCount, dlCount);
    m_pVerCurve->SetSlopeData(pSlopeInfo, slopeCount);
}

bool RailwayAPI::TrsCmlDistToNE(const double& dCml, const double& dDist, double& N_Y, double& E_X, double& dFwj)
{
    double dTanAngle = 0.0;
    if (!m_pHorCurve->TrsCmlDistToNE(dCml, -dDist, E_X, N_Y, dTanAngle))
        return false;
    dTanAngle = MATH_PI_2 - dTanAngle;
    BaseCalFun::KeepAngleIn2PI(dTanAngle);
    dFwj = dTanAngle;
    
    return true;
}

bool RailwayAPI::TrsNEToCmlDist(const double& N_Y, const double& E_X, double& dCml, double& dDist, double& dFwj)
{
    double dTanAngle = 0.0;
    if (!m_pHorCurve->TrsNEToCmlDist(E_X, N_Y, dCml, dDist, dTanAngle))
        return false;
    
    dDist = -dDist;
    dTanAngle = MATH_PI_2 - dTanAngle;
    BaseCalFun::KeepAngleIn2PI(dTanAngle);
    dFwj = dTanAngle;
    
    return true;
}

double RailwayAPI::GetLength()
{
    return m_pHorCurve->GetLength();
}

///连续里程->现场里程
void RailwayAPI::TrsCmltoCkml(const double& cml, char ckml[64])
{
    m_pHorCurve->TrsCmltoCkml(cml, ckml);
}

void RailwayAPI::TrsCkmlToCml(char ckml[64], double& cml, bool& bReliability, char strErr[64])
{
    m_pHorCurve->TrsCkmlToCml(ckml, cml, bReliability, strErr);
}

void RailwayAPI::GetDesignHeight(double dCml, double& dHZ, double& dFyj)
{
    m_pVerCurve->TrsCmlToHeight(dCml, dHZ, dFyj);
}
