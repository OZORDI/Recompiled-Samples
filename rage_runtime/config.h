/**
 * RAGE Runtime Configuration
 * 
 * Edit these paths to point to your extracted game assets.
 * The VFS will map Xbox paths to these host directories.
 */

#ifndef RAGE_RUNTIME_CONFIG_H_
#define RAGE_RUNTIME_CONFIG_H_

// Default root for all extracted RAGE game assets
// Change this to wherever you've extracted your game files
#define RAGE_ASSET_ROOT "/Users/Ozordi/Downloads/xenia/xenia/RAGE/extracted"

// Per-game asset directories (relative to RAGE_ASSET_ROOT)
#define GTA_IV_ASSETS    "gta_iv"
#define GTA_V_ASSETS     "gta_v"
#define MAX_PAYNE_ASSETS "max_payne"
#define MIDNIGHT_CLUB_ASSETS "midnight_club"

// Full paths (computed from root)
#define GTA_IV_PATH    RAGE_ASSET_ROOT "/" GTA_IV_ASSETS
#define GTA_V_PATH     RAGE_ASSET_ROOT "/" GTA_V_ASSETS
#define MAX_PAYNE_PATH RAGE_ASSET_ROOT "/" MAX_PAYNE_ASSETS
#define MIDNIGHT_CLUB_PATH RAGE_ASSET_ROOT "/" MIDNIGHT_CLUB_ASSETS

#endif  // RAGE_RUNTIME_CONFIG_H_
