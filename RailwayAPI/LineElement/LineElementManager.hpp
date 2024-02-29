//
//  LineElementManager.hpp
//  RailwayAPI
//
//  Created by 易傲飞 on 2023/10/27.
//

#ifndef LineElementManager_hpp
#define LineElementManager_hpp

#include "BaseLineElement.hpp"
#include "StraightLineElement.hpp"
#include "CurveElement.hpp"
#include "ArcElement.hpp"
#include "BaseStruct.hpp"

/// 线元管理类
class LineElementManager
{
public:
    LineElementManager();
    
    virtual ~LineElementManager();
    
protected:
    ///线元数组
    BaseLineElement** m_arrLineElement;
    ///线元数目
    uint32_t m_nElementCount;
    //交点数组
    tagJDInfo* m_arrJD;
    //交点数目
    uint32_t m_nJDCount;
    
private:
    /// 切线长度
    /// - Parameters:
    ///   - dFL: 前缓和曲线长度
    ///   - dBL: 后缓和曲线长度
    ///   - dArcR: 圆曲线半径
    ///   - dTurnAngle: 转向角度
    ///   - bFront: 前后标识
    double CalTangentLen(double dFL, double dBL, double dArcR, double dTurnAngle, bool bFront);
    
    /// 计算缓和曲线属性点
    /// - Parameters:
    ///   - dL0: 缓和曲线长度
    ///   - dArcR: 圆曲线半径
    Point2d CalCurvePos(double dL0, double dArcR);
    
    /// 重置数据
    void ResetData();
    
private:
    ///计算点的投影属于哪一线元
    virtual BaseLineElement* PosBelongTo(Point2d pos) = 0;
    
    ///计算里程属于哪一线元
    virtual BaseLineElement* CmlBelongTo(double dCml) = 0;
    
public:
    /// 设置数据
    /// - Parameters:
    ///   - pJDInfo: 交点数组
    ///   - iCount: 数组大小
    void SetJDData(const tagJDInfo* pJDInfo, uint32_t iCount);
    
    
    /// 更新交点坐标
    /// - Parameters:
    ///   - nIndex: 交点索引
    ///   - dX: X坐标
    ///   - dY: Y坐标
    bool UpdateJD(const int& nIndex, const double& dX, const double& dY);
    
    /// 导出指定里程范围内平曲线数据
    /// - Parameters:
    ///   - nArrCount: 数组大小
    ///   - dStartCml: 起点里程
    ///   - dEndCml: 终点里程
    ///   - dDist: 偏移距离
    ///   - dCurveStep: 缓和曲线步长
    ///   - return: 线元数组首地址
    tagExportLineElement* ExportHorCurve(int& nArrCount, double dStartCml, double dEndCml, double dDist, double dCurveStep);
    
    /// 导出交点信息
    /// - Parameter nCount: 交点数目
    const tagJDInfo* ExportJDInfo(int& nCount);
};

#endif /* LineElementManager_hpp */
