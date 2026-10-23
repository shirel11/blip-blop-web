const { chromium } = require('playwright');
(async () => {
  const browser = await chromium.launch({ headless: true });
  const page = await browser.newPage({ viewport: { width: 1200, height: 630 }, deviceScaleFactor: 1 });
  await page.goto('http://127.0.0.1:8083/resources/share-card.svg');
  await page.screenshot({ path: 'web/public/resources/share.png', omitBackground: false });
  await browser.close();
})().catch((error) => { console.error(error); process.exitCode = 1; });
