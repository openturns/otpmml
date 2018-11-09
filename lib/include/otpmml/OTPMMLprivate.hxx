
#ifndef OTPMML_PRIVATE_HXX
#define OTPMML_PRIVATE_HXX

/* From http://gcc.gnu.org/wiki/Visibility */
/* Generic helper definitions for shared library support */
#if defined _WIN32 || defined __CYGWIN__
#define OTPMML_HELPER_DLL_IMPORT __declspec(dllimport)
#define OTPMML_HELPER_DLL_EXPORT __declspec(dllexport)
#define OTPMML_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define OTPMML_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define OTPMML_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define OTPMML_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define OTPMML_HELPER_DLL_IMPORT
#define OTPMML_HELPER_DLL_EXPORT
#define OTPMML_HELPER_DLL_LOCAL
#endif
#endif

/* Now we use the generic helper definitions above to define OTPMML_API and OTPMML_LOCAL.
 * OTPMML_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
 * OTPMML_LOCAL is used for non-api symbols. */

#ifndef OTPMML_STATIC /* defined if OT is compiled as a DLL */
#ifdef OTPMML_DLL_EXPORTS /* defined if we are building the OT DLL (instead of using it) */
#define OTPMML_API OTPMML_HELPER_DLL_EXPORT
#else
#define OTPMML_API OTPMML_HELPER_DLL_IMPORT
#endif /* OTPMML_DLL_EXPORTS */
#define OTPMML_LOCAL OTPMML_HELPER_DLL_LOCAL
#else /* OTPMML_STATIC is defined: this means OT is a static lib. */
#define OTPMML_API
#define OTPMML_LOCAL
#endif /* !OTPMML_STATIC */


#endif // OTPMML_PRIVATE_HXX

