#include "Note.h"

#include "Conversions.h"

#include <algorithm>
#include <cmath>
#include <iterator>

namespace vaw::multiple_sines {

Note::Note(std::int64_t start, float duration_seconds, std::string const & note_name) {
  start_millis = start;
  end_millis = start + static_cast<int64_t>(round(duration_seconds * 1000));
  frequency = NOTE_TO_FREQUENCY.at(note_name);
}

void Note::AddVolumePoint(std::int64_t time_millis, float volume) {
  time_volume_map[time_millis] = volume;
}

float Note::GetVolume(std::int64_t elapsed_time_millis) const {
  auto upper = time_volume_map.upper_bound(elapsed_time_millis);
  auto lower = std::prev(upper);

  std::uint64_t domain = upper->first - lower->first;
  float range = upper->second - lower->second;
  float slope = range / static_cast<float>(domain);
  std::uint64_t relative_elapsed = elapsed_time_millis - lower->first;
  float volume_difference = slope * relative_elapsed;
  
  return lower->second + volume_difference;
}

}  // namespace vaw::multiple_sines

