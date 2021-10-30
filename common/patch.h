
#ifndef DWRANDOMIZER_PATCH_H
#define DWRANDOMIZER_PATCH_H

#include <stdint.h>

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
uint16_t vpatch(const dw_rom *rom, const uint32_t address, const uint32_t size,
        ...);
/**
 * Patches the rom at the specified address with the specified bytes.
 *
 * @param rom The rom struct to be patched
 * @param address The address inside the rom to apply the patch
 * @param size The size of the patch
 * @param data An array of bytes to use for the patch.
 * @return The address of the end of the patch
 */
uint16_t patch(const dw_rom *rom, const uint32_t address, const uint32_t size,
        const uint8_t *data);

/**
 * Patches the data at the specified pointer
 *
 * @param p A pointer to the rom data to be patched
 * @param size The size of the patch
 * @param data An array of bytes to use for the patch.
 * @return A pointer to the end of the patch
 */
uint8_t *ppatch(uint8_t *p, const uint32_t size, const uint8_t *data);

/**
 * Patches the data at the specified pointer
 *
 * @param p A pointer to the rom data to be patched
 * @param size The size of the patch.
 * @param ... A series of uint8_t bytes to be used for the patchj
 * @return A pointer to the end of the patch
 */
uint8_t *pvpatch(uint8_t *p, const uint32_t size, ...);

#endif
