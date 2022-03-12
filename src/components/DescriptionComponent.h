#ifndef DESCRIPTION_COMPONENT_H
#define DESCRIPTION_COMPONENT_H

#include "Components.h"

class DescriptionComponent : public BaseComponent {
public:

	DescriptionComponent(shared_ptr<Entity> e);

	std::optional<int> getInteger(string key);
	std::optional<double> getRealNumber(string key);
	std::optional<string> getString(string key);

	void setInteger(string key, int val);
	void setRealNumber(string key, double val);
	void setString(string key, string val);

	void removeInteger(string key);
	void removeRealNumber(string key);
	void removeString(string key);

	void flushIntegers();
	void flushRealNumbers();
	void flushStrings();
	void flush();

	static  ComponentEnum getType();
	~DescriptionComponent();

private:
	unordered_map<string, int>    integers;
	unordered_map<string, double> reals;
	unordered_map<string, string> strings;
};

#endif //DESCRIPTION_COMPONENT_H
