#pragma once
// ═══════════════════════════════════════════════════════════
//  OFFSETS FOR Minecraft Bedrock 1.26.33.1  (arm64-v8a)
//  Fill from your Ghidra/IDA session.
//  Values are relative to libminecraftpe.so base address.
//  While any value is 0, the mod builds and loads in STUB MODE
//  (no hooks installed) so CI stays green.
// ═══════════════════════════════════════════════════════════

#define OFF_LEVEL_TICK            0x00000000   // Level::tick()
#define OFF_SET_BLOCK             0x00000000   // BlockSource::setBlock()
#define OFF_GET_WEATHER           0x00000000   // Level::getWeather()

#define OFF_LEVEL_WEATHER_TYPE    0x000        // Level → int (0 clear,1 rain,2 thunder)
#define OFF_BLOCK_ID              0x000        // Block → int
#define OFF_BLOCKPOS_X            0x000
#define OFF_BLOCKPOS_Y            0x004
#define OFF_BLOCKPOS_Z            0x008

#define BLOCK_ID_OAK_LOG          17
#define BLOCK_ID_OAK_LOG2         162
#define BLOCK_ID_MANGROVE_LOG     739   // VERIFY
#define BLOCK_ID_CHERRY_LOG       741   // VERIFY
#define BLOCK_ID_CRIMSON_STEM     480   // VERIFY
#define BLOCK_ID_WARPED_STEM      481   // VERIFY

#define OFFSETS_UNSET (OFF_LEVEL_TICK == 0 || OFF_SET_BLOCK == 0)
