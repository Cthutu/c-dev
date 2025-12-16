#define KORE_IMPLEMENTATION
#include <kore/intern.h>
#include <kore/kore.h>

int kmain(int argc, char** argv)
{
    KORE_UNUSED(argc);
    KORE_UNUSED(argv);

    Interner interner;
    intern_init(&interner);

    string test_str   = intern_cstr(&interner, "Hello, World!");
    string test_str_2 = intern_cstr(&interner, "Hello, World!");

    prn("Interned string 1 (%p): " STRINGP, test_str.data, STRINGV(test_str));
    prn("Interned string 2 (%p): " STRINGP,
        test_str_2.data,
        STRINGV(test_str_2));

    return 0;
}
