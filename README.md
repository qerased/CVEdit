## CVEdit — Qt/OpenCV filter editor for photos, videos, and webcam

CVEdit is a small desktop app written with C++ using Qt6 and OpenCV.
It applies a configurable chain of visual filters to images,
video files, and live webcam streams.
Each filter can be toggled on/off, re-ordered, and tuned in real time.

### Features
- Loading images and videos from disk
- Webcam capture
- Filter chain with ordering control (per-filter numeric order)
- Live parameter tweaking with immediate preview
- Take a screenshot of the current preview

### Available filters
- Grayscale
- Gaussian Blur (intensity via kernel size)
- Canny Edge Detection (two thresholds, Replace/Overlay modes)
- Glitch/Shake (animated warp; depends on tick number)
- Pixel Sort
  - Mode: Luminosity, Red, Green, Blue, Hue
  - Scope: Global, Rows, Cols
  - Axis: Horizontal, Vertical
  - Chunk/Stride: segment length and step
- Kuwahara (painterly effect; window size)
- Bloom (Threshold/Radius/Strength, custom glow color)

### Testing
- A test image is embedded as a Qt resource (`:/test.png`). Use File → "Show Test Image" to quickly try filters.

---

## Build

Requirements:
- **CMake 4.0+**
- **A C++20 compiler**
- **Qt 6** (Widgets)
- **OpenCV 4.x** with development headers

### Linux (e.g. Ubuntu/Debian)
Clone the repository and build:
```bash
git clone https://github.com/qerased/CVEdit.git
cd CVEdit
mkdir build && cd build
cmake ..
make
```

## Run & Usage
- File &rarr; Show Test Image: load the embedded sample image
- File &rarr; Open Image: open an image from disk
- File &rarr; Open Video: open a video file
- Source &rarr; Start Webcam: start camera
- Source &rarr; Stop Source: stop the current source
- Toolbar &rarr; Screenshot: save the current preview as PNG/JPEG

Filters panel (Dock):
- Each filter has an Enable checkbox and an Order field (integer; lower means earlier in the chain)
- Filter-specific sliders/controls update the preview immediately

---

## Project structure
- `src/main_window.*` — main UI, menus, filters dock, preview, event handling
- `src/video_source.*` — webcam/video input via `cv::VideoCapture`, FPS/size helpers
- `src/utils.*` — image loading via Qt I/O + `cv::imdecode`, `cv::Mat` &harr; `QImage/QPixmap`
- `src/filters/*` — base filter and implementations; `filter_chain` sorts and applies in order
- `resources/icons.qrc` — Qt resource file (includes `test.png`)
- `CMakeLists.txt` — Qt6 Widgets + OpenCV build configuration

---

## Roadmap / Ideas
- Alpha-channel support in `utils::load_image_mat`
- Optional downscale in preview for live sources; buffer reuse to reduce copies
- App icon and a small sample assets set
- Filter presets saving/loading
- Multithreading for faster rendering

## Examples

<table>
  <tr>
    <td>
      <img src="https://github.com/user-attachments/assets/829e5512-d0f9-41fd-bea7-68837e4ae17d" alt="example 1" width="100%" />
    </td>
    <td>
      <img src="https://github.com/user-attachments/assets/0075d64e-ac89-44fd-bc3a-130b302a9552" alt="example 2" width="100%" />
    </td>
  </tr>
  <tr>
    <td>
      <img src="https://github.com/user-attachments/assets/a85dd4a2-5fe2-4f90-bdea-23ca04a33a23" alt="example 3" width="100%" />
    </td>
    <td>
      <img src="https://github.com/user-attachments/assets/45908b0a-868b-4525-a187-71434a458cc5" alt="example 4" width="100%" />
    </td>
  </tr>
</table>
