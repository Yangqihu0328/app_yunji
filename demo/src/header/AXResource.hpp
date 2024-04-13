/**************************************************************************************************
 *
 * Copyright (c) 2019-2023 Axera Semiconductor (Shanghai) Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor (Shanghai) Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor (Shanghai) Co., Ltd.
 *
 **************************************************************************************************/

#pragma once
#include <mutex>
#include <unordered_map>

/**
 * @brief manage resource
 *    1. all objects will be destoryed when CAXResource dtor
 */
template <typename T>
class CAXResource {
public:
    CAXResource(void) noexcept = default;
    ~CAXResource(void) {
        destory();
    }

    /* return the total count including available and borrowed resource */
    std::size_t capacity(void) const {
        std::lock_guard<std::mutex> lck(m_mtx);
        return m_map.size();
    }

    /* return the available resource count */
    std::size_t size(void) const {
        std::lock_guard<std::mutex> lck(m_mtx);
        std::size_t sz = {0};
        for (auto &&kv : m_map) {
            if (kv.second == STATE::AVAILABLE) {
                ++sz;
            }
        }

        return sz;
    }

    /* pre-alloc resources */
    template <typename... Args>
    void reserve(std::size_t sz, Args &&...args) {
        std::lock_guard<std::mutex> lck(m_mtx);
        for (std::size_t i = 0; i < sz; ++i) {
            //这个new出来资源
            T *p = new T(std::forward<Args>(args)...);
            m_map[p] = STATE::AVAILABLE;
        }
    }

    //可接受多个参数
    //&& 是右值引用的语法，表示引用一个右值，不会进行内存拷贝的临时对象
    //也就是必须传过来的是右值，不然出现编译报错
    template <typename... Args>
    void insert(Args &&...args) {
        T *p = new T(std::forward<Args>(args)...);
        std::lock_guard<std::mutex> lck(m_mtx);
        m_map[p] = STATE::AVAILABLE;
    }

    void remove(T *p) {
        std::lock_guard<std::mutex> lck(m_mtx);
        auto it = m_map.find(p);
        if (m_map.end() != it) {
            m_map.erase(it);
            delete p;
        }
    }

    /* borrow available resource, marked as BORROWED */
    template <typename... Args>
    T *borrow(Args &&...args) {
        std::lock_guard<std::mutex> lck(m_mtx);
        for (auto &&kv : m_map) {
            //可用改为借用的，再返回第一个值
            if (kv.second == STATE::AVAILABLE) {
                kv.second = STATE::BORROWED;
                return kv.first;
            }
        }

        /* if no available resource, alloc one and marked as BORROWED */
        //如果找不到可用资源，才进行重新分配
        T *p = new T(std::forward<Args>(args)...);
        m_map[p] = STATE::BORROWED;
        return p;
    }

    /* give back resource */
    void giveback(T *p) {
        std::lock_guard<std::mutex> lck(m_mtx);
        auto it = m_map.find(p);
        //找到对应，设置为可用
        if (m_map.end() != it) {
            m_map[p] = STATE::AVAILABLE;
        }
    }

    void destory(void) {
        std::lock_guard<std::mutex> lck(m_mtx);
        for (auto &&kv : m_map) {
            //类型指针释放
            if (kv.first) {
                delete kv.first;
            }
        }

        m_map.clear();
    }

private:
    CAXResource(const CAXResource &rhs) = delete;
    CAXResource &operator=(const CAXResource &rhs) = delete;

private:
    enum class STATE { AVAILABLE = 0, BORROWED = 1 };
    std::unordered_map<T *, STATE> m_map;
    mutable std::mutex m_mtx;
};
