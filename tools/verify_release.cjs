const { chromium, webkit, devices } = require('playwright');
const fs = require('node:fs');
const assert = require('node:assert/strict');
const base = process.env.GAME_URL || 'http://127.0.0.1:8083/';
fs.mkdirSync('output/release', { recursive: true });
async function state(page) { return page.evaluate(() => JSON.parse(window.render_game_to_text())); }
async function play(page) {
  await page.click('#start');
  await page.waitForFunction(() => JSON.parse(window.render_game_to_text()).phase === 'ready', {}, { timeout: 120000 });
  assert.equal(await page.evaluate(() => typeof Module._web_key), 'function');
  await page.click('#start');
  await page.waitForTimeout(600);
  await page.keyboard.press('Escape', { delay: 150 });
  await page.waitForTimeout(500);
  for (let i = 0; i < 20; i++) {
    if ((await state(page)).phase === 'error') throw new Error(await page.evaluate(() => document.querySelector('#status').textContent + '\n' + Module.FS.readFile('/BlipBlop.log', {encoding:'utf8'})));
    if ((await state(page)).player) {
      // The first state is published before the player has fallen into the
      // level and before the opening dialogue has yielded control.
      for (let j = 0; j < 20 && (await state(page)).player.y < 430; j++) {
        await page.keyboard.press('Enter', { delay: 150 });
        await page.waitForTimeout(400);
      }
      return;
    }
    await page.keyboard.press('Enter', { delay: 150 });
    await page.waitForTimeout(600);
  }
  throw new Error('Opening level did not start: ' + JSON.stringify(await state(page)));
}
async function run(type, mobile = false) {
  const browser = await type.launch({ headless: true });
  try {
    const context = await browser.newContext(mobile ? { ...devices['iPhone 15 Pro'], viewport: { width: 393, height: 852 } } : { viewport: { width: 1200, height: 1000 } });
    const page = await context.newPage();
    const errors = [];
    page.on('pageerror', (error) => errors.push(String(error)));
    page.on('console', (message) => { if (message.type() === 'error' || message.text().includes('Engine startup')) console.log(type.name(), message.type(), message.text()); });
    await page.goto(new URL('?lang=en', base).href);
    assert.equal(await page.locator('html').getAttribute('lang'), 'en');
    await page.click('#language');
    assert.equal(await page.locator('html').getAttribute('dir'), 'rtl');
    await page.reload();
    assert.equal(await page.locator('html').getAttribute('lang'), 'he');
    await page.click('#language');
    await page.screenshot({ path: `output/release/${mobile ? 'phone' : 'desktop'}-start.png` });
    await play(page);
    const before = await state(page);
    console.log(type.name(), 'before', before);
    if (mobile) {
      await page.setViewportSize({ width: 852, height: 393 });
      await page.waitForTimeout(400);
      const pad = await page.locator('[data-direction="ArrowRight"]').boundingBox();
      await page.mouse.move(pad.x + pad.width / 2, pad.y + pad.height / 2);
      await page.mouse.down();
      await page.waitForFunction((x) => JSON.parse(window.render_game_to_text()).player.x > x + 30, before.player.x, { timeout: 10000 });
      assert((await state(page)).pressed.includes('ArrowRight'));
      await page.mouse.up();
      await page.waitForTimeout(150);
      assert.deepEqual((await state(page)).pressed, []);
      const geometry = await page.evaluate(() => ({
        screen: document.querySelector('#screen').getBoundingClientRect().toJSON(),
        left: document.querySelector('.movement-panel').getBoundingClientRect().toJSON(),
        right: document.querySelector('.action-panel').getBoundingClientRect().toJSON(),
        width: innerWidth, scroll: document.documentElement.scrollWidth
      }));
      assert.equal(geometry.width, geometry.scroll);
      assert(geometry.left.right <= geometry.screen.left + 1);
      assert(geometry.right.left >= geometry.screen.right - 1);
      console.log('Landscape geometry', geometry);
    } else {
      await page.keyboard.down('ArrowRight'); await page.keyboard.down('z');
      await page.waitForFunction((x) => JSON.parse(window.render_game_to_text()).player.x > x + 30, before.player.x, { timeout: 10000 });
      await page.keyboard.up('ArrowRight'); await page.keyboard.up('z');
    }
    const moved = await state(page);
    assert(moved.player.x > before.player.x + 30, 'Player must move');
    await page.keyboard.press('x', { delay: 160 }); await page.waitForTimeout(100);
    await page.screenshot({ path: `output/release/${mobile ? 'phone' : 'desktop'}-play.png` });
    await page.click('#pause');
    await page.waitForFunction(() => JSON.parse(window.render_game_to_text()).paused);
    const paused = await state(page); await page.waitForTimeout(350);
    assert.equal(paused.paused, true);
    assert.equal((await state(page)).player.x, paused.player.x);
    await page.screenshot({ path: `output/release/${mobile ? 'phone' : 'desktop'}-pause.png` });
    await page.click('#pause');
    await page.waitForFunction(() => !JSON.parse(window.render_game_to_text()).paused);
    assert.equal((await state(page)).paused, false);
    await page.click('#help-toggle');
    await page.waitForFunction(() => JSON.parse(window.render_game_to_text()).paused);
    assert(await page.locator('#instructions').evaluate((el) => el.open));
    assert.equal((await state(page)).paused, true);
    await page.click('#help-close');
    await page.waitForFunction(() => !JSON.parse(window.render_game_to_text()).paused).catch(async (error) => {
      console.log('Resume diagnostic', await state(page), await page.evaluate(() => ({ hidden:document.hidden, focused:document.activeElement.id, help:document.querySelector('#instructions').open, button:document.querySelector('#pause').textContent })));
      await page.screenshot({path:'output/release/resume-failure.png'});
      throw error;
    });
    assert.equal((await state(page)).paused, false);
    // A real blur event must clear all virtual input, including deferred releases.
    await page.evaluate(() => window.dispatchEvent(new Event('blur')));
    assert.deepEqual((await state(page)).pressed, []);
    assert.deepEqual(errors, []);
    console.log(type.name(), 'PASS', await state(page));
  } finally { await browser.close(); }
}
(async () => { if (!process.argv.includes('--webkit')) await run(chromium); if(!process.argv.includes('--chromium')) await run(webkit, true); })().catch((e) => { console.error(e); process.exitCode = 1; });
