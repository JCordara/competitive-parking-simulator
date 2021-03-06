#include "DescriptionComponent.h"


DescriptionComponent::DescriptionComponent(weak_ptr<Entity> e)
	: BaseComponent(e)
{}

std::optional<int> DescriptionComponent::getInteger(string key) {
	auto it = integers.find(key);
	if (it != integers.end()) return std::optional<int>(it->second);
	return std::optional<int>();
}

std::optional<double> DescriptionComponent::getRealNumber(string key) {
	auto it = reals.find(key);
	if (it != reals.end()) return std::optional<double>(it->second);
	return std::optional<double>();
}
std::optional<string> DescriptionComponent::getString(string key) {
	auto it = strings.find(key);
	if (it != strings.end()) return std::optional<string>(it->second);
	return std::optional<string>();
}

std::optional<glm::vec3> DescriptionComponent::getVec3(string key) {
	auto it = vec3s.find(key);
	if (it != vec3s.end()) return std::optional<glm::vec3>(it->second);
	return std::optional<glm::vec3>();
}

void DescriptionComponent::setInteger(string key, int val) {
	removeInteger(key);
	integers.insert({ key, val });
}
void DescriptionComponent::setRealNumber(string key, double val) {
	removeRealNumber(key);
	reals.insert({ key, val });
}
void DescriptionComponent::setString(string key, string val) {
	removeString(key);
	strings.insert({ key, val });
}
void DescriptionComponent::setVec3(string key, glm::vec3 val) {
	removeVec3(key);
	vec3s.insert({ key, val });
}

void DescriptionComponent::removeInteger(string key) {
	auto it = integers.find(key);
	if (it != integers.end())
		integers.erase(key);
}
void DescriptionComponent::removeRealNumber(string key) {
	auto it = reals.find(key);
	if (it != reals.end())
		reals.erase(key);
}
void DescriptionComponent::removeString(string key) {
	auto it = strings.find(key);
	if (it != strings.end())
		strings.erase(key);
}
void DescriptionComponent::removeVec3(string key) {
	auto it = vec3s.find(key);
	if (it != vec3s.end())
		vec3s.erase(key);
}

void DescriptionComponent::flushIntegers() { integers.clear(); }
void DescriptionComponent::flushRealNumbers() { reals.clear(); }
void DescriptionComponent::flushStrings() { strings.clear();  }
void DescriptionComponent::flushVec3s() { vec3s.clear(); }

void DescriptionComponent::flush() {
	flushIntegers();
	flushRealNumbers();
	flushStrings();
	flushVec3s();
}

ComponentEnum DescriptionComponent::getType() {
	return ComponentEnum::description;
}

DescriptionComponent::~DescriptionComponent() {
	// Nothing to do here yet
}
