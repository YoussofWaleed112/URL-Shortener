# URL Shortener Service

A command-line URL shortener written in C++17. Uses only the standard library — no external dependencies.

## Build

```bash
make
```

Requires g++ with C++17 support. The `data/` directory is created automatically.

## Run

```bash
./url-shortener
```

## Sample Session

```
  ╔══════════════════════════════╗
  ║     URL Shortener Service    ║
  ╚══════════════════════════════╝

  1. Shorten a URL
  2. Resolve a short URL
  3. Exit

  Choice: 1
  Enter long URL: https://github.com/user/project/issues/154
  Short URL: https://gh/154

  Choice: 1
  Enter long URL: https://meet.google.com/byq-qkkd-gud
  Short URL: https://mt-gogle/byq

  Choice: 1
  Enter long URL: not-a-url
  Error: invalid URL.
  URL must start with http:// or https://, contain a dot in the
  domain, and have no spaces.

  Choice: 2
  Enter short URL: https://gh/154
  Original URL: https://github.com/user/project/issues/154

  Choice: 1
  Enter long URL: https://github.com/user/project/issues/154
  Short URL: https://gh/154    ← same short URL returned (no duplicate)

  Choice: 3
  Goodbye.
```

## Storage

Mappings are saved to `data/mappings.txt` as tab-separated pairs:

```
https://github.com/user/project/issues/154	https://gh/154
https://meet.google.com/byq-qkkd-gud	        https://mt-gogle/byq
```

Previously created short URLs persist across runs.

## Project Structure

```
url-shortener/
├── src/
│   ├── main.cpp          # CLI entry point
│   ├── UrlShortener.h    # Class declaration
│   ├── UrlShortener.cpp  # Core shortening & resolution logic
│   ├── validator.h       # URL validation interface
│   └── validator.cpp     # Validation implementation
├── data/
│   └── mappings.txt      # Auto-created on first run
├── Makefile
└── README.md
```

## How Short Codes Are Generated

1. **Domain abbreviation** — a lookup table covers ~20 common domains (`github.com` → `gh`, `youtube.com` → `yt`, etc.). Unknown domains have vowels stripped and are truncated to 4 characters.
2. **Path slug** — path segments are scanned right-to-left. Numeric IDs are preferred (e.g. `/issues/154` → `154`). Otherwise the first 3–4 characters of the most meaningful segment are used.
3. **Collision handling** — if two different long URLs produce the same short code, a numeric suffix is appended (`/abc` → `/abc2`, `/abc3`, …).
