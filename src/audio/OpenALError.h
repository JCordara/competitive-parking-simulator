#ifndef OPEN_AL_ERROR_H
#define OPEN_AL_ERROR_H

#define AL_LIBTYPE_STATIC

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>

#include <string>
#include <iostream>


#define alCheckErrors() check_al_errors(__FILE__, __LINE__)
#define alcCheckErrors(device) check_alc_errors(__FILE__, __LINE__, device)

bool check_al_errors(const std::string& filename, const std::uint_fast32_t line);
bool check_alc_errors(const std::string& filename, const std::uint_fast32_t line, ALCdevice* device);


#endif //OPEN_AL_ERROR_H
