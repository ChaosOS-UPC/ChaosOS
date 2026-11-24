# ChaosOS

**A concurrent suite that intercepts I/O and trolls the Linux desktop, written in C.**

ChaosOS gradually and *controllably* alters the normal behaviour of a Linux system — console output, keyboard input, mouse, windows and notifications — to show how applications interact with the operating system. Everything is built on plain POSIX: library interposition (`LD_PRELOAD`), processes (`fork`/`exec`/`wait`), signals and threads.

> 🚧 **Work in progress.** Team project for the SIOP course at the Universitat Politècnica de Catalunya (UPC), 2026.

## Architecture

Seven fully decoupled modules: two shared libraries loaded with `LD_PRELOAD`, four worker processes and one orchestrator.

| Module | Source → output | What it does |
|---|---|---|
| Chaos Mode (orchestrator) | `chaos_launcher.c` → `./chaos_launcher` | Starts the workers with `fork()` + `execlp()`, raises the chaos level every few seconds with `alarm()` + `SIGUSR1`, sends `SIGUSR2` at the maximum level and stops everything cleanly on `SIGINT`. |
| Printf Troll | `printf_troll.c` → `lib_printf.so` | Intercepts `puts()` and `write()` on `fd == 1` to alter console output. |
| Keyboard Troll | `key_troll.c` → `lib_key.so` | Intercepts `read()` on `fd == 0` and randomly swaps some typed characters. |
| Mouse Troll | `mouse_troll.c` → `./mouse_troll` | Thread loop that nudges the cursor with `xdotool`, faster on every `SIGUSR1`. |
| Window Troll | `win_troll.c` → `./win_troll` | On `SIGUSR1`, moves the active window a few pixels, as if it were running away. |
| Notification & Sound Troll | `alert_troll.c` → `./alert_troll` | On `SIGUSR1`, forks two children in parallel: a sarcastic `notify-send` and an `aplay` sound. |
| Update Troll | `update_troll.c` → `./update_troll` | On `SIGUSR2` (maximum chaos), opens a fake full-screen "Updating system..." window with `zenity`. |

### Signal protocol

| Signal | Meaning |
|---|---|
| `SIGUSR1` | Increase the chaos level |
| `SIGUSR2` | Critical event (maximum chaos) |
| `SIGINT` | Emergency stop: terminate all workers cleanly |

Each worker can be tested on its own by sending these signals by hand: `kill -SIGUSR1 <PID>`.

## Build

```bash
make          # builds every module whose source is already in src/
make clean
```

## Usage

```bash
# Libraries (one command only)
LD_PRELOAD=./lib_key.so cat
LD_PRELOAD=./lib_printf.so ls

# Full demo
./chaos_launcher &
LD_PRELOAD="./lib_printf.so ./lib_key.so" bash
# ...Ctrl+C on the launcher stops everything
```

## Environment

Developed and tested on an **Ubuntu 22.04 / 24.04 virtual machine** (2 vCPUs, 4 GB RAM) with the **"Ubuntu on Xorg"** session: Wayland blocks external access to the cursor and window positions.

```bash
sudo apt install build-essential libomp-dev xdotool zenity libnotify-bin alsa-utils
```

### ⚠️ Safety

- Take a **VM snapshot** before testing.
- Use `LD_PRELOAD` only in front of a single command. **Never** `export` it in your shell, add it to `~/.bashrc` or touch `/etc/ld.so.preload`.

## Repository workflow

- `main` is protected: changes go in through pull requests.
- One branch per module: `chaos-launcher`, `printf-troll`, `key-troll`, `mouse-troll`, `win-troll`, `alert-troll`, `update-troll`.
- Sources go in `src/`.

## License

[MIT](LICENSE) © 2026 ChaosOS-UPC team

---

<details>
<summary>Català</summary>

## ChaosOS

**Suite concurrent en C que intercepta l'E/S i fa la guitza a l'escriptori de Linux.**

ChaosOS altera de manera progressiva i *controlada* el comportament normal d'un sistema Linux (sortida per consola, teclat, ratolí, finestres i notificacions) per mostrar com interactuen les aplicacions amb el sistema operatiu. Tot es basa en POSIX: interposició de llibreries (`LD_PRELOAD`), processos (`fork`/`exec`/`wait`), signals i threads.

> 🚧 **En desenvolupament.** Projecte en equip de l'assignatura SIOP de la Universitat Politècnica de Catalunya (UPC), 2026.

### Mòduls

| Mòdul | Font → sortida | Què fa |
|---|---|---|
| Chaos Mode (orquestrador) | `chaos_launcher.c` | Llança els treballadors amb `fork()` + `execlp()`, puja el nivell de caos amb `alarm()` + `SIGUSR1`, envia `SIGUSR2` al màxim i ho atura tot amb `SIGINT`. |
| Printf Troll | `printf_troll.c` → `lib_printf.so` | Intercepta `puts()` i `write()` (`fd == 1`). |
| Keyboard Troll | `key_troll.c` → `lib_key.so` | Intercepta `read()` (`fd == 0`) i canvia algunes tecles a l'atzar. |
| Mouse Troll | `mouse_troll.c` | Bucle amb threads que mou el cursor amb `xdotool`. |
| Window Troll | `win_troll.c` | Amb `SIGUSR1`, mou la finestra activa. |
| Notification & Sound Troll | `alert_troll.c` | Amb `SIGUSR1`, dos fills en paral·lel: `notify-send` i `aplay`. |
| Update Troll | `update_troll.c` | Amb `SIGUSR2`, una falsa actualització a pantalla completa amb `zenity`. |

### Protocol de signals
`SIGUSR1` = pujar el caos · `SIGUSR2` = esdeveniment crític · `SIGINT` = aturada d'emergència.

### Compilar i executar
`make` compila els mòduls que ja tenen el codi a `src/`. Exemples d'ús a l'apartat *Usage*.

### Entorn i seguretat
Màquina virtual Ubuntu 22.04/24.04 amb la sessió **"Ubuntu on Xorg"**. Fes un **snapshot** abans de provar res i no facis mai `export LD_PRELOAD`.

### Flux de treball
`main` protegida (tot entra per pull request) i una branca per mòdul.

</details>
