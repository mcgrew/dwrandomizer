#include <stdint.h>
#include <stdarg.h>

#include "dwr_types.h"

/**
 * Patches the rom at the specified address with the specified bytes.
 *
 * @param rom The rom struct to be patched
 * @param address The address inside the rom to apply the patch.
 * @param size The size of the patch
 * @param ... A series of uint8_t bytes, the patch data
 * @return The address of the end of the patch
 */
uint16_t vpatch(dw_rom *rom, uint32_t address, uint32_t size, ...)
{
    uint32_t i;
    va_list arg;
    uint8_t *p;

    p = &rom->raw[address];
    va_start(arg, size);

    for (i=0; i < size; i++) {
        *(p++) = va_arg(arg, int);
    }
    va_end(arg);
    return p - rom->raw;
}

/**
 * Patches the rom at the specified address with the specified bytes.
 *
 * @param rom The rom struct to be patched
 * @param address The address inside the rom to apply the patch
 * @param size The size of the patch
 * @param data An array of bytes to use for the patch.
 * @return The address of the end of the patch
 */
uint16_t patch(dw_rom *rom, uint32_t address, uint32_t size,
                      uint8_t *data)
{
    uint32_t i;
    uint8_t *p;

    p = &rom->raw[address];

    for (i=0; i < size; i++) {
        *(p++) = data[i];
    }
    return p - rom->raw;
}

/**
 * Patches the data at the specified pointer
 *
 * @param p A pointer to the rom data to be patched
 * @param size The size of the patch
 * @param data An array of bytes to use for the patch.
 * @return A pointer to the end of the patch
 */
uint8_t *ppatch(uint8_t *p, uint32_t size, uint8_t *data)
{
    uint32_t i;

    for (i=0; i < size; i++) {
        *(p++) = data[i];
    }
    return p;
}

/**
 * Patches the data at the specified pointer
 *
 * @param p A pointer to the rom data to be patched
 * @param size The size of the patch.
 * @param ... A series of uint8_t bytes to be used for the patchj
 * @return A pointer to the end of the patch
 */
uint8_t *pvpatch(uint8_t *p, uint32_t size, ...)
{
    uint32_t i;
    va_list arg;

    va_start(arg, size);

    for (i=0; i < size; i++) {
        *(p++) = va_arg(arg, int);
    }
    va_end(arg);
    return p;

}
