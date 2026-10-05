#include <stdint.h>
#include <string.h>

#include "pipewire_stable_id.h"

#define PIPEWIRE_ID_NAMESPACE UINT64_C(0x4000000000000000)
#define PIPEWIRE_ID_HASH_MASK UINT64_C(0x3fffffffffffffff)
#define FNV1A_64_OFFSET_BASIS UINT64_C(0xcbf29ce484222325)
#define FNV1A_64_PRIME        UINT64_C(0x100000001b3)

static uint64_t
fnv1a_64_update(uint64_t hash, const unsigned char *data, size_t len)
{
  while (len--)
    {
      hash ^= *data++;
      hash *= FNV1A_64_PRIME;
    }

  return hash;
}

uint64_t
pipewire_stable_id_from_name(const char *node_name)
{
  static const unsigned char stable_id_prefix[] = "owntone:pipewire-sink:v1";
  uint64_t hash;

  if (!node_name || node_name[0] == '\0')
    return 0;

  hash = fnv1a_64_update(FNV1A_64_OFFSET_BASIS,
                         stable_id_prefix, sizeof(stable_id_prefix));
  hash = fnv1a_64_update(hash, (const unsigned char *)node_name,
                         strlen(node_name));

  return PIPEWIRE_ID_NAMESPACE | (hash & PIPEWIRE_ID_HASH_MASK);
}
