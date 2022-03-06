#ifndef C_STD_H
#define C_STD_H

/* STL */
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <math.h>
#include <functional>
#include <stdexcept>
#include <type_traits>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Log.h"

using std::string;                           // Strings
using std::unordered_map, std::pair;         // Mapping
using std::shared_ptr,    std::make_shared;  // Shared pointers
using std::unique_ptr,    std::make_unique;  // Unique pointers
using std::enable_if_t,   std::is_base_of_v; // Template param type assertion
using std::dynamic_pointer_cast;             // Downcast smart pointers


#endif // C_STD_H
