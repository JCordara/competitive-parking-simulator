#pragma once
#include "Window.h"
#include <list>
#include <functional>
#include <set>

//Use these to generate the function object
#define bindMethodFunction_2_Variables(a,b) std::bind(a,b, std::placeholders::_1, std::placeholders::_2)
#define bindMethodFunction_0_Variables(a,b) std::bind(a,b)

class GameEventManager : public CallbackInterface {

public:
	// These are the types of functions that can be set to respond to an event, if a function
	// you want to call does not follows these (example extra inputs), a wrapper will need to be
	// created of the form below that then calls the intended function
	using mousePositionFunction = std::function<void(double, double)>;
	using windowSizeFunction = std::function<void(int, int)>;
	using scrollFunction = std::function<void(double, double)>;
	using mouseButtonFunction = std::function<void()>;
	using keyFunction = std::function<void()>;

	// These are for the window to call to notify a event has happened
	void keyCallback(int key, int scancode, int action, int mods);
	void mouseButtonCallback(int button, int action, int mods);
	void cursorPosCallback(double xpos, double ypos);
	void scrollCallback(double xoffset, double yoffset);
	void windowSizeCallback(int width, int height);

	//Functions to register/deregister a fucntion of the types defined above
	unsigned int registerWindowSize(windowSizeFunction func);
	bool deregisterWindowSize(windowSizeFunction func, unsigned int id);

	unsigned int registerKey(keyFunction func, int key, int action, int mods);
	bool deregisterKey(keyFunction func, unsigned int id, int key, int action, int mods);

	unsigned int registerMousePosition(mousePositionFunction func);
	bool deregisterMousePosition(mousePositionFunction func, unsigned int id);

	unsigned int registerScroll(scrollFunction func);
	bool deregisterScroll(scrollFunction func, unsigned int id);

	unsigned int registerMouseButton(mouseButtonFunction func, int button, int action, int mods);
	bool deregisterMouseButton(mouseButtonFunction func, unsigned int id, int button, int action, int mods);

private:
	std::set<unsigned int> keylogger;
	unsigned int current = 0;

	unsigned int nextValidKey() {
		unsigned int start = current - 1;
		for (; keylogger.find(current) != keylogger.end(); current++) {
			if (current == start) exit(-1); //Unlikely to happen (would need 4,294,967,295 registered functions at once)
		}
		return current;
	}

	bool removeKey(unsigned int id) {
		auto it = keylogger.find(id);
		if (it == keylogger.end()) return false;
		keylogger.erase(it);
		return true;
	}

	template <typename T>
	class id_function {
	public:
		struct desc {
			int buttonOrKey;
			int action;
			int mod;
		};

		bool compareDescriptor(int buttonOrKey, int action, int mod) {
			if (mod > 0) return (buttonOrKey == descriptor.buttonOrKey) && (action == descriptor.action);
			return (buttonOrKey == descriptor.buttonOrKey) && (action == descriptor.action) && (mod == descriptor.mod);
		}
		id_function(T function, unsigned int id) {
			this->function = function;
			this->id = id;
		}
		id_function(T function, unsigned int id, int buttonOrKey, int action, int mod) {
			this->function = function;
			this->id = id;
			this->descriptor.buttonOrKey = buttonOrKey;
			this->descriptor.action = action;
			this->descriptor.mod = mod;
		}

		bool operator==(const id_function<T>& o) const { return id == o.id; }
		T getFunction() { return function; }
		int getDescriptor() { return descriptor; }
	private:
		unsigned int id;
		T function;
		desc descriptor;
	};

	std::list<id_function<windowSizeFunction>> windowSizeCallBacks;
	std::list<id_function<mousePositionFunction>> mousePositionCallBacks;
	std::list<id_function<scrollFunction>> scrollCallBacks;
	std::list<id_function<mouseButtonFunction>> mouseButtonCallBacks;
	std::list<id_function<keyFunction>> keyCallBacks;

};
