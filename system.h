#ifndef SYSTEM_H
#define SYSTEM_H

#include <random>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

//! Returns the default random engine.
inline std::default_random_engine& default_generator() {
	#if defined(__MINGW32_MAJOR_VERSION) && __MINGW32_MAJOR_VERSION < 5
		static std::default_random_engine generator(time(NULL));
	#else
		static std::default_random_engine generator(std::random_device{}());
	#endif
	return generator;
}

//! Returns a random number in [0..n-1].
inline unsigned int unif_random(unsigned int n) {
	std::uniform_int_distribution<unsigned int> distro(0, n-1);
	return distro(default_generator());
}

//! MSVC compatibility: undefine max and min macros.
#if defined(max)
#undef max
#endif

#if defined(min)
#undef min
#endif

//! Macros used to export symbols to shared library interface.
#if defined _WIN32
  #define GAME2048_HELPER_DLL_IMPORT __declspec(dllimport)
  #define GAME2048_HELPER_DLL_EXPORT __declspec(dllexport)
  #define GAME2048_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define GAME2048_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define GAME2048_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define GAME2048_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define GAME2048_HELPER_DLL_IMPORT
    #define GAME2048_HELPER_DLL_EXPORT
    #define GAME2048_HELPER_DLL_LOCAL
  #endif
#endif

#ifdef GAME2048_DLL // defined if the project is compiled as a DLL
  #ifdef GAME2048_DLL_EXPORTS // defined if we are building the dll (as opposed to using it)
    #define GAME2048_API GAME2048_HELPER_DLL_EXPORT
  #else
    #define GAME2048_API GAME2048_HELPER_DLL_IMPORT
  #endif // GAME2048_DLL_EXPORTS
  #define GAME2048_LOCAL GAME2048_HELPER_DLL_LOCAL
#else // GAME2048_DLL is not defined: this means GAME2048 is a static lib.
  #define GAME2048_API
  #define GAME2048_LOCAL
#endif // GAME2048_DLL

// An exception class.
class GAME2048_API IllegalAction: public std::runtime_error {
public:
	IllegalAction(const std::string &__arg): std::runtime_error(__arg) {}
	virtual ~IllegalAction() throw() {}
};

#endif // SYSTEM_H
