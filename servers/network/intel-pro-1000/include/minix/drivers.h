// Based on the file with the same name from the Minix 3 source code
// Local modifications

// chaos libraries
#include <memory/memory.h>
#include <system/system.h>

// Minix compatibility headers
#include <sys/cdefs.h>

#include <minix/com.h>
#include <minix/sysutil.h>

// Compatibility defines
#define OK 0
#define ENXIO -6

// Compatibility names, to comply with what's used by the Minix driver.
typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef long int ssize_t;
typedef char message;

// Compatibility functions
static void memset(void *address, uint8_t c, unsigned int size)
{
    memory_set_uint8_t(address, c, size);
}
