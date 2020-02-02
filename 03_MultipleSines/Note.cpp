#include "Note.h"

#include "Conversions.h"

#include <iterator>

namespace vaw::multiple_sines {

Note::Note(std::uint64_t start, float duration_seconds, std::string const & note_name) {
  start_millis = start;
  end_millis = start + static_cast<uint64_t>(round(duration_seconds * 1000));
  frequency = NOTE_TO_FREQUENCY.at(note_name);
}

void Note::AddVolumePoint(std::uint64_t time_millis, float volume) {
  time_volume_map[time_millis] = volume;
}

float Note::GetVolume(std::uint64_t elapsed_time_millis) const {
  auto lower = time_volume_map.lower_bound(elapsed_time_millis);
  auto upper = std::next(lower);

  std::uint64_t domain = upper->first - lower->first;
  float range = upper->second - lower->second;
  std::uint64_t relative_elapsed = elapsed_time_millis - lower->first;

  float scalar = static_cast<float>(relative_elapsed) / static_cast<float>(domain);
  return range * scalar;
}

}  // namespace vaw::multiple_sines

