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


## About this fork

This repository is a fork of the upstream
[OwnTone server](https://github.com/owntone/owntone-server).

The default `master` branch is intentionally kept as the upstream baseline.
The fork-specific implementation used in production lives on the
[`pipewire-native`](https://github.com/Topolynx/owntone-server/tree/pipewire-native)
branch.

The main goal of that branch is to make OwnTone's native PipeWire backend work
cleanly in installations where several physical PipeWire sinks must appear as
independent OwnTone outputs and where those outputs are consumed by Home
Assistant.

Compared with upstream, the fork-specific PipeWire work adds:

- deterministic, stable OwnTone output IDs derived from each PipeWire sink's
  `node.name`, instead of relying on volatile PipeWire runtime object IDs;
- observation of the PipeWire registry so sinks can disappear and reappear
  without losing their logical identity;
- stable resolution from an OwnTone sink ID to the sink's current PipeWire
  runtime object ID;
- hardened stream restart and hot-unplug behaviour for targeted sinks; and
- an opt-in multisink mode that exposes each valid PipeWire `Audio/Sink` as
  an independent OwnTone output.

Multisink mode is enabled with:

```conf
audio {
    type = "pipewire"
    mixer = "pwstream"
    pipewire_multisink = true
}
```

With `pipewire_multisink` omitted or set to `false`, the previous
single-output PipeWire behaviour remains the default.

### Why stable sink IDs matter

PipeWire global object IDs are runtime handles. They can change when a device
is disconnected, rediscovered or recreated by the audio stack.

Home Assistant's OwnTone/forked-daapd integration uses the OwnTone output ID as
part of the entity unique ID. If a physical audio device is exposed through a
volatile PipeWire ID, the same sink can therefore be registered repeatedly as
new Home Assistant entities.

The fork derives a deterministic ID from the sink identity instead, so a
rediscovered device such as `USB Audio` or `HDMI Output` keeps the same
OwnTone output identity.

### Docker, Avahi and Shairport Sync

The tested deployment also includes Home Assistant, a separate Shairport Sync
receiver, and OwnTone running in a host-network Docker container.

When the Linux host already runs Avahi, starting additional Avahi daemons
inside host-network containers can cause hostname conflicts and repeated mDNS
collision/registration cycles. That can make AirPlay outputs disappear and
reappear and may leave stale Home Assistant entities behind.

The companion
[owntone-pulseaudio-docker](https://github.com/Topolynx/owntone-pulseaudio-docker)
repository contains the production container setup used with this fork. It
supports native PipeWire multisink output and an opt-in
`OWNTONE_EXTERNAL_AVAHI=1` mode, where OwnTone uses the host Avahi daemon
through the host system D-Bus socket instead of starting a competing Avahi
daemon in the container.

That Avahi/Docker integration belongs to the companion container repository,
not to the OwnTone server patch itself.

This fork-specific work has been validated in the described
PipeWire + Home Assistant + Shairport Sync deployment. It is not an upstream
OwnTone guarantee, and upstream may evolve independently.


## Looking for help?

Visit the [OwnTone documentation](https://owntone.github.io/owntone-server/) for
usage and set up instructions, API documentation, etc.

If you are looking for information on how to get and install OwnTone, then see
the [Installation](https://owntone.github.io/owntone-server/installation/)
instructions.
