# Full Sheet Curling Timer — Web

Minimal single-page web UI intended for static hosting (GitHub Pages or similar) and using the Web Bluetooth API.

## Prerequisites

- Node.js 18+ (or newer)
- A Web Bluetooth-capable browser (typically Chromium-based)
- HTTPS is required for Web Bluetooth (localhost is allowed)

## Develop

From this folder:

```bash
npm install
npm run dev
```

Then open the URL Vite prints.

## Build (static)

```bash
npm run build
```

Vite outputs the static site to `dist/`.

## GitHub Pages notes

- This project uses `base: "./"` in `vite.config.ts` so the built site works when hosted from a sub-path.
- Web Bluetooth will only work on GitHub Pages over HTTPS.
