/* public access modifier */
#define pub

/* private access modifier */
#define prv static

/* global access modifier */
#define global extern

/* This is a function which supports a better error handling mechanism which prints stack traces:
 * - use alert() to only log an error.
 * - use error() to return an error code - you can pass a custom negative error code.
 * - use try() to call functions marked with `fn` - if the called function fails, the error code is returned.
 * - use ok() to return success code.
 * - use catch() to check if the result of an error handled function is an error code - it turns the result into a bool.
 * - use unreachable() to return an `Unreachable path!` error.
 */
typedef int fn;

// NOTE: any result set enum can be used with "error(CUSTOM_RESULT)" macro
typedef enum FnResult {
    ERROR = -1,
    INCOMPLETE = 0,
    OK = 1,
} FnResult;

#define ok()        return OK
#define noerr(expr) ((expr) >= 0)
#define catch(expr) ((expr) < 0)

#define alert(expr)                                                          \
    if (catch (expr)) {                                                      \
        logError("    | -> %s() - %s:%d", __FUNCTION__, __FILE__, __LINE__); \
    }

#define try(expr)                                                                \
    {                                                                            \
        fn _result_ = (expr);                                                    \
        if (catch (_result_)) {                                                  \
            logFatal("    | -> %s() - %s:%d", __FUNCTION__, __FILE__, __LINE__); \
            return _result_;                                                     \
        }                                                                        \
    }

#define errorCustom(err, message, ...)                                                                    \
    logFatal("ERROR -> %s() - %s:%d - error: " message, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__); \
    return err

#define errorGeneric(message, ...) errorCustom(ERROR, message, ##__VA_ARGS__)

#define error(...) GET_OVERRIDE_012(_, errorGeneric, errorCustom, __VA_ARGS__)

#define unreachable error("Unreachable path!")
