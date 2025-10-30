//
//  Array.hpp
//
//  Created by 易傲飞 on 2025/1/3.
//

#ifndef Array_hpp
#define Array_hpp

#include <assert.h>
#include <new>

#pragma pack (push, 8)
#pragma push_macro("new")
#pragma push_macro("delete")
#undef new
#undef delete

#define ARRAY_GROWTH_THRESHOLD 0x10000      //最大增长字节
#define ARRAY_GROWTH_MIN_COUNT 8            //最小增长数目

template <class Type>
class DyArray
{
private:
    int m_nMaxCount;            //缓冲数目
    int m_nElementCount;        //元素数目
    Type* m_pData;              //缓存指针
    
public:
    /// 构造函数
    DyArray(int nCount = 0);
    /// 拷贝构造函数
    DyArray(const DyArray<Type>&);
    /// 移动构造函数
    DyArray(DyArray<Type>&&) noexcept;
    /// 析构函数
    ~DyArray();
    
    /// 移动赋值符
    DyArray<Type>& operator = (DyArray<Type>&&) noexcept;
    /// 拷贝赋值符
    DyArray<Type>& operator = (const DyArray<Type>&);
    
    Type& operator[] (int);
    const Type& operator[] (int) const;
    
public:
    /// 获取当前元素数目
    inline int GetCount() const {
        return m_nElementCount;
    }
    
public:
    /// 重置
    void Reset();
    
    /// 添加元素
    /// - Parameter item: 新元素
    int Add(const Type& item);
    
    /// 设置元素数目
    /// - Parameter nNewSize: 元素数目
    void SetElementCount(int nNewSize);
    
    /// 移除所有元素
    void RemoveAll();
    
    /// 移除指定位置指定数目元素
    /// - Parameter nIndex: 移除数目
    void RemoveAt(int nIndex, int nCount = 1);
    
    /// 在指定位置插入元素
    /// - Parameters:
    ///   - nIndex: 插入索引
    ///   - item: 新元素
    void InsertAt(int nIndex, const Type& item, int nCount = 1);
    
    /// 获取指定位置元素
    /// - Parameter nIndex: 索引
    Type& GetAt(int nIndex);
    
    /// 获取指定位置元素
    /// - Parameter nIndex: 索引
    const Type& GetAt(int nIndex) const;
    
    Type* begin() { return m_pData;}
    Type* end() { return m_pData + m_nElementCount;}
    
    const Type* begin() const { return m_pData;}
    const Type* end() const { return m_pData + m_nElementCount;}

public:
    /// 申请内存
    /// - Parameter nNewCount: 元素数目
    void AllocMemory(int nNewCount);
};

template <class T>
DyArray<T>::DyArray(int nCount)
    :m_pData(nullptr),
    m_nMaxCount(0),
    m_nElementCount(0)
{
    if (nCount > 0)
        SetElementCount(nCount);
}

template <class T>
DyArray<T>::DyArray(const DyArray<T>& src)
    :m_pData(nullptr),
    m_nMaxCount(0),
    m_nElementCount(0)
{
    SetElementCount(src.m_nElementCount);
    for (int i = 0; i < m_nElementCount; i++)
        m_pData[i] = src[i];
}

template <class T>
DyArray<T>::DyArray(DyArray<T>&& src) noexcept
    :m_pData(nullptr),
    m_nMaxCount(0),
    m_nElementCount(0)
{
    m_pData = src.m_pData;
    m_nMaxCount = src.m_nMaxCount;
    m_nElementCount = src.m_nElementCount;
        
    src.m_pData = nullptr;
    src.m_nMaxCount = 0;
    src.m_nElementCount = 0;
}

template <class T>
DyArray<T>::~DyArray()
{
    Reset();
}

template <class T>
void DyArray<T>::Reset()
{
    if (m_pData)
    {
        for (int i = 0; i < m_nElementCount; i++)
            (m_pData + i)->~T();
        
        ::operator delete ((void*)m_pData);
        m_pData = nullptr;
        
        m_nMaxCount = 0;
        m_nElementCount = 0;
    }
}

template <class T>
int DyArray<T>::Add(const T &item)
{
    int nIndex = m_nElementCount;
    InsertAt(nIndex, item);
    return nIndex;
}

template <class T>
void DyArray<T>::SetElementCount(int nNewSize)
{
    if (nNewSize <= 0)
        return;
    
    AllocMemory(nNewSize);
    if (nNewSize > m_nElementCount)
    {
        for (int i = m_nElementCount; i < nNewSize; i++)
            new ((void*)(m_pData + i)) T();
    }
    else if (nNewSize < m_nElementCount)
    {
        for (int i = nNewSize; i < m_nElementCount; i++)
            (m_pData + i)->~T();
        memset((void*)(m_pData + nNewSize), 0, m_nElementCount - nNewSize);
    }
    
    m_nElementCount = nNewSize;
}

