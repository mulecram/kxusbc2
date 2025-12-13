#include "userrow.h"

#include <avr/io.h>

// Memory-mapped pointer to userrow
struct UserRow *userrow = (struct UserRow*)USER_SIGNATURES_START;

bool userrow_valid(void) {
    return userrow->magic == USERROW_MAGIC;
}
