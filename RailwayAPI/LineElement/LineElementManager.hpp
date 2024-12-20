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

enum class CurveType
{
    HorizontalCurve = 0,
    VerticalCurve
};

/// 线元管理类
class LineElementManager
{
public:
    LineElementManager(CurveType eCurveType);
    
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
    ///曲线要素数组
    tagCurveElementItem* m_arrCurveElement;
    ///曲线要素数组大小
    uint32_t m_nCurveElementCount;
    ///曲线类别
    CurveType m_eCurveType;
    
private:
    /// 重置数据
    void ResetData();
    
    /// 计算单元曲线起点
    /// - Parameters:
    ///   - nIndex: 单元交点索引
    ///   - vecWhole: 曲线起始向量
    Point2d CalUnitStartPos(uint32_t nIndex, const Vector2d& vecWhole);
    
    /// 计算线元单元
    /// - Parameters:
    ///   - nCurIndex: 当前交点索引
    ///   - arrLineElement: 线元单元数组
    ///   - nCurveElementCount： 线元数组大小
    ///   - return: 单元起点坐标
    Point2d CalculateLineElement(uint32_t nCurIndex, BaseLineElement** arrLineElement, uint8_t& nCurveElementCount);
    
    /// 计算线元单元
    /// - Parameters:
    ///   - nCurIndex: 当前交点索引
    ///   - arrLineElement: 线元单元数组
    ///   - nCurveElementCount： 线元数组大小
    ///   - posCurveStart:线元单元起点
    ///   - dCurrentCml: 当前里程
    void JointLineElement(uint32_t nCurIndex, BaseLineElement** arrLineElement, uint8_t& nCurveElementCount, Point2d posCurveStart, double& dCurrentCml);
    
    /// 计算曲线要素
    /// - Parameters:
    ///   - nCurIndex: 当前交点索引
    ///   - arrLineElement: 线元单元数组
    ///   - nCurveElementCount： 线元数组大小
    void CurveElement(uint32_t nCurIndex, BaseLineElement** arrLineElement, uint8_t nCurveElementCount);
    
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
    
    /// 获取总长度
    double GetLength();
};

#endif /* LineElementManager_hpp */
