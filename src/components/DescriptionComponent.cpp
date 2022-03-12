#include "DescriptionComponent.h"


DescriptionComponent::DescriptionComponent(shared_ptr<Entity> e)
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

void DescriptionComponent::flushIntegers() { integers.clear(); }
void DescriptionComponent::flushRealNumbers() { reals.clear(); }
void DescriptionComponent::flushStrings() { strings.clear();  }

void DescriptionComponent::flush() {
	flushIntegers();
	flushRealNumbers();
	flushStrings();
}

ComponentEnum DescriptionComponent::getType() {
	return ComponentEnum::description;
}

DescriptionComponent::~DescriptionComponent() {
	// Nothing to do here yet
}
