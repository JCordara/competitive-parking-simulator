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

#include "stdafx.h"

// Function pointer object with any single argument type
// https://blog.molecular-matters.com/2011/09/19/generic-type-safe-delegates-and-events-in-c/
template<typename T>
class Delegate {
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
template<typename T>
class Event {
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

// Template specialization for zero arguments
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