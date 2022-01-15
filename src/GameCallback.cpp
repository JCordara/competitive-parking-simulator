#pragma once
#include "GameCallback.h"

unsigned int GameEventManager::registerWindowSize(windowSizeFunction func) {
	unsigned int newKey = nextValidKey();
	windowSizeCallBacks.push_back(id_function<windowSizeFunction>(func, newKey));
	keylogger.insert(newKey);
	return newKey;
}
bool GameEventManager::deregisterWindowSize(windowSizeFunction func, unsigned int id){
	if (removeKey(id)) {
		size_t old_size = windowSizeCallBacks.size();
		windowSizeCallBacks.remove(id_function<windowSizeFunction>(func, id));
		return (old_size > windowSizeCallBacks.size());
	}else return false;
}

void GameEventManager::windowSizeCallback(int width, int height) {
	CallbackInterface::windowSizeCallback(width, height); //Will call glViewport for us (dont remove)
	for (std::list<id_function<windowSizeFunction>>::iterator iteratior = windowSizeCallBacks.begin(); iteratior != windowSizeCallBacks.end(); iteratior++)
		(*iteratior).getFunction()(width, height);
}

unsigned int GameEventManager::registerMousePosition(mousePositionFunction func) {
	unsigned int newKey = nextValidKey();
	mousePositionCallBacks.push_back(id_function<mousePositionFunction>(func, newKey));
	keylogger.insert(newKey);
	return newKey;
}
bool GameEventManager::deregisterMousePosition(mousePositionFunction func, unsigned int id) {
	if (removeKey(id)) {
		size_t old_size = mousePositionCallBacks.size();
		mousePositionCallBacks.remove(id_function<mousePositionFunction>(func, id));
		return (old_size > mousePositionCallBacks.size());
	}
	else return false;
}

void GameEventManager::cursorPosCallback(double xpos, double ypos) {
	for (std::list<id_function<mousePositionFunction>>::iterator iteratior = mousePositionCallBacks.begin(); iteratior != mousePositionCallBacks.end(); iteratior++)
		(*iteratior).getFunction()(xpos, ypos);
}

unsigned int GameEventManager::registerScroll(scrollFunction func) {
	unsigned int newKey = nextValidKey();
	scrollCallBacks.push_back(id_function<scrollFunction>(func, newKey));
	keylogger.insert(newKey);
	return newKey;
}
bool GameEventManager::deregisterScroll(scrollFunction func, unsigned int id) {
	if (removeKey(id)) {
		size_t old_size = scrollCallBacks.size();
		scrollCallBacks.remove(id_function<scrollFunction>(func, id));
		return (old_size > scrollCallBacks.size());
	}
	else return false;
}

void GameEventManager::scrollCallback(double xoffset, double yoffset) {
	for (std::list<id_function<scrollFunction>>::iterator iteratior = scrollCallBacks.begin(); iteratior != scrollCallBacks.end(); iteratior++)
		(*iteratior).getFunction()(xoffset, yoffset);
}

unsigned int GameEventManager::registerMouseButton(mouseButtonFunction func, int button, int action, int mods) {
	unsigned int newKey = nextValidKey();
	mouseButtonCallBacks.push_back(id_function<mouseButtonFunction>(func, newKey, button, action, mods));
	keylogger.insert(newKey);
	return newKey;
}
bool GameEventManager::deregisterMouseButton(mouseButtonFunction func, unsigned int id, int button, int action, int mods) {
	if (removeKey(id)) {
		size_t old_size = mouseButtonCallBacks.size();
		mouseButtonCallBacks.remove(id_function<mouseButtonFunction>(func, id, button, action, mods));
		return (old_size > mouseButtonCallBacks.size());
	}
	else return false;
}

void GameEventManager::mouseButtonCallback(int button, int action, int mods) {
	for (std::list<id_function<mouseButtonFunction>>::iterator iteratior = mouseButtonCallBacks.begin(); iteratior != mouseButtonCallBacks.end(); iteratior++)
		if((*iteratior).compareDescriptor(button, action, mods)) (*iteratior).getFunction()();
}

unsigned int GameEventManager::registerKey(keyFunction func, int key, int action, int mods) {
	unsigned int newKey = nextValidKey();
	keyCallBacks.push_back(id_function<keyFunction>(func, newKey, key, action, mods));
	keylogger.insert(newKey);
	return newKey;
}
bool GameEventManager::deregisterKey(keyFunction func, unsigned int id, int key, int action, int mods) {
	if (removeKey(id)) {
		size_t old_size = keyCallBacks.size();
		keyCallBacks.remove(id_function<keyFunction>(func, id, key, action, mods));
		return (old_size > keyCallBacks.size());
	}
	else return false;
}

void GameEventManager::keyCallback(int key, int scancode, int action, int mods) {
	for (std::list<id_function<keyFunction>>::iterator iteratior = keyCallBacks.begin(); iteratior != keyCallBacks.end(); iteratior++)
		if ((*iteratior).compareDescriptor(key, action, mods)) (*iteratior).getFunction()();
}
