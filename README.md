# OwnTone

OwnTone is a media server that lets you play audio sources such as local files,
Spotify, pipe input or internet radio to AirPlay 1 and 2 receivers, Chromecast
receivers, Roku Soundbridge, a browser or the server’s own sound system. Or you
can listen to your music via any client that supports mp3 streaming.

You control the server via a web interface, Apple Remote, an Android remote
(e.g. Retune), an MPD client, json API or DACP.

OwnTone also serves local files via the Digital Audio Access Protocol (DAAP) to
iTunes (Windows), Apple Music (macOS) and Rhythmbox (Linux), and via the Roku
Server Protocol (RSP) to Roku devices.

Runs on Linux, BSD and macOS.

OwnTone was previously called forked-daapd, which again was a rewrite of
mt-daapd (Firefly Media Server).


## Fork-specific native PipeWire multisink support

This fork contains an experimental extension to OwnTone's native PipeWire
backend for installations that need PipeWire sinks to appear as independent
OwnTone outputs. It has been validated in a Home Assistant and PipeWire setup;
upstream OwnTone may evolve independently.

The server-side changes in this repository add:

- deterministic output IDs derived from each sink's PipeWire `node.name`;
- observation of the PipeWire registry and resolution from a stable sink ID
  to its current runtime PipeWire object ID;
- fail-closed stream restart and hot-unplug handling for targeted sinks; and
- optional publication of each valid PipeWire `Audio/Sink` as a separate
  OwnTone output.

Multisink mode is opt-in. Enable it with the native PipeWire backend and the
per-stream mixer:

```conf
audio {
    type = "pipewire"
    mixer = "pwstream"
    pipewire_multisink = true
}
```

`mixer = "pwstream"` is required in multisink mode so that every OwnTone
output controls only its own stream volume. With `pipewire_multisink` omitted
or set to `false`, the previous single-output behaviour remains the default:
WirePlumber routes that output to the system default sink.

PipeWire global object IDs are runtime handles and can change when a device is
removed and rediscovered. Home Assistant's OwnTone/forked-daapd integration
uses the OwnTone output ID as part of an entity's unique ID, so exposing the
volatile PipeWire ID could register one physical device repeatedly as multiple
entities. This fork instead derives a deterministic ID from the sink identity,
allowing a rediscovered sink such as `USB Audio` or `HDMI Output` to retain the
same OwnTone output ID.

### Deployment note: host-network Docker, Avahi and Shairport Sync

A tested deployment runs OwnTone and, optionally, Shairport Sync in Docker
containers with `network_mode: host` while the Linux host provides Avahi.
Running a separate Avahi daemon inside each host-network container can cause
hostname conflicts and repeated client collision/registration cycles. Those
mDNS disruptions can make AirPlay outputs disappear and reappear, potentially
leaving stale Home Assistant entities.

The deployment solution is to use the host's Avahi daemon as the single mDNS
daemon and let containerized applications reach it through the host system
D-Bus socket. Independent Avahi daemons should not be started inside those
host-network containers.

This host-Avahi integration is not implemented by the OwnTone source changes
in this repository. See the companion
[owntone-pulseaudio-docker](https://github.com/Topolynx/owntone-pulseaudio-docker)
repository for the Docker/OpenRC `OWNTONE_EXTERNAL_AVAHI` setup.


## Looking for help?

Visit the [OwnTone documentation](https://owntone.github.io/owntone-server/) for
usage and set up instructions, API documentation, etc.

If you are looking for information on how to get and install OwnTone, then see
the [Installation](https://owntone.github.io/owntone-server/installation/)
instructions.
