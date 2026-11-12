# Hardware & Execution Specifications

## Project: My Preprocessor (`mypreprocessor`)
**Author:** Ajay Murugan  
**Target Category:** Systems Programming / Embedded Systems Tooling  

---

## 1. System & Hardware Context

The project is an implementation of a C preprocessor tool (`mypreprocessor`) designed to parse, transform, and expand C source files prior to downstream compilation.

In the context of the curriculum guidelines requiring hardware verification:
- **Project Type:** User-space Systems Utility & Compiler Toolchain Component.
- **Circuit Requirements:** As this project operates purely at the software, runtime, and OS file-system level, **no external discrete electronic circuits, breadboard wiring, or microcontroller GPIO interfacing** are required.
- **Hardware Platform:** Can be deployed and verified on standard host workstations (x86_64) as well as embedded development boards (ARM Cortex-A series, such as Raspberry Pi 3/4/5, BeagleBone Black, or STM32MP1 running Embedded Linux).

---

## 2. Hardware Verification Environment

| Parameter | Host Development Platform | Embedded Hardware Target (e.g., Raspberry Pi) |
| :--- | :--- | :--- |
| **Processor Architecture** | x86_64 (Intel / AMD) | ARMv7 / ARMv8 (Cortex-A53 / Cortex-A72) |
| **Operating System** | Ubuntu 20.04 / 22.04 LTS, Debian, or WSL2 | Raspberry Pi OS (Debian-based) / Yocto Linux |
| **RAM Utilization** | < 4 MB active process memory | < 4 MB active process memory |
| **Disk Storage** | < 2 MB (Executable binary + temporary buffers) | < 2 MB |
| **Toolchain** | GCC (GNU Compiler Collection), GNU Make | GNU Arm Embedded Toolchain / Native GCC |

---

## 3. Hardware Execution Verification

1. **Compilation on Target Hardware:**
   ```bash
   make clean
   make
   ```
2. **Execution & File I/O Verification:**
   ```bash
   ./mypreprocessor inputfile.c
   ```
3. **Verification Output:**
   - The process reads from flash/disk storage via `fopen()`.
   - Utilizes RAM-backed temporary streams (`tmpfile()`) for low-latency multi-pass parsing.
   - Outputs the generated `.i` file to the target filesystem.
