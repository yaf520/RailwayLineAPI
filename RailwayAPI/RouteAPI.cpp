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

using namespace std;

RouteAPI::RouteAPI()
{
    m_pHorCurve = new (nothrow) HorizontalCurve();
    m_pMileConvert = new (nothrow) MileConvert();
    m_pVerCurve = new (nothrow) VerticalCurve(m_pMileConvert);
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

bool RouteAPI::TrsCmlDistToNE(const double& dCml, const double& dDist, double& N_Y, double& E_X, double& dFwj)
{
    double dTanAngle = 0.0;
    if (!m_pHorCurve->TrsCmlDistToNE(dCml, -dDist, E_X, N_Y, dTanAngle))
        return false;
    dTanAngle = MATH_PI_2 - dTanAngle;
    BaseCalFun::KeepAngleIn2PI(dTanAngle);
    dFwj = dTanAngle;
    
    return true;
}

bool RouteAPI::TrsNEToCmlDist(const double& N_Y, const double& E_X, double& dCml, double& dDist, double& dFwj)
{
    return m_pHorCurve->TrsNEToCmlDist(E_X, N_Y, dCml, dDist, dFwj);
}

tagCmlDistAngle* RouteAPI::TrsNEToCmlDist(const double& N_Y, const double& E_X, uint32_t& nCount)
{
    return m_pHorCurve->TrsNEToCmlDist(E_X, N_Y, nCount);
}

Point2d* RouteAPI::IntersectWithLine(const double& dAngle, const double& N_Y, const double& E_X, uint32_t& nArrCount)
{
    return m_pHorCurve->IntersectWithLine(dAngle, E_X, N_Y , nArrCount);
}

double RouteAPI::GetLength()
{
    return m_pHorCurve->GetLength();
}

///连续里程->现场里程
bool RouteAPI::TrsCmltoCkml(const double& cml, char ckml[64], int nPrec)
{
    return m_pMileConvert->TrsCmltoCkml(cml, ckml, nPrec);
}

bool RouteAPI::TrsCkmlToCml(char ckml[64], double& cml, char strErr[64])
{
    return m_pMileConvert->TrsCkmlToCml(ckml, cml, strErr);
}

void RouteAPI::GetDesignHeight(double dCml, double& dHZ, double& dFyj)
{
    m_pVerCurve->TrsCmlToHeight(dCml, dHZ, dFyj);
}

tagExportLineElement* RouteAPI::ExportHorCurve(int& nArrCount, double dStartCml, double dEndCml, double dDist, double dCurveStep)
{
    if (!m_pHorCurve)
    {
        nArrCount = 0;
        return nullptr;
    }
    return m_pHorCurve->ExportHorCurve(nArrCount, dStartCml, dEndCml, dDist, dCurveStep);
}

const tagJDInfo* RouteAPI::ExportJDInfo(int& nCount)
{
    return m_pHorCurve->ExportJDInfo(nCount);
}
