#ifndef VAW_ADD_NOTES_NOTE_H
#define VAW_ADD_NOTES_NOTE_H

#include <cmath>
#include <string>
#include <map>

#include "Conversions.h"

namespace vaw::add_notes {

// Time is in signed integers to avoid overflow due to subtraction of durations later on
struct Note {
  std::int64_t start_millis;
  std::int64_t end_millis;
  float frequency;
  // A map of timestamp in milliseconds to a volume level. Instantaneous volume will be
  // interpolated using the reference points
  std::map<std::int64_t, float> time_volume_map;

  Note(std::int64_t start, float duration_seconds, std::string const & note_name);

  void AddVolumePoint(int64_t time_millis, float volume);
  float GetVolume(int64_t elapsed_time_millis) const;
};

}  // namespace vaw::add_notes

#endif  // VAW_ADD_NOTES_NOTE_H
