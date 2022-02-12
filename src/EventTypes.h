/**
 * @file EventTypes.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

#include "Common.h"

// Based on this article
// https://blog.molecular-matters.com/2011/09/19/generic-type-safe-delegates-and-events-in-c/

// Base Delegate template class
template<typename ...params>
class Delegate {};

template<typename T>
class Delegate<T> {
private:

    typedef void* InstancePtr;
    typedef void (*FunctionPtr)(InstancePtr, T);
    typedef std::pair<InstancePtr, FunctionPtr> Stub;

    Stub m_stub;

    template<void (*function)(T)>
    static inline void functionStub (InstancePtr, T arg) {
        return (function)(arg);
    }

    template<class C, void (C::*function)(T)>
    static inline void classMethodStub (InstancePtr instance, T arg) {
        return (static_cast<C*>(instance)->*function)(arg);
    }

public:

    Delegate() : m_stub(nullptr, nullptr) {}

    template<void (*function)(T)>
    void bind () {
        m_stub.first = nullptr;
        m_stub.second = &functionStub<function>;
    }

    template<class C, void (C::*function)(T)>
    void bind (C* instance) {
        m_stub.first = instance;
        m_stub.second = &classMethodStub<C, function>;
    }

    void invoke(T arg) {
        if (m_stub.second == nullptr) {
            std::cerr << "Cannot invoke unbound delegate\n";
            return;
        }
        return m_stub.second(m_stub.first, arg);
    }

    bool operator==(Delegate& other) {
        bool isEqual = (m_stub.first == other.m_stub.first) 
            && (m_stub.second == other.m_stub.second);
        return isEqual;
    }
};

// For two arguments
template<typename Tx, typename Ty>
class Delegate<Tx, Ty> {
private:

    typedef void* InstancePtr;
    typedef void (*FunctionPtr)(InstancePtr, Tx, Ty);
    typedef std::pair<InstancePtr, FunctionPtr> Stub;

    Stub m_stub;

    template<void (*function)(Tx, Ty)>
    static inline void functionStub (InstancePtr, Tx arg0, Ty arg1) {
        return (function)(arg0, arg1);
    }

    template<class C, void (C::*function)(Tx, Ty)>
    static inline void classMethodStub (InstancePtr instance, Tx arg0, Ty arg1) {
        return (static_cast<C*>(instance)->*function)(arg0, arg1);
    }

public:

    Delegate() : m_stub(nullptr, nullptr) {}

    template<void (*function)(Tx, Ty)>
    void bind () {
        m_stub.first = nullptr;
        m_stub.second = &functionStub<function>;
    }

    template<class C, void (C::*function)(Tx, Ty)>
    void bind (C* instance) {
        m_stub.first = instance;
        m_stub.second = &classMethodStub<C, function>;
    }

    void invoke(Tx arg0, Ty arg1) {
        if (m_stub.second == nullptr) {
            std::cerr << "Cannot invoke unbound delegate\n";
            return;
        }
        return m_stub.second(m_stub.first, arg0, arg1);
    }

    bool operator==(Delegate& other) {
        bool isEqual = (m_stub.first == other.m_stub.first) 
            && (m_stub.second == other.m_stub.second);
        return isEqual;
    }
};

// Template specialization for zero arguments
template<>
class Delegate<void> {
private:

    typedef void* InstancePtr;
    typedef void (*FunctionPtr)(InstancePtr);
    typedef std::pair<InstancePtr, FunctionPtr> Stub;

    Stub m_stub;

    template<void (*function)(void)>
    static inline void functionStub (InstancePtr) {
        return (function)();
    }

    template<class C, void (C::*function)(void)>
    static inline void classMethodStub (InstancePtr instance) {
        return (static_cast<C*>(instance)->*function)();
    }

public:

    Delegate() : m_stub(nullptr, nullptr) {}

    template<void (*function)(void)>
    void bind () {
        m_stub.first = nullptr;
        m_stub.second = &functionStub<function>;
    }

    template<class C, void (C::*function)(void)>
    void bind (C* instance) {
        m_stub.first = instance;
        m_stub.second = &classMethodStub<C, function>;
    }

    void invoke() {
        if (m_stub.second == nullptr) {
            std::cerr << "Cannot invoke unbound delegate\n";
            return;
        }
        return m_stub.second(m_stub.first);
    }

    bool operator==(Delegate& other) {
        bool isEqual = (m_stub.first == other.m_stub.first) 
            && (m_stub.second == other.m_stub.second);
        return isEqual;
    }
};


// Subject class (Observer pattern)
// Base Event template class
template<typename ...params>
class Event {};

// One argument
template<typename T>
class Event<T> {
public:
    template<void (*func)(T)>
    void registerHandler() {
        Delegate<T> delegate;
        delegate.bind<func>();
        observers.push_back(delegate);
    }
    
    template<class C, void (C::*func)(T)>
    void registerHandler(C* instance) {
        Delegate<T> delegate;
        delegate.bind<C, func>(instance);
        observers.push_back(delegate);
    }

    template<void (*func)(T)>
    void unregisterHandler() {
        // Create a matching Delegate
        Delegate<T> d;
        d.bind<func>();
        // Iterate through attached function delegates
        for (auto it = observers.begin(); it != observers.end(); it++) {
            // Find one that is equal
            if (*it == d) {
                observers.erase(it); // Then remove it
                return;
            }
        }
    }

    template<class C, void (C::*func)(T)>
    void unregisterHandler(C* instance) {
        // Create a matching Delegate
        Delegate<T> d;
        d.bind<C, func>(instance);
        // Iterate through attached function delegates
        for (auto it = observers.begin(); it != observers.end(); it++) {
            // Find one that is equal
            if (*it == d) {
                observers.erase(it); // Then remove it
                return;
            }
        }
    }

    void broadcast(T arg) {
        for (auto& f : observers) {
            f.invoke(arg);
        }
    }

private:
    std::vector<Delegate<T>> observers;
};

// 2 arguments
template<typename Tx, typename Ty>
class Event<Tx, Ty> {
public:
    template<void (*func)(Tx, Ty)>
    void registerHandler() {
        Delegate<Tx, Ty> delegate;
        delegate.bind<func>();
        observers.push_back(delegate);
    }
    
    template<class C, void (C::*func)(Tx, Ty)>
    void registerHandler(C* instance) {
        Delegate<Tx, Ty> delegate;
        delegate.bind<C, func>(instance);
        observers.push_back(delegate);
    }

    template<void (*func)(Tx, Ty)>
    void unregisterHandler() {
        // Create a matching Delegate
        Delegate<Tx, Ty> d;
        d.bind<func>();
        // Iterate through attached function delegates
        for (auto it = observers.begin(); it != observers.end(); it++) {
            // Find one that is equal
            if (*it == d) {
                observers.erase(it); // Then remove it
                return;
            }
        }
    }

    template<class C, void (C::*func)(Tx, Ty)>
    void unregisterHandler(C* instance) {
        // Create a matching Delegate
        Delegate<Tx, Ty> d;
        d.bind<C, func>(instance);
        // Iterate through attached function delegates
        for (auto it = observers.begin(); it != observers.end(); it++) {
            // Find one that is equal
            if (*it == d) {
                observers.erase(it); // Then remove it
                return;
            }
        }
    }

    void broadcast(Tx arg0, Ty arg1) {
        for (auto& f : observers) {
            f.invoke(arg0, arg1);
        }
    }

private:
    std::vector<Delegate<Tx, Ty>> observers;
};

// Zero arguments
template<>
class Event<void> {
public:
    template<void (*func)(void)>
    void registerHandler() {
        Delegate<void> delegate;
        delegate.bind<func>();
        observers.push_back(delegate);
    }
    
    template<class C, void (C::*func)()>
    void registerHandler(C* instance) {
        Delegate<void> delegate;
        delegate.bind<C, func>(instance);
        observers.push_back(delegate);
    }

    template<void (*func)(void)>
    void unregisterHandler() {
        // Create a matching Delegate
        Delegate<void> d;
        d.bind<func>();
        // Iterate through attached function delegates
        for (auto it = observers.begin(); it != observers.end(); it++) {
            // Find one that is equal
            if (*it == d) {
                observers.erase(it); // Then remove it
                return;
            }
        }
    }

    template<class C, void (C::*func)(void)>
    void unregisterHandler(C* instance) {
        // Create a matching Delegate
        Delegate<void> d;
        d.bind<C, func>(instance);
        // Iterate through attached function delegates
        for (auto it = observers.begin(); it != observers.end(); it++) {
            // Find one that is equal
            if (*it == d) {
                observers.erase(it); // Then remove it
                return;
            }
        }
    }

    void broadcast() {
        for (auto& f : observers) {
            f.invoke();
        }
    }

private:
    std::vector<Delegate<void>> observers;
};


#endif // EVENT_TYPES_H