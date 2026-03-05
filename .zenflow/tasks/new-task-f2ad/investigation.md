# Investigation: Mouse Not Responding in CS2 Menu

## Bug Summary

The ImGui GUI opens and closes with INSERT but does not respond to mouse input
(cannot click buttons, move the cursor over controls, etc.).

---

## Root Cause Analysis

After reading all source files the root cause is **two compounding bugs** in
`src/ImGui_Renderer.cpp`:

### Bug 1 — Infinite Recursion in the SDL Hook (CRITICAL)

**File**: `src/ImGui_Renderer.cpp` → `HookedSDL_PollEvent`

```cpp
static int HookedSDL_PollEvent(void* event)
{
    // BUG: oSDL_PollEvent points to the address that was PATCHED with
    // a JMP back to HookedSDL_PollEvent → infinite recursion / stack overflow
    int result = oSDL_PollEvent(event);   // <-- WRONG
    ...
}
```

`oSDL_PollEvent` is stored as the *address* of `SDL_PollEvent` in the target
DLL **before** the bytes are overwritten. After `InstallSDLHook()` patches
those bytes with `JMP → HookedSDL_PollEvent`, calling `oSDL_PollEvent(...)` is
equivalent to calling `HookedSDL_PollEvent(...)` again → stack overflow on the
first mouse event.

The code already has a correct helper `CallOriginalSDLPollEvent` that saves/
restores the original bytes before calling the real function, but it is **never
used inside the hook handler**.

**Fix**: Replace `oSDL_PollEvent(event)` with `CallOriginalSDLPollEvent(event)`
in `HookedSDL_PollEvent`.

---

### Bug 2 — Wrong SDL Struct Offsets for SDL3 (CRITICAL)

**File**: `src/ImGui_Renderer.cpp` — hard-coded byte offsets inside `HookedSDL_PollEvent`

CS2 (since mid-2023) ships with **SDL3**, not SDL2. The struct layouts changed.
The code uses SDL2 offsets throughout:

| Field | Code offset | SDL2 offset | SDL3 offset |
|---|---|---|---|
| `SDL_MouseMotionEvent.x` | +20 | +20 ✓ | **+28** ✗ |
| `SDL_MouseMotionEvent.y` | +24 | +24 ✓ | **+32** ✗ |
| `SDL_MouseButtonEvent.button` | +16 | +16 ✓ | **+24** ✗ |
| `SDL_MouseWheelEvent.y (float)` | +20 | +20 ✓ | **+28** ✗ |

Because the offsets are wrong, even if the infinite recursion were fixed, the
`io.MousePos` coordinates read from the event would be garbage values, and the
button byte would be read from the wrong field.

SDL3 structs have an extra `Uint32 reserved` field at +4 and a `Uint64
timestamp` at +8 which push all subsequent fields forward by 8 bytes compared
to SDL2.

---

### Bug 3 — `ImGui_ImplWin32_Init` Is Never Called (secondary)

**File**: `src/ImGui_Renderer.cpp` → `InitImGui`

`imgui_impl_win32.h` is included and `ImGui_ImplWin32_WndProcHandler` is
declared, but `ImGui_ImplWin32_Init(hWnd)` is never called and
`ImGui_ImplWin32_NewFrame()` is never called in `Render`. This means there is
no Win32 fallback for keyboard/mouse input. If the SDL hook fails, nothing
works.

---

## Affected Components

- `src/ImGui_Renderer.cpp` — all three bugs are here
  - `HookedSDL_PollEvent` (bugs 1 and 2)
  - `InitImGui` (bug 3)
  - `Render` (bug 3 — missing `ImGui_ImplWin32_NewFrame()`)

---

## Proposed Solution

### Fix 1 — Use `CallOriginalSDLPollEvent` in the hook (fixes Bug 1)

```cpp
static int HookedSDL_PollEvent(void* event)
{
    int result = CallOriginalSDLPollEvent(event);  // was: oSDL_PollEvent(event)
    ...
}
```

### Fix 2 — Correct SDL3 struct offsets (fixes Bug 2)

Detect whether `SDL2.dll` or `SDL3.dll` is loaded and use the appropriate
offsets, or always use SDL3 offsets since CS2 ships SDL3:

```cpp
// SDL3 offsets
case SDL_MOUSEMOTION_EVENT:
{
    float x = *reinterpret_cast<float*>((char*)event + 28);  // was +20
    float y = *reinterpret_cast<float*>((char*)event + 32);  // was +24
    io.AddMousePosEvent(x, y);
    *reinterpret_cast<uint32_t*>(event) = 0;
    break;
}
case SDL_MOUSEBUTTONDOWN:
{
    uint8_t button = *reinterpret_cast<uint8_t*>((char*)event + 24);  // was +16
    ...
}
case SDL_MOUSEBUTTONUP:
{
    uint8_t button = *reinterpret_cast<uint8_t*>((char*)event + 24);  // was +16
    ...
}
case SDL_MOUSEWHEEL_EVENT:
{
    float wheelY = *reinterpret_cast<float*>((char*)event + 28);  // was +20
    io.AddMouseWheelEvent(0.f, wheelY);
    *reinterpret_cast<uint32_t*>(event) = 0;
    break;
}
```

Also prefer the newer `io.Add*Event()` API over directly writing `io.MouseDown[]`
and `io.MousePos` (which is the legacy path and may be ignored in newer ImGui
versions compiled with `IMGUI_DISABLE_OBSOLETE_FUNCTIONS`).

### Fix 3 — Initialize and call the Win32 backend (fixes Bug 3, recommended)

In `InitImGui` after the SDL hook:
```cpp
ImGui_ImplWin32_Init(hWnd);
```

In `Render` before `ImGui_ImplDX11_NewFrame()`:
```cpp
ImGui_ImplWin32_NewFrame();
```

In `Shutdown`:
```cpp
ImGui_ImplWin32_Shutdown();
```

This provides a reliable second layer of input via `WM_MOUSEMOVE` / `WM_LBUTTONDOWN`
etc., and also correctly updates `io.DisplaySize` every frame.

---

## Edge Cases and Side Effects

- **SDL DLL name**: the code already checks both `SDL2.dll` and `SDL3.dll`.
  If `GetModuleHandleA` returns `nullptr` for both, `InstallSDLHook` silently
  does nothing and the menu has no mouse input. A log/warning should be added.
- **SDL3 float coords**: In SDL3, `x`/`y` in motion events are `float`, not
  `int`. The cast in the fix above already uses `float*`.
- **ImGui AddMouseButtonEvent vs MouseDown[]**: Use `io.AddMouseButtonEvent(idx, true/false)`
  (modern API) instead of directly writing `io.MouseDown[]`.
- **Thread safety**: The SDL hook fires on CS2's main thread (same as the
  render hook), so no additional locking is needed.
