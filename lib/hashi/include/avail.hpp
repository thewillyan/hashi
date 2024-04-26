#ifndef HASHI_AVAIL
#define HASHI_AVAIL

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

// NOTE: availability file
// - 2 first bytes are number of entries in the table (unsigned short).
// - the following 1 bits represents that the entry is marked as deleted.
// - if they are marked as deleted, therefore thery are available as storage.
class AvailFileParser {
private:
  std::uint16_t num_entries;
  std::string avail_file;

public:
  AvailFileParser(std::string file_path);

  unsigned short get_num_entries() const;
  std::optional<size_t> get_avail_idx() const;
  std::vector<size_t> get_all_unavail() const;

  void incr_num_entries();
  void set_as_avail(size_t idx) const;
  void set_as_unavail(size_t idx) const;
};

#endif
