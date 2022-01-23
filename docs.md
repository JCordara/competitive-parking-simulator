# Engine documentation
To help the rest of the team keep up with the crazy stuff Jeff and Joseph are 
doing

## Table of contents

<!--ts-->
   * [Events](#events)
   * [Rendering](#rendering)
   * [Audio](#audio)
<!--te-->

<a name="events"/>

## The event system
The `Events` namespace in Event.h holds a list of globally accessible events 
that will probably be frequently added to and modified as we build out the 
engine. (probably things like GameStart, GameUpdate, collision events, ...)

The event system is built on the observer pattern, where each event is a subject
and observers are functions rather than objects. This means that we don't need
to register entire sub-systems (Rendering, Audio, AI, ...) as observers to a
central "event queue", rather each sub-system can register specific handler
functions as observers (listeners) for certain events. This eliminates the need
for obnoxiously huge switch statements in sub-system event handlers, and should 
boost performance a little.

##### Usage:
To respond to an event, create a function to handle the event and register it as
an observer. For example, here's how you might print something to the console 
when the game starts:
```cpp
void printSomething() {
    printf("um hello\n");
}

Events::GameStart.registerHandler<printSomething>();
```
It gets a little more complex with class functions (which is what you'll use to
register handlers 99% of the time):
```cpp
class Foo {
public:
    Foo() {
        Events::GameStart.registerHandler<Foo, &Foo::onGameStart>(this);
    }
    
    void onGameStart () {
        printf("um hello\n");
    }
};
```
Or you could put the call to `registerHandler()` outside of the class, like so:
```cpp
class Foo {
    onGameStart () {
        printf("um hello\n");
    }
};

Foo foo;
Events::GameStart.registerHandler<Foo, &Foo::onGameStart>(&foo);
```
The `registerHandler` function is a little complicated, but trust me I 
simplified it as much as I could. C++ just doesn't like the idea of functional 
programming and I don't love the standard library implementation. Here's how it 
works:

There are two overloads for the function, one for free functions and one for 
class functions/methods. The free function one is a lot simpler and it looks
like this:
```cpp
void Event::registerHandler<*FreeFunctionPtr>();
```
So you just have to pass the function address as the template parameter. But for
class functions, the event needs to know the owner class, the function address, 
and the instance to invoke the function on. So it looks like this:
```cpp
void Event::registerHandler<Class, *ClassFunctionPtr>(*instance);
```
And you get a class function pointer with this syntax:
```cpp
&Class::function
```
##### Creating new events
To create a new event, add it to the `Events` namespace in Event.h. Events are 
declared as extern and with a template parameter to indicate what type of data
is associated with them. (e.g. "DamageEnemy" could take an int for how much 
damage was done). Some events don't have data associated, like `GameStart`, so 
they recieve the `<void>` template parameter.

After adding the event to the list, you also need to define it in a source file
(Event.cpp) since it's extern. Otherwise the linker complains about the events 
being defined multiple times. Ex:
```cpp
// Event.h
class Events {
public:
  ...
  static Event<int> DamageEnemy;
  ...
};
// Event.cpp
Event<int> Events::DamageEnemy;
```

<a name="rendering"/>

## The rendering system
Stub

<a name="audio"/>

## The audio system
Stub
