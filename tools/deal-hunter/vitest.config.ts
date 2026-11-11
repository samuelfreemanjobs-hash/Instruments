import { defineConfig } from "vitest/config";

export default defineConfig({
  test: {
    include: ["server/**/*.test.ts", "web/src/lib/**/*.test.ts"],
  },
});
