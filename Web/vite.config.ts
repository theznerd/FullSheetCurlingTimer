import { defineConfig } from "vite";

export default defineConfig({
  // Makes built asset paths relative, which is the most portable default
  // for GitHub Pages / simple static hosts.
  base: "./",
});
