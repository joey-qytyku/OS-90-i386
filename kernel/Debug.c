#include <stdarg.h>

// Cannot find stdarg.h, use builtins?
// Is autocomplete not showing them?
void KePrint(const char *f, ...)
{
    va_list ap;
    int i;
    va_start(ap,i);
}
