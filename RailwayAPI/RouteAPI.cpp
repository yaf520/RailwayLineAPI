//
//  RailwayAPI.cpp
//  RailwayAPI
//
//  Created by 易傲飞 on 2023/11/3.
//

#include <iostream>
#include "RouteAPI.hpp"
#include "HorizontalCurve.hpp"
#include "VerticalCurve.hpp"
#include "MileConvert.hpp"

RouteAPI::RouteAPI()
{
    m_pHorCurve = new (std::nothrow) HorizontalCurve();
    m_pMileConvert = new (std::nothrow) MileConvert();
    m_pVerCurve = new (std::nothrow) VerticalCurve(m_pMileConvert);
}

RouteAPI::~RouteAPI()
{
    if (m_pHorCurve)
        delete m_pHorCurve;
    
    if (m_pVerCurve)
        delete m_pVerCurve;
    
    if (m_pMileConvert)
        delete m_pMileConvert;
}

void RouteAPI::SetData(const tagJDInfo* pJDInfo, uint32_t jdCount, const tagDLInfo* pDLCount, uint32_t dlCount, const tagSlopeInfo* pSlopeInfo, uint32_t slopeCount)
{
    m_pHorCurve->SetJDData(pJDInfo, jdCount);
    m_pMileConvert->SetDLData(pDLCount, dlCount);
    m_pVerCurve->SetSlopeData(pSlopeInfo, slopeCount);
}

void RouteAPI::UpdateJD(int nIndex, const Vector2d& vecOffset)
{
    m_pHorCurve->UpdateJDCoordinates(nIndex, vecOffset);
}

bool RouteAPI::TrsCmlDistToNE(double dCml, double dDist, double& N_Y, double& E_X, double& dFwj) const
{
    double dTanAngle = 0.0;
    if (!m_pHorCurve->TrsCmlDistToNE(dCml, -dDist, E_X, N_Y, dTanAngle))
        return false;
    dFwj = BaseCalFun::TransferAngle(dTanAngle);
    
    return true;
}

bool RouteAPI::TrsNEToCmlDist(double N_Y, double E_X, double& dCml, double& dDist, double& dFwj) const
{
    if (!m_pHorCurve->TrsNEToCmlDist(E_X, N_Y, dCml, dDist, dFwj))
        return false;
    
    dDist = -dDist;
    dFwj = BaseCalFun::TransferAngle(dFwj);
    
    return true;
}

tagCmlDistAngle* RouteAPI::TrsNEToCmlDist(double N_Y, double E_X, uint32_t& nCount) const
{
    tagCmlDistAngle* arrRet = m_pHorCurve->TrsNEToCmlDist(E_X, N_Y, nCount);
    for (int i = 0; i < nCount; i++)
    {
        (arrRet + i)->dDist *= -1.0;
        (arrRet + i)->dAngle = BaseCalFun::TransferAngle((arrRet + i)->dAngle);
    }
    return arrRet;
}

Point2d* RouteAPI::IntersectWithLine(double dAngle, double N_Y, double E_X, uint32_t& nArrCount) const
{
    return m_pHorCurve->IntersectWithLine(dAngle, E_X, N_Y , nArrCount);
}

double RouteAPI::GetLength()
{
    return m_pHorCurve->GetLength();
}

///连续里程->现场里程
bool RouteAPI::TrsCmltoCkml(double cml, char ckml[64], int nPrec) const
{
    return m_pMileConvert->TrsCmltoCkml(cml, ckml, nPrec);
}

bool RouteAPI::TrsCkmlToCml(char ckml[64], double& cml, char strErr[64]) const
{
    return m_pMileConvert->TrsCkmlToCml(ckml, cml, strErr);
}

void RouteAPI::GetDesignHeight(double dCml, double& dHZ, double& dFyj) const
{
    m_pVerCurve->TrsCmlToHeight(dCml, dHZ, dFyj);
}

tagExportLineElement* RouteAPI::ExportHorCurve(int& nArrCount, double dStartCml, double dEndCml, double dDist, double dCurveStep) const
{
    if (!m_pHorCurve)
    {
        nArrCount = 0;
        return nullptr;
    }
    return m_pHorCurve->ExportHorCurve(nArrCount, dStartCml, dEndCml, dDist, dCurveStep);
}

const tagJDInfo* RouteAPI::ExportJDInfo(int& nCount) const
{
    return m_pHorCurve->ExportJDInfo(nCount);
}
