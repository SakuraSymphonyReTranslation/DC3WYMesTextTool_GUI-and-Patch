#pragma once

namespace dc3wy_line_spacing {
// Return addresses following the body's SetStr calls in sub_4233A0.
inline bool IsDialogueBodyCall(unsigned int caller) {
  return caller == 0x004242B7 || caller == 0x004299DC;
}

// Three visual states of backlog body text in sub_4043C0. Name and voice
// controls use other calls to the same generic text constructor.
inline bool IsBacklogBodyCall(unsigned int caller) {
  return caller == 0x00404D6D || caller == 0x00404D90 ||
         caller == 0x00404DB3;
}

inline bool HasRuby(const char *text) {
  if (!text) return false;
  const unsigned char *p = reinterpret_cast<const unsigned char *>(text);
  for (; *p; ++p) {
    if (*p == 0x81 && p[1] == 0x6F) return true;
    if ((*p >= 0x81 && *p <= 0x9F) || (*p >= 0xE0 && *p <= 0xFC)) {
      if (!p[1]) break;
      ++p;
    }
  }
  return false;
}

inline int LineAdvance(int textureHeight, int spacing) {
  // A normal texture is 22+12=34px. Leave at least 24px for its 22px
  // character cell and outline. Large @l text retains its larger cell.
  if (textureHeight < 12) return textureHeight;
  if (spacing < -10) spacing = -10;
  if (spacing > 64) spacing = 64;
  return textureHeight + spacing;
}
} // namespace dc3wy_line_spacing
