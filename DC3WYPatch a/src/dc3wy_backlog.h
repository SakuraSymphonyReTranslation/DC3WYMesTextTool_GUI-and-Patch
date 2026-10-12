#pragma once

namespace dc3wy_backlog {
struct IconName {
  const char *name;
  int line; // Atlas Y, [esp+0x10] in sub_4043C0.
  int row;  // Atlas X, EDI in sub_4043C0.
};

// Exact Indonesian name fields, aligned with Japanese entries in all 189
// H:/Games/Lingugacha/dc3wy/JSON_Translated/wy_*.json files (2026-10-09).
// Coordinates verified in DC3WY.EXE, 0x404680..0x404BFC. Only characters
// supported by the original atlas are included; group/unknown labels keep
// the engine's textual name fallback.
inline constexpr IconName names[] = {
    {"Rikka", 0, 0}, {"Ricca", 0, 0}, {"Ricca Greenwood", 0, 0},
    {"Charles", 0, 1}, {"Himeno", 0, 2},
    {"Sara", 0, 3}, {"Sarah", 0, 3}, {"Aoi", 0, 4}, {"Shiki", 0, 5},
    {"Mikoto", 1, 0}, {"Sumomo", 1, 1},
    {"Mary", 1, 2}, {"Mary Holmes", 1, 2}, {"Edward", 1, 3},
    {"Rurika", 1, 4}, {"Jill", 1, 5},
    {"Sakura", 2, 0}, {"Suginami", 2, 1}, {"Kousuke", 2, 2},
    {"Ian", 2, 3}, {"Eto", 2, 4}, {"Tomoe", 2, 5},
    {"Elizabeth", 3, 0}, {"Anzu", 3, 2}, {"Kiyotaka", 3, 5},
};

inline bool ResolveIcon(const char *name, int &line, int &row) {
  if (!name || !*name)
    return false;
  for (const auto &entry : names) {
    unsigned int i = 0;
    // The MES importer encodes spaces as underscores to avoid encrypted NULs.
    // Match the whole 32-byte speaker field, never a prefix or group label.
    for (; i < 32 && name[i] && entry.name[i]; ++i) {
      if ((name[i] == '_' ? ' ' : name[i]) != entry.name[i])
        break;
    }
    if (i < 32 && name[i] == '\0' && entry.name[i] == '\0') {
      line = entry.line;
      row = entry.row;
      return true;
    }
  }
  return false;
}
} // namespace dc3wy_backlog
