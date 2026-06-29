# 3D-Swarm

This repository contains the host software and embedded firmware for the 3D-Swarm / Octomag electromagnetic coil control stack.

## Repository Layout

```text
3D-Swarm/
|-- software/
|   `-- octomag/
|       |-- octomag.pro
|       |-- main.cpp
|       |-- mainwindow.*
|       |-- cameras.*
|       |-- coils.*
|       |-- mp285.*
|       |-- ni6343.*
|       `-- resources/
`-- firmware/
    |-- Realtime485.ioc
    |-- .mxproject
    |-- Core/
    |-- Drivers/
    |-- Hardware/
    |-- USB_DEVICE/
    |-- Middlewares/
    `-- MDK-ARM/
        `-- Realtime485.uvprojx
```

## Development Environment

### Host PC Application

The host application is a Qt/qmake C++ project.

Recommended tools:

- Windows development machine.
- Qt Creator.
- Qt 6.x with the MSVC 64-bit kit. The original local build directories indicate Qt 6.4.2 with MSVC2019 64-bit.
- Microsoft Visual Studio / MSVC toolchain matching the selected Qt kit.
- OpenCV 4.7.0 for Windows.
- NI-DAQmx with C external compiler support.

Project settings:

- Build system: qmake (`software/octomag/octomag.pro`).
- Language standard: C++17.
- Qt modules: `core`, `gui`, `widgets`, and `serialport`.
- OpenCV library expected by the project: `opencv_world470` for release and `opencv_world470d` for debug.
- NI library expected by the project: `NIDAQmx`.

### STM32 Firmware

The embedded controller is an STM32CubeMX-generated STM32F4 project with a Keil MDK build target.

Recommended tools:

- STM32CubeMX for editing or regenerating peripheral configuration from `firmware/Realtime485.ioc`.
- Keil uVision / Keil MDK-ARM for building `firmware/MDK-ARM/Realtime485.uvprojx`.
- Keil STM32F4 device family pack. The project file references `Keil.STM32F4xx_DFP.2.17.0`.
- ST-LINK or another supported STM32 programmer/debugger for flashing and debugging.

Target and firmware stack:

- MCU target: `STM32F411CEUx`.
- CPU family: ARM Cortex-M4.
- Firmware libraries: STM32F4 HAL, CMSIS, and STM32 USB Device Library.
- USB class: CDC device.
- Main generated peripherals used by the application include GPIO, USART1, TIM2, TIM3, TIM10, and USB FS.

## Host Software

Project entry point:

```text
software/octomag/octomag.pro
```

The host software is a C++ / Qt application for controlling and observing the Octomag setup.

Main modules:

- `mainwindow.*`: main Qt UI logic.
- `cameras.*`: OpenCV camera capture and image handling.
- `coils.*`: serial communication with the electromagnetic coil controller.
- `mp285.*`: serial communication with the MP-285 manipulator.
- `ni6343.*`: NI-6343 / NI-DAQmx analog output control.
- `resources.qrc` and `resources/`: Qt resources and UI icons.

Expected local dependencies:

- Qt Widgets
- Qt SerialPort
- MSVC 64-bit Qt kit
- OpenCV 4.7.0 for Windows
- NI-DAQmx C external compiler support

The Qt project file currently expects the external dependencies at:

```text
software/third_libs/opencv-4.7.0-windows/opencv/
software/third_libs/nidaqmx/ExternalCompilerSupport/C/
```

`software/third_libs/` is not committed because it contains large external SDK and binary files. To build locally, restore the original `third_libs` directory under `software/`, or update `software/octomag/octomag.pro` to point to installed OpenCV and NI-DAQmx paths on your machine.

Recommended build flow:

1. Open `software/octomag/octomag.pro` in Qt Creator.
2. Select an MSVC 64-bit Qt kit with Widgets and SerialPort support.
3. Confirm OpenCV and NI-DAQmx include/library paths.
4. Use an out-of-source build directory.

## Electromagnetic Coil Driver Code

The coil stack has host-side communication code and embedded driver code.

Host-side files:

- `software/octomag/coils.h`
- `software/octomag/coils.cpp`

These files handle Qt SerialPort communication with the coil controller from the PC application.

Embedded driver files:

- `firmware/Hardware/Inc/SAC6010.h`
- `firmware/Hardware/Src/SAC6010.c`

The SAC6010 driver code contains functions for current-output control, current mode setup, output enable/disable, and parsing commands received from the PC. The USB CDC receive path in `firmware/USB_DEVICE/App/usbd_cdc_if.c` passes received data into `sac6010_read_from_pc(...)`.

Runtime application flow:

- `firmware/Core/Src/main.c` initializes GPIO, USART1, USB device, and timers.
- TIM2 and TIM3 are used for PWM outputs.
- TIM10 is used for periodic realtime update handling.
- The firmware stores per-coil current state in `coils_current[8]` and output enable state in `coils_power`.

## STM32 Firmware

STM32CubeMX project:

```text
firmware/Realtime485.ioc
```

Keil MDK project:

```text
firmware/MDK-ARM/Realtime485.uvprojx
```

The firmware drives the electromagnetic coil controller hardware and exposes realtime communication paths through the configured STM32 peripherals.

Main directories:

- `Core/`: CubeMX-generated application code, peripheral setup, interrupts, and HAL MSP code.
- `Hardware/`: project-specific SAC6010 driver code.
- `USB_DEVICE/`: USB CDC device configuration and interface code.
- `Drivers/`: STM32F4 HAL, CMSIS, and related driver source files.
- `Middlewares/`: STM32 USB Device Library.
- `MDK-ARM/`: Keil project files, startup code, debug configuration, and RTE metadata.

Recommended build flow:

1. Open `firmware/MDK-ARM/Realtime485.uvprojx` in Keil uVision.
2. Build the `Realtime485` target for the STM32F411 configuration.
3. Keil build output is generated under `firmware/MDK-ARM/Realtime485/`.
4. Use `firmware/Realtime485.ioc` with STM32CubeMX when peripheral configuration must be regenerated.
