# AURAVOX Pro

Cross-platform JUCE 8 vocal effects VST3 for Windows and macOS. This repository hosts the source, automated builds, and installation instructions.

**Prototype:** vocal high-pass EQ, presence EQ, compressor, saturation, filtered echo, input/output, wet/dry, bypass, meter and presets. **No pitch correction yet.**

Download a completed VST3 from the successful **Build AURAVOX Pro VST3** workflow under **Actions → Artifacts**. Extract `AURAVOX Pro.vst3` and copy the full folder to `C:\Program Files\Common Files\VST3`, then use FL Studio's plugin manager to scan for plugins. Windows builds are unsigned. Do not claim that a binary is verified until CI succeeds.

If you have Visual Studio 2022 with C++, CMake, and Git, build locally with `cmake -S . -B build/windows -G "Visual Studio 17 2022" -A x64` followed by `cmake --build build/windows --config Release --target AURAVOXPro_VST3`.

This project uses JUCE, subject to JUCE licensing terms. macOS builds are unsigned and may require local security approval; commercial distribution requires appropriate signing/notarization.
