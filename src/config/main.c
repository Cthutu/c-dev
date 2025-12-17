#define KORE_IMPLEMENTATION
#include <kore/intern.h>
#include <kore/kore.h>
#include <kore/sexp.h>

int kmain(int argc, char** argv)
{
    KORE_UNUSED(argc);
    KORE_UNUSED(argv);

    Interner interner;
    intern_init(&interner);
    intern_leak(&interner);

    string test_str   = intern_cstr(&interner, "Hello, World!");
    string test_str_2 = intern_cstr(&interner, "Hello, World!");

    prn("Interned string 1 (%p): " STRINGP,
        intern_get_info(test_str),
        STRINGV(test_str));
    prn("Interned string 2 (%p): " STRINGP,
        intern_get_info(test_str_2),
        STRINGV(test_str_2));

    return 0;
}
