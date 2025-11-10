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
- Save/Load filter presets

### Available filters
- Grayscale
- Gaussian Blur
- Canny Edge Detection
- Shake
- Pixel Sort
- Kuwahara
- Bloom
- Tint/Colorize
- Vignette
- Film Noise
- Scanlines
- Pixel Displace
- Gradient Overlay

### Available presets
- Old TV
- Digital Collapse
- Dreamy colors
- Cyberpunk Neon
- Horror Cam
- Noir
- Painting
- Vaporwave

### Testing
- A test image is embedded as a Qt resource (`:/test.png`). Use File → "Show Test Image" to quickly try filters.
- Test filter presets are embedded as a Qt resource (`:/presets/*.json`). Use presets dock menu to access them.

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
- Multithreading for faster rendering

## Example

![example](https://s679sas.storage.yandex.net/rdisk/da866c7ccb584db7ffe2948856587d1daf3397aeaf86688b67883ada9966885d/6911d2b8/FODcF8SSEmHaLV-6-ylB84CUlKL9BtFjKuOLnIUq6G1QgL2mj-C8dUlQKG9g5sj8k122Ii6VcM5RmXZw1Cc1ww==?uid=0&filename=cveeeeedit.gif&disposition=inline&hash=&limit=0&content_type=image%2Fgif&owner_uid=0&fsize=41505477&hid=9c4a1e6b29b65f2c9db35db008adfc0d&media_type=image&tknv=v3&etag=f560dafdbe91840e277b6ac07f825af2&ts=6433c35901e00&s=04cf8aa81b1f469f2f32fa69c27a82eeed828dcab8e3b94a87c9e482d83f0a78&pb=U2FsdGVkX1-vUb-l8enFhwuVCRnLygXLrwxhhX2DTnVF-MuIOnEX_pIkzAq-DuNmjjsKTMhMMHTCQcaS4FFFhjBRWcmnqD891KhtEootdmE)