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
#include "SpiralLineElement.hpp"
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
    /// 重置数据
    void ResetData();
    
    /// 计算单元曲线起点
    /// - Parameters:
    ///   - nIndex: 单元交点索引
    ///   - vecWhole: 曲线起始向量
    Point2d CalUnitStartPos(uint32_t nIndex, const Vector2d& vecWhole);
    
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
    void SetJDData(const tagJDInfo* pJDInfo, uint32_t nCount);
    
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
