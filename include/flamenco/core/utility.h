/*
 libflamenco - lightweight and efficient software sound mixing library.
 (c) Sergei Cherepanov, 2008. Licensed under GPL license.
 
 Вспомогательные классы.
 */
#pragma once
#ifndef _FLAMENCO_CORE_UTILITY_H_
#define _FLAMENCO_CORE_UTILITY_H_

#include <cassert>
#include <stdexcept>

namespace flamenco
{

// Идиома "noncopyable".
class noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    noncopyable( const noncopyable & );
    noncopyable & operator =( const noncopyable & );
};

// Класс со встроенным счетчиком ссылок.
class refcountable : noncopyable
{
protected:
    refcountable()
        : mRefCount(0), mIsOk(MAGIC)
        {}
    virtual ~refcountable() = 0
    {
        assert(isOk());
        mIsOk = 0xdeadbeef; // Затираем метку.
        if (0 != mRefCount)
            throw std::logic_error("deleted refcountable has dangling references");
    }
    
protected:
    inline bool isOk() const
    {
        return MAGIC == mIsOk;
    }
    inline s32 refcount() const
    {
        return mRefCount;
    }
    
private:
    s32 mRefCount;
    u32 mIsOk;
    
    // Магическое число для проверки на затирание памяти.
    static const u32 MAGIC = 0xacceded;
    
    inline s32 incRef()
    {
        return ++mRefCount;
    }
    inline s32 decRef()
    {
        assert(mRefCount > 0);
        return --mRefCount;
    }
    template <class Refcountable> friend class reference;
};

// Умный указатель на refcountable.
template<class Refcountable>
class reference
{
public:
    explicit reference( Refcountable * object = NULL )
        : mObject(object)
    {
        if (NULL != object)
        {
            assert(object->isOk());
            assert(0 == object->refcount() && "Initializing with refcountable with dangling references");
            object->incRef();
        }
    }
    
    template <class Y>
    reference( reference<Y> ref )
        : mObject(&*ref)
    {
        incRef();
    }
    
    reference & operator =( const reference & ref )
    {
        if (this != &ref)
        {
            decRef();
            mObject = ref.mObject;
            incRef();
        }
        return *this;
    }
    
    ~reference()
    {
        decRef();
    }
    
    operator bool() const
    {
        return NULL != mObject;
    }
    
    const Refcountable * operator ->() const
    {
        assert(NULL != mObject);
        assert(mObject->isOk());
        return mObject;
    }
    Refcountable * operator ->()
    {
        return const_cast<Refcountable *>(const_cast<const reference *>(this)->operator ->());
    }
    const Refcountable & operator *() const
    {
        assert(NULL != mObject);
        assert(mObject->isOk());
        return *mObject;
    }
    Refcountable & operator *()
    {
        return const_cast<Refcountable &>(const_cast<const reference *>(this)->operator *());
    }
    
    bool unique() const
    {
        return NULL != mObject && 1 == mObject->refcount();
    }
    
    bool operator ==( const reference & ref ) const
    {
        return mObject == ref.mObject;
    }
    
private:
    Refcountable * mObject;
    
    // Увеличение счетчика ссылок с проверками.
    void incRef()
    {
        if (NULL != mObject)
        {
            assert(mObject->isOk());
            mObject->incRef();
        }
    }
    
    // Уменьшение счетчика ссылок с корректным удалением.
    void decRef()
    {
        if (NULL == mObject)
            return;
        assert(mObject->isOk());
        if (0 == mObject->decRef())
        {
            delete mObject;
            mObject = NULL;
        }
    }
};

} // namespace flamenco

#endif // _FLAMENCO_CORE_UTILITY_H_
