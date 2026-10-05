#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "pipewire_stable_id.h"

#define PIPEWIRE_ID_NAMESPACE UINT64_C(0x4000000000000000)
#define PIPEWIRE_ID_SIGN_BIT  UINT64_C(0x8000000000000000)

struct stable_id_test_vector
{
  const char *node_name;
  uint64_t expected;
};

static int
check_id(const char *description, uint64_t actual, uint64_t expected)
{
  if (actual == expected)
    return 0;

  fprintf(stderr, "%s: got %" PRIu64 " (0x%016" PRIx64
                  "), expected %" PRIu64 " (0x%016" PRIx64 ")\n",
          description, actual, actual, expected, expected);
  return -1;
}

int
main(void)
{
  static const struct stable_id_test_vector vectors[] = {
    {
      "alsa_output.usb-C-Media_Electronics_Inc._USB_Audio_Device-00.analog-stereo",
      UINT64_C(5919947916309331545),
    },
    {
      "alsa_output.pci-0000_00_1f.3.analog-stereo",
      UINT64_C(5252894625094985996),
    },
  };
  uint64_t id;
  uint64_t repeat;
  uint64_t similar;
  size_t i;

  if (check_id("NULL input", pipewire_stable_id_from_name(NULL), 0) < 0)
    return 1;
  if (check_id("empty input", pipewire_stable_id_from_name(""), 0) < 0)
    return 1;

  for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++)
    {
      id = pipewire_stable_id_from_name(vectors[i].node_name);
      if (check_id(vectors[i].node_name, id, vectors[i].expected) < 0)
        return 1;

      repeat = pipewire_stable_id_from_name(vectors[i].node_name);
      if (check_id("determinism", repeat, id) < 0)
        return 1;

      if (!(id & PIPEWIRE_ID_NAMESPACE) || (id & PIPEWIRE_ID_SIGN_BIT))
        {
          fprintf(stderr, "ID outside the positive signed-63-bit PipeWire namespace: 0x%016" PRIx64 "\n", id);
          return 1;
        }

      printf("ok: %" PRIu64 " 0x%016" PRIx64 " %s\n",
             id, id, vectors[i].node_name);
    }

  similar = pipewire_stable_id_from_name(
    "alsa_output.pci-0000_00_1f.3.analog-sterep");
  if (similar == vectors[1].expected)
    {
      fprintf(stderr, "Similar node names produced the same ID\n");
      return 1;
    }

  puts("ok: NULL/empty, determinism, similar names and namespace bits");
  return 0;
}
