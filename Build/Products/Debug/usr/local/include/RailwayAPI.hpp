//
//  RailwayAPI.hpp
//  RailwayAPI
//
//  Created by 易傲飞 on 2023/11/3.
//

#ifndef RailwayAPI_
#define RailwayAPI_

#include "BaseStruct.hpp"

///平曲线
class HorizontalCurve;

///竖曲线
class VerticalCurve;

/* The classes below are exported */
#pragma GCC visibility push(default)

class RailwayAPI
{ 
public:
    RailwayAPI();
    virtual ~RailwayAPI();
    
private:
    ///平曲线对象指针
    HorizontalCurve* m_pHorCurve;
    
    ///竖曲线对象指针
    VerticalCurve* m_pVerCurve;
    
public:
    /// 设置数据
    /// - Parameters:
    ///   - pJDInfo: 交点信息数组
    ///   - jdCount: 交点数组大小
    ///   - pDLCount: 断链信息数组
    ///   - dlCount: 断链数组大小
    ///   - pSlopeInfo: 坡度信息数组
    ///   - slopeCount: 坡度数组大小
    void SetData(const tagJDInfo* pJDInfo, uint32_t jdCount, const tagDLInfo* pDLCount, uint32_t dlCount, const tagSlopeInfo* pSlopeInfo, uint32_t slopeCount);
    
    /// 连续里程+投影距离 -> 坐标+方位角
    /// - Parameters:
    ///   - dCml: 连续里程
    ///   - dDist: 投影距离(左负右正)
    ///   - N_Y: 纬距
    ///   - E_X: 经距
    ///   - dFwj: 方位角
    bool TrsCmlDistToNE(const double& dCml, const double& dDist, double& N_Y, double& E_X, double& dFwj);
    
    /// 坐标 -> 连续里程+投影距离+方位角
    /// - Parameters:
    ///   - N_Y: 纬距
    ///   - E_X: 经距
    ///   - dCml: 连续里程
    ///   - dDist: 投影距离(左负右正)
    ///   - dFwj: 方位角
    bool TrsNEToCmlDist(const double& N_Y, const double& E_X, double& dCml, double& dDist, double& dFwj);
    
    /// 线路总长度
    double GetLength();
    
    /// 连续里程->现场里程
    /// - Parameter cml: 连续里程
    bool TrsCmltoCkml(const double& cml, char ckml[64], int nPrec = 3);
    
    /// 现场里程->连续里程
    /// - Parameters:
    ///   - cml: 连续里程
    ///   - bReliability: 是否可靠
    bool TrsCkmlToCml(char ckml[64], double& cml, char strErr[64]);
    
    /// 连续里程->轨面高程
    /// - Parameters:
    ///   - dCml: 连续里程
    ///   - dHZ: 轨面高程
    ///   - dFyj: 俯仰角
    void GetDesignHeight(double dCml, double& dHZ, double& dFyj);
};

#pragma GCC visibility pop
#endif