# PWR

A small APPLaunch app for **Cardputer Zero**: Restart or Shut down with confirmation.

It runs the usual `systemctl reboot` / `systemctl poweroff` path so in a sense, it is just a bloated launcher wrapper. the power button should shut the system first, but sometimes we need that feeling that it is done the right way.

<p align="center">
  <img src="./screenshot/store-home-dark.png" alt="PWR home screen" width="320" />
  &nbsp;
  <img src="./screenshot/store-confirm-dark.png" alt="PWR confirm screen" width="320" />
</p>

## Controls

| Key | Home | Confirm |
| --- | --- | --- |
| `Left` / `Right` (or `Z` / `C`) | Focus Restart / Shut down | — |
| `Enter` | Open confirm for the focused action | Confirm |
| `6` | Toggle light / dark | — |
| hold `ESC` / `4` | Exit | — |
| `ESC` | — | Cancel |


## Install

Grab an arm64 `.deb` from [Releases](https://github.com/mohamedation/pwr/releases) or the latest CI **pwr-arm64-deb** artifact, then on the device:

```shell
sudo apt install --no-install-recommends ./PWR_0.1.0_rev1_arm64.deb
```

## License

MIT — see [LICENSE](./LICENSE). Third-party font licenses are in `assets/fonts/LICENSE.txt`.
