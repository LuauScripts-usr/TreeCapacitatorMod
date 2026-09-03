#pragma once
// ═══════════════════════════════════════════════════════════
//  OFFSETS FOR Minecraft Bedrock 1.26.33.1  (arm64-v8a)
//  Fill these from your Ghidra/IDA session.
//  ALL values are relative to libminecraftpe.so base address.
// ═══════════════════════════════════════════════════════════

// ─── Function offsets ──────────────────────────────────────
#define OFF_LEVEL_TICK            0x00000000   // Level::tick()
#define OFF_SET_BLOCK             0x00000000   // BlockSource::setBlock()
#define OFF_GET_WEATHER           0x00000000   // Level::getWeather()

// ─── Struct member offsets ─────────────────────────────────
#define OFF_LEVEL_WEATHER_TYPE    0x000        // within Level → int (0,1,2)
#define OFF_BLOCK_ID              0x000        // within Block  → int16/int32
#define OFF_BLOCKPOS_X            0x000        // within BlockPos → int32
#define OFF_BLOCKPOS_Y            0x004        // within BlockPos → int32
#define OFF_BLOCKPOS_Z            0x008        // within BlockPos → int32

// ─── Block IDs for logs (verify from registry) ────────────
#define BLOCK_ID_OAK_LOG          17
#define BLOCK_ID_SPRUCE_LOG       17    // same ID, different data value
#define BLOCK_ID_BIRCH_LOG        17
#define BLOCK_ID_JUNGLE_LOG       17
#define BLOCK_ID_OAK_LOG2         162
#define BLOCK_ID_ACACIA_LOG       162
#define BLOCK_ID_DARK_OAK_LOG     162
#define BLOCK_ID_MANGROVE_LOG     739   // 1.26.x new IDs, VERIFY
#define BLOCK_ID_CHERRY_LOG       741   // VERIFY
#define BLOCK_ID_CRIMSON_STEM     480   // VERIFY
#define BLOCK_ID_WARPED_STEM      481   // VERIFY

// ─── Sanity check ──────────────────────────────────────────
#if OFF_LEVEL_TICK == 0 || OFF_SET_BLOCK == 0
    #error "YOU MUST FILL IN THE OFFSETS BEFORE COMPILING"
#endif
