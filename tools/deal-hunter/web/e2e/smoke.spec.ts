import { test, expect } from "@playwright/test";

test.describe("Black Friday Madness smoke", () => {
  test("search → product intel", async ({ page }) => {
    await page.goto("/search");
    await expect(page.getByRole("heading", { name: /Universal product search/i })).toBeVisible();
    await page.waitForTimeout(2500);
    const productLink = page.locator('a[href^="/product/"]').first();
    await expect(productLink).toBeVisible({ timeout: 15_000 });
    await productLink.click();
    await expect(page.getByRole("link", { name: /Back to search/i })).toBeVisible({ timeout: 15_000 });
    await expect(page.getByText(/% confidence/i).first()).toBeVisible();
  });

  test("wishlist route loads", async ({ page }) => {
    await page.goto("/wishlist");
    await expect(page.getByRole("heading", { name: /Wishlist/i })).toBeVisible();
  });

  test("compare pricing agent", async ({ page }) => {
    await page.goto("/compare");
    await page.getByPlaceholder(/Ultraboost/i).fill("dyson");
    await page.getByRole("button", { name: /Find best deal/i }).click();
    await expect(page.getByText(/value|Dyson|match/i).first()).toBeVisible({ timeout: 15_000 });
  });
});
