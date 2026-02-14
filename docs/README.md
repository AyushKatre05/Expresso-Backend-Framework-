# Expresso documentation

This folder contains the **documentation UI** for Expresso.

## Viewing the docs

1. **Open in browser (static):** Open `index.html` in your browser (file:// or any static server).
2. **Served by Expresso:** From the repo root, run:
   ```bash
   ./expresso-server --docs ./docs
   ```
   Then visit **http://localhost:4221/docs**.

## Contents

- **Overview** — What Expresso is and quick start.
- **How it works** — Request flow, components, and data flow behind the scenes.
- **Use like Express** — Mapping Express concepts to Expresso and adding routes.
- **API reference** — CLI options and built-in endpoints.
- **Roadmap & additions** — Recommended and required extensions (router, middleware, static files, etc.).