template <class T>
void DyArray<T>::InsertAt(int nIndex, const T& item, int nCount)
{
    if (nIndex < m_nElementCount)
    {
        int nOldCount = m_nElementCount;
        SetElementCount(nCount + m_nElementCount);
        
        memmove((void*)(m_pData + nIndex + nCount), (void*)(m_pData + nIndex), sizeof(T) * (nOldCount - nIndex));
        memset((void*)(m_pData + nIndex), 0, sizeof(T) * nCount);
//        for (int i = 0; i < nCount; i++)
//            new ((void*)(m_pData + nIndex + i)) T();
    }
    else
        SetElementCount(nIndex + nCount);
    
    for (int i = 0; i < nCount; i++)
        m_pData[nIndex + i] = item;
}

template <class T>
void DyArray<T>::RemoveAll()
{
    if (m_pData)
    {
        for (int i = 0; i < m_nElementCount; i++)
            (m_pData + i)->~T();
        
        memset((void*)m_pData, 0, sizeof(T) * m_nElementCount);
        m_nElementCount = 0;
    }
}

template <class T>
void DyArray<T>::RemoveAt(int nIndex, int nCount)
{
    assert(nIndex + nCount <= m_nElementCount);
    if (nIndex + nCount > m_nElementCount)
        return;
    
    for (int i = 0; i < nCount; i++)
        (m_pData + nIndex + i)->~T();
    int nMoveCount = m_nElementCount - (nIndex + nCount);
    if (nMoveCount > 0)
        memmove(m_pData + nIndex, m_pData + nIndex + nCount, sizeof(T) * nMoveCount);
    
    m_nElementCount -= nCount;
}

template <class T>
T& DyArray<T>::GetAt(int nIndex)
{
    assert(nIndex >= 0 && nIndex < m_nElementCount);
    return m_pData[nIndex];
}

template <class T>
const T& DyArray<T>::GetAt(int nIndex) const
{
    assert(nIndex >= 0 && nIndex < m_nElementCount);
    return m_pData[nIndex];
}

template <class T>
T& DyArray<T>::operator[] (int nIndex)
{
    return GetAt(nIndex);
}

template <class T>
const T& DyArray<T>::operator[](int nIndex) const
{
    return GetAt(nIndex);
}

template <class T>
DyArray<T>& DyArray<T>::operator = (DyArray<T>&& src) noexcept
{
    if (this != &src)
    {
        Reset();
        
        m_pData = src.m_pData;
        m_nMaxCount = src.m_nMaxCount;
        m_nElementCount = src.m_nElementCount;
        
        src.m_pData = nullptr;
        src.m_nMaxCount = 0;
        src.m_nElementCount = 0;
    }
    
    return *this;
}

template <class T>
DyArray<T>& DyArray<T>::operator = (const DyArray<T>& src)
{
    if (this != &src)
    {
        RemoveAll();
        SetElementCount(src.m_nElementCount);
        
        for (int i = 0; i < m_nElementCount; i++)
            m_pData[i] = src[i];
    }
    return *this;
}

template <class T>
void DyArray<T>::AllocMemory(int nNewCount)
{
    if (nNewCount <= 0)
        return;
    
    if (nNewCount > m_nMaxCount)
    {
        const int nGrowCount = (m_nMaxCount * sizeof(T) < ARRAY_GROWTH_THRESHOLD ? m_nMaxCount : ARRAY_GROWTH_THRESHOLD / sizeof(T));
        int minSize = m_nMaxCount + (nGrowCount < ARRAY_GROWTH_MIN_COUNT ? ARRAY_GROWTH_MIN_COUNT : nGrowCount);
        if (nNewCount > minSize)
            minSize = nNewCount;
        m_nMaxCount = minSize;
        
        T* pNewData = (T*)::operator new(sizeof(T) * m_nMaxCount);
        memcpy((void*)pNewData, (void*)m_pData, sizeof(T) * m_nElementCount);
        memset((void*)(pNewData + m_nElementCount), 0, sizeof(T) * (m_nMaxCount - m_nElementCount));
        
        ::operator delete ((void*)m_pData);
        m_pData = pNewData;
    }
}

#pragma pop_macro("new")
#pragma pop_macro("delete")
#pragma pack (pop)
#endif /* Array_hpp */
