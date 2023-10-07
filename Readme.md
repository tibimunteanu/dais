### TODO

- add containers like string, dynamic array, hash table, etc.
- add custom events
- add cglm or finalize custom math library
- add test manager
- add platform layer (just win32 for now) with monitors, windows, input, time, io, etc.
- finish vulkan initialization to clear screen
- add instrumentation

### Coding Style

- use UPPER_CASE for constants
- use PascalCase for types, structs, enums
- use camelCase for variables, parameters, functions
- use out_camelCase for out parameters
- use pPascalCase, ppPascalCase, etc for pointer parameters
- use pfnPascalCase for function pointer parameters
- use PFN_typeObjectVerbProperty for function pointers
- use typeObjectVerbProperty for functions (f64Atan2)
- use \_pascalCase for internal functions
- use singular for enum types e.g. AppState
- use ENUM_NAME_ENUM_VALUE for enum values

###### Error handling and propagation

- favor `out_pResult` pointers for out values
- most functions should either be `fn` or `void`
- if a function is `fn`, we can use `try()`, `alert()`, `panic()`, to propagate errors
- we can just test a `fn` result with `catch()` and `noerr()`
- use `ok()` to signal that a function executed successfully
- we can define new result set enums and use them with `panic()`
- result set enums can alternatively be handled in switch cases
- when a `panic()` occurs, the full stack trace is logged

- IDEA: we can actually make Result a union and use a macro like `Result(T)` for convenience.
  error sets could use the range [MIN_I16, MAX_I16] and we use the top 16 bits
  of the I32 underlying type of Result to set a fixed error tag so that the macros can
  tell if the Result is an error or not.
  so `try()` casts the Result to the error type and checks the top 16 bits for the presence of the
  error tag. if it's there, then it logs the error and propagates it (returns the result).
  if the error tag is not there, then it casts the result to T so that it can be assigned as needed.
