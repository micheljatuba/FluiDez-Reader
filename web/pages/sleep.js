// Screens editor: converts any picture in the browser into an 8-bit grayscale
// BMP at the panel's exact portrait size, uploads it to /sleep (sleep screen)
// or /bootscreen (boot screen) and pins it. The device then only streams rows
// from the SD card and applies its own panel-tuned dithering, so nothing heavy
// runs on the reader.

const I18N = {
  en: {
    tabSleep: "Sleep screen",
    tabBoot: "Boot screen",
    noneShort: "None",
    loading: "Loading...",
    rotate: "Rotate through all images",
    newHelp:
      "Pick any photo or drawing. It is converted here in your browser to the exact size of the screen, so the device shows it instantly.",
    choose: "Choose image",
    dragHint: "Drag to reposition",
    fit: "Fit",
    fitFill: "Fill screen",
    fitWhite: "Whole image, white edges",
    fitBlack: "Whole image, black edges",
    zoom: "Zoom",
    brightness: "Brightness",
    contrast: "Contrast",
    einkPreview: "Show e-ink preview (4 grays)",
    save: "Save and use on device",
    reset: "Reset",
    galleryTitle: "Images on the device",
    use: "Use",
    remove: "Delete",
    inUse: "In use",
    emptyGallery: "No images yet.",
    confirmDelete: "Delete this image from the device?",
    converting: "Converting...",
    uploading: "Sending to the device...",
    failed: "Something went wrong: ",
    badImage: "This file could not be opened as an image.",
    "sleep.currentTitle": "Current sleep image",
    "sleep.newTitle": "New sleep image",
    "sleep.pinned": "The device shows this image when it sleeps.",
    "sleep.rotating": "Rotating: each time it sleeps, the device shows a different image from {folder}.",
    "sleep.off": "The sleep screen uses another mode. Pin an image or turn on rotation to show your own.",
    "sleep.galleryHelp":
      "Images in the /sleep folder. JPG and PNG copied straight to the card also work: the device converts them once. When none is pinned, it picks one at random each time it sleeps.",
    "sleep.saved": "Done! Lock the device to see it.",
    "sleep.modeSwitched": "Sleep screen switched to Custom image.",
    "boot.currentTitle": "Current boot image",
    "boot.newTitle": "New boot image",
    "boot.pinned": "The device shows this image when it turns on.",
    "boot.rotating": "Rotating: each time it turns on, the device shows a different image from {folder}.",
    "boot.off": "The custom boot screen is off. Pin an image or turn on rotation to show your own.",
    "boot.galleryHelp":
      "BMP images in the /bootscreen folder. When none is pinned, the device picks one at random each time it turns on.",
    "boot.saved": "Done! Turn the device off and on to see it.",
    "boot.modeSwitched": "Custom boot screen turned on.",
    "boot.hiddenFolder":
      "The device rotates through {folder}, so images saved here only show when they are pinned.",
  },
  pt: {
    tabSleep: "Tela de descanso",
    tabBoot: "Tela de inicialização",
    noneShort: "Nenhuma",
    loading: "Carregando...",
    rotate: "Sortear entre todas as imagens",
    newHelp:
      "Escolha qualquer foto ou desenho. A conversão acontece aqui no navegador, no tamanho exato da tela, para o aparelho mostrar na hora.",
    choose: "Escolher imagem",
    dragHint: "Arraste para reposicionar",
    fit: "Encaixe",
    fitFill: "Preencher a tela",
    fitWhite: "Imagem inteira, bordas brancas",
    fitBlack: "Imagem inteira, bordas pretas",
    zoom: "Zoom",
    brightness: "Brilho",
    contrast: "Contraste",
    einkPreview: "Prévia do e-ink (4 tons de cinza)",
    save: "Salvar e usar no aparelho",
    reset: "Restaurar",
    galleryTitle: "Imagens no aparelho",
    use: "Usar",
    remove: "Apagar",
    inUse: "Em uso",
    emptyGallery: "Nenhuma imagem ainda.",
    confirmDelete: "Apagar esta imagem do aparelho?",
    converting: "Convertendo...",
    uploading: "Enviando para o aparelho...",
    failed: "Algo deu errado: ",
    badImage: "Não foi possível abrir este arquivo como imagem.",
    "sleep.currentTitle": "Imagem de descanso atual",
    "sleep.newTitle": "Nova imagem de descanso",
    "sleep.pinned": "O aparelho mostra esta imagem ao entrar em descanso.",
    "sleep.rotating": "Sorteio ligado: a cada descanso, o aparelho mostra uma imagem diferente da pasta {folder}.",
    "sleep.off":
      "A tela de descanso está em outro modo. Fixe uma imagem ou ligue o sorteio para usar as suas.",
    "sleep.galleryHelp":
      "Imagens da pasta /sleep. JPG e PNG copiados direto para o cartão também funcionam: o aparelho converte uma vez. Sem imagem fixada, ele sorteia uma a cada vez que entra em descanso.",
    "sleep.saved": "Pronto! Bloqueie o aparelho para ver.",
    "sleep.modeSwitched": "Tela de descanso alterada para Imagem personalizada.",
    "boot.currentTitle": "Imagem de inicialização atual",
    "boot.newTitle": "Nova imagem de inicialização",
    "boot.pinned": "O aparelho mostra esta imagem ao ligar.",
    "boot.rotating": "Sorteio ligado: a cada vez que liga, o aparelho mostra uma imagem diferente da pasta {folder}.",
    "boot.off":
      "A tela de inicialização personalizada está desligada. Fixe uma imagem ou ligue o sorteio para usar as suas.",
    "boot.galleryHelp":
      "Imagens BMP da pasta /bootscreen. Sem imagem fixada, o aparelho sorteia uma a cada vez que liga.",
    "boot.saved": "Pronto! Desligue e ligue o aparelho para ver.",
    "boot.modeSwitched": "Tela de inicialização personalizada ligada.",
    "boot.hiddenFolder":
      "O aparelho sorteia entre as imagens de {folder}, então as salvas aqui só aparecem quando fixadas.",
  },
};
const LANG = window.FZ_LANG === "pt" ? "pt" : "en";
const t = (key, vars) => {
  let text = I18N[LANG][key] || I18N.en[key] || key;
  for (const [name, value] of Object.entries(vars || {})) text = text.replace("{" + name + "}", value);
  return text;
};

// Each tab edits one screen: where its images live and which formats it draws.
const TARGETS = {
  sleep: { folder: "/sleep", folderName: "sleep", pattern: /\.(bmp|jpe?g|png)$/i },
  boot: { folder: "/bootscreen", folderName: "bootscreen", pattern: /\.bmp$/i },
};
const state = {
  target: location.hash === "#boot" ? "boot" : "sleep",
  device: null,
  width: 480,
  height: 800,
  source: null,
  fit: "fill",
  zoom: 1,
  offsetX: 0,
  offsetY: 0,
  brightness: 0,
  contrast: 10,
};
const target = () => TARGETS[state.target];
const tt = (key, vars) => t(state.target + "." + key, vars);

const $ = (id) => document.getElementById(id);
const work = document.createElement("canvas");

function applyTranslations() {
  document.documentElement.lang = LANG === "pt" ? "pt-BR" : "en";
  document.querySelectorAll("[data-i18n]").forEach((el) => {
    el.textContent = t(el.dataset.i18n);
  });
  $("currentTitle").textContent = tt("currentTitle");
  $("newTitle").textContent = tt("newTitle");
  $("galleryHelp").textContent = tt("galleryHelp");
}

function setStatus(text, kind) {
  const el = $("status");
  el.textContent = text || "";
  el.className = "status" + (kind ? " " + kind : "");
}

function downloadUrl(path) {
  return "/download?path=" + encodeURIComponent(path);
}

function selectTab(name) {
  state.target = name;
  history.replaceState(null, "", name === "boot" ? "#boot" : "#sleep");
  document.querySelectorAll("#screenTabs button").forEach((b) => b.classList.toggle("active", b.dataset.target === name));
  applyTranslations();
  setStatus("");
  renderCurrent();
  loadGallery();
}

// ---------------------------------------------------------------------------
// Device state
// ---------------------------------------------------------------------------

async function loadDeviceState() {
  try {
    const res = await fetch("/api/sleep-image");
    if (!res.ok) throw new Error(res.status + " " + res.statusText);
    state.device = await res.json();
    state.width = state.device.width || state.width;
    state.height = state.device.height || state.height;
    renderCurrent();
  } catch (err) {
    $("currentText").textContent = t("failed") + err.message;
  }
}

function pinnedPath() {
  if (!state.device) return "";
  return (state.target === "boot" ? state.device.bootPinned : state.device.pinned) || "";
}

function screenEnabled() {
  if (!state.device) return false;
  return state.target === "boot" ? state.device.bootEnabled : state.device.customMode;
}

function renderCurrent() {
  if (!state.device) return;
  const pinned = pinnedPath();
  const frame = $("currentFrame");
  frame.textContent = "";
  if (pinned) {
    const img = document.createElement("img");
    img.src = downloadUrl(pinned);
    img.alt = pinned;
    frame.appendChild(img);
  } else {
    const span = document.createElement("span");
    span.className = "empty muted";
    span.textContent = t("noneShort");
    frame.appendChild(span);
  }

  const rotating = screenEnabled() && !pinned;
  if (!screenEnabled()) $("currentText").textContent = tt("off");
  else if (pinned) $("currentText").textContent = tt("pinned") + " (" + pinned + ")";
  else $("currentText").textContent = tt("rotating", { folder: target().folder });

  const rotationFolder = state.target === "boot" ? state.device.bootRotationFolder : "";
  $("modeText").textContent =
    rotationFolder && rotationFolder.toLowerCase() !== target().folder ? tt("hiddenFolder", { folder: rotationFolder }) : "";
  $("rotateBtn").hidden = rotating;
}

async function ensureFolder() {
  // A missing folder makes the listing drop the connection, so treat any
  // failure as "create it"; an "already exists" reply is fine too.
  try {
    const res = await fetch("/api/files?path=" + encodeURIComponent(target().folder));
    if (res.ok) {
      await res.json();
      return;
    }
  } catch (err) {
    console.info("Screen folder not listed, creating it", err);
  }
  const body = new URLSearchParams({ name: target().folderName, path: "/" });
  const made = await fetch("/mkdir", { method: "POST", body });
  if (!made.ok) {
    const text = await made.text();
    if (!/exists/i.test(text)) throw new Error(text);
  }
}

async function loadGallery() {
  const gallery = $("gallery");
  const listedTarget = state.target;
  let items = [];
  try {
    const res = await fetch("/api/files?path=" + encodeURIComponent(target().folder));
    if (res.ok) items = await res.json();
  } catch (err) {
    console.error(err);
  }
  if (listedTarget !== state.target) return;  // the user switched tabs meanwhile
  const images = items
    .filter((it) => !it.isDirectory && target().pattern.test(it.name))
    .sort((a, b) => b.name.localeCompare(a.name));
  gallery.textContent = "";
  if (!images.length) {
    const p = document.createElement("p");
    p.className = "muted";
    p.textContent = t("emptyGallery");
    gallery.appendChild(p);
    return;
  }
  const pinned = pinnedPath();
  for (const it of images) {
    const path = target().folder + "/" + it.name;
    const isPinned = path === pinned;
    const tile = document.createElement("div");
    tile.className = "tile" + (isPinned ? " pinned" : "");
    const thumb = document.createElement("div");
    thumb.className = "thumb";
    const img = document.createElement("img");
    img.loading = "lazy";
    img.src = downloadUrl(path);
    img.alt = it.name;
    thumb.appendChild(img);
    const name = document.createElement("div");
    name.className = "name";
    name.textContent = it.name;
    const row = document.createElement("div");
    row.className = "row";
    if (isPinned) {
      const badge = document.createElement("span");
      badge.className = "badge";
      badge.textContent = t("inUse");
      row.appendChild(badge);
    } else {
      const useBtn = document.createElement("button");
      useBtn.className = "btn btn-secondary";
      useBtn.textContent = t("use");
      useBtn.onclick = () => pinImage(path).then(refreshAll).catch((e) => alert(t("failed") + e.message));
      row.appendChild(useBtn);
    }
    const delBtn = document.createElement("button");
    delBtn.className = "btn btn-danger";
    delBtn.textContent = t("remove");
    delBtn.onclick = () => deleteImage(path);
    row.appendChild(delBtn);
    tile.append(thumb, name, row);
    gallery.appendChild(tile);
  }
}

async function postScreen(params) {
  const body = new URLSearchParams({ target: state.target, ...params });
  const res = await fetch("/api/sleep-image", { method: "POST", body });
  if (!res.ok) throw new Error(await res.text());
  return res.json();
}

const pinImage = (path) => postScreen({ action: "pin", path });
const unpinImage = () => postScreen({ action: "unpin" });
const rotateImages = () => postScreen({ action: "rotate" });

async function deleteImage(path) {
  if (!confirm(t("confirmDelete"))) return;
  try {
    if (path === pinnedPath()) await unpinImage();
    const res = await fetch("/delete", { method: "POST", body: new URLSearchParams({ path }) });
    if (!res.ok) throw new Error(await res.text());
  } catch (err) {
    alert(t("failed") + err.message);
  }
  refreshAll();
}

async function refreshAll() {
  await loadDeviceState();
  await loadGallery();
}

// ---------------------------------------------------------------------------
// Editor
// ---------------------------------------------------------------------------

async function decodeImage(file) {
  if (window.createImageBitmap) {
    try {
      return await createImageBitmap(file, { imageOrientation: "from-image" });
    } catch (err) {
      console.warn("createImageBitmap failed, falling back to <img>", err);
    }
  }
  return new Promise((resolve, reject) => {
    const url = URL.createObjectURL(file);
    const img = new Image();
    img.onload = () => {
      URL.revokeObjectURL(url);
      resolve(img);
    };
    img.onerror = () => {
      URL.revokeObjectURL(url);
      reject(new Error(t("badImage")));
    };
    img.src = url;
  });
}

function sourceSize() {
  const s = state.source;
  return { w: s.width || s.naturalWidth, h: s.height || s.naturalHeight };
}

function drawScale() {
  const { w, h } = sourceSize();
  const cover = Math.max(state.width / w, state.height / h);
  const contain = Math.min(state.width / w, state.height / h);
  return (state.fit === "fill" ? cover : contain) * state.zoom;
}

// Keeps a "fill" image covering the whole screen while panning.
function clampOffsets() {
  if (state.fit !== "fill") return;
  const { w, h } = sourceSize();
  const scale = drawScale();
  const maxX = Math.max(0, (w * scale - state.width) / 2);
  const maxY = Math.max(0, (h * scale - state.height) / 2);
  state.offsetX = Math.min(maxX, Math.max(-maxX, state.offsetX));
  state.offsetY = Math.min(maxY, Math.max(-maxY, state.offsetY));
}

// Returns the adjusted 8-bit luminance of every output pixel.
function composeGray() {
  const W = state.width;
  const H = state.height;
  work.width = W;
  work.height = H;
  const ctx = work.getContext("2d", { willReadFrequently: true });
  ctx.fillStyle = state.fit === "black" ? "#000" : "#fff";
  ctx.fillRect(0, 0, W, H);
  const { w, h } = sourceSize();
  const scale = drawScale();
  const dw = w * scale;
  const dh = h * scale;
  ctx.imageSmoothingEnabled = true;
  ctx.imageSmoothingQuality = "high";
  ctx.drawImage(state.source, (W - dw) / 2 + state.offsetX, (H - dh) / 2 + state.offsetY, dw, dh);

  const rgba = ctx.getImageData(0, 0, W, H).data;
  const gray = new Uint8ClampedArray(W * H);
  const contrast = 1 + state.contrast / 100;
  const brightness = state.brightness;
  for (let i = 0, p = 0; i < gray.length; i++, p += 4) {
    const lum = (77 * rgba[p] + 150 * rgba[p + 1] + 29 * rgba[p + 2]) >> 8;
    gray[i] = (lum - 128) * contrast + 128 + brightness;
  }
  return gray;
}

// Approximates the reader's Atkinson pass with its X4-tuned thresholds so the
// preview shows the four real panel tones instead of smooth grays.
function einkDither(gray, W, H) {
  const levels = [15, 30, 80, 210];
  const out = new Uint8ClampedArray(gray.length);
  const err = new Float32Array(gray.length);
  for (let y = 0; y < H; y++) {
    for (let x = 0; x < W; x++) {
      const i = y * W + x;
      const v = Math.min(255, Math.max(0, gray[i] + err[i]));
      const q = v < 30 ? 0 : v < 50 ? 1 : v < 140 ? 2 : 3;
      out[i] = levels[q];
      const e = (v - levels[q]) / 8;
      if (x + 1 < W) err[i + 1] += e;
      if (x + 2 < W) err[i + 2] += e;
      if (y + 1 < H) {
        if (x > 0) err[i + W - 1] += e;
        err[i + W] += e;
        if (x + 1 < W) err[i + W + 1] += e;
      }
      if (y + 2 < H) err[i + 2 * W] += e;
    }
  }
  return out;
}

let renderQueued = false;
function renderPreview() {
  if (renderQueued || !state.source) return;
  renderQueued = true;
  requestAnimationFrame(() => {
    renderQueued = false;
    clampOffsets();
    const W = state.width;
    const H = state.height;
    const gray = composeGray();
    const shown = $("einkPreview").checked ? einkDither(gray, W, H) : gray;
    const canvas = $("preview");
    canvas.width = W;
    canvas.height = H;
    const ctx = canvas.getContext("2d");
    const img = ctx.createImageData(W, H);
    for (let i = 0, p = 0; i < shown.length; i++, p += 4) {
      img.data[p] = img.data[p + 1] = img.data[p + 2] = shown[i];
      img.data[p + 3] = 255;
    }
    ctx.putImageData(img, 0, 0);
  });
}

// 8-bit paletted grayscale BMP, bottom-up. The firmware reads it row by row
// and dithers to the panel, so no extra RAM is needed at sleep time.
function encodeGrayBmp(gray, W, H) {
  const rowSize = (W + 3) & ~3;
  const paletteSize = 256 * 4;
  const offset = 14 + 40 + paletteSize;
  const fileSize = offset + rowSize * H;
  const buf = new ArrayBuffer(fileSize);
  const view = new DataView(buf);
  const bytes = new Uint8Array(buf);
  view.setUint8(0, 0x42);
  view.setUint8(1, 0x4d);
  view.setUint32(2, fileSize, true);
  view.setUint32(10, offset, true);
  view.setUint32(14, 40, true);
  view.setInt32(18, W, true);
  view.setInt32(22, H, true);
  view.setUint16(26, 1, true);
  view.setUint16(28, 8, true);
  view.setUint32(30, 0, true);
  view.setUint32(34, rowSize * H, true);
  view.setInt32(38, 2835, true);
  view.setInt32(42, 2835, true);
  view.setUint32(46, 256, true);
  view.setUint32(50, 0, true);
  for (let i = 0; i < 256; i++) {
    const p = 54 + i * 4;
    bytes[p] = bytes[p + 1] = bytes[p + 2] = i;
  }
  for (let y = 0; y < H; y++) {
    const dst = offset + (H - 1 - y) * rowSize;
    bytes.set(gray.subarray(y * W, (y + 1) * W), dst);
  }
  return new Blob([buf], { type: "image/bmp" });
}

function uniqueName() {
  const d = new Date();
  const pad = (n) => String(n).padStart(2, "0");
  return (
    "fluidez-" +
    d.getFullYear() +
    pad(d.getMonth() + 1) +
    pad(d.getDate()) +
    "-" +
    pad(d.getHours()) +
    pad(d.getMinutes()) +
    pad(d.getSeconds()) +
    ".bmp"
  );
}

async function saveToDevice() {
  if (!state.source) return;
  const saveBtn = $("saveBtn");
  saveBtn.disabled = true;
  try {
    setStatus(t("converting"));
    clampOffsets();
    const blob = encodeGrayBmp(composeGray(), state.width, state.height);
    setStatus(t("uploading"));
    await ensureFolder();
    const name = uniqueName();
    const form = new FormData();
    form.append("file", blob, name);
    const res = await fetch("/upload?path=" + encodeURIComponent(target().folder), { method: "POST", body: form });
    if (!res.ok) throw new Error(await res.text());
    const result = await pinImage(target().folder + "/" + name);
    setStatus(tt("saved") + (result.modeChanged ? " " + tt("modeSwitched") : ""), "ok");
    await refreshAll();
  } catch (err) {
    setStatus(t("failed") + err.message, "err");
  } finally {
    saveBtn.disabled = false;
  }
}

function resetAdjustments() {
  state.fit = "fill";
  state.zoom = 1;
  state.offsetX = 0;
  state.offsetY = 0;
  state.brightness = 0;
  state.contrast = 10;
  $("zoom").value = 100;
  $("brightness").value = 0;
  $("contrast").value = 10;
  document.querySelectorAll("#fitGroup button").forEach((b) => b.classList.toggle("active", b.dataset.fit === "fill"));
  renderPreview();
}

function setupEditor() {
  $("imageInput").addEventListener("change", async (e) => {
    const file = e.target.files[0];
    if (!file) return;
    $("fileName").textContent = file.name;
    try {
      state.source = await decodeImage(file);
    } catch (err) {
      setStatus(err.message, "err");
      return;
    }
    $("editor").hidden = false;
    setStatus("");
    resetAdjustments();
  });

  document.querySelectorAll("#fitGroup button").forEach((btn) => {
    btn.addEventListener("click", () => {
      state.fit = btn.dataset.fit;
      document.querySelectorAll("#fitGroup button").forEach((b) => b.classList.toggle("active", b === btn));
      renderPreview();
    });
  });
  $("zoom").addEventListener("input", (e) => {
    state.zoom = Number(e.target.value) / 100;
    renderPreview();
  });
  $("brightness").addEventListener("input", (e) => {
    state.brightness = Number(e.target.value);
    renderPreview();
  });
  $("contrast").addEventListener("input", (e) => {
    state.contrast = Number(e.target.value);
    renderPreview();
  });
  $("einkPreview").addEventListener("change", renderPreview);
  $("saveBtn").addEventListener("click", saveToDevice);
  $("resetBtn").addEventListener("click", resetAdjustments);
  $("rotateBtn").addEventListener("click", () =>
    rotateImages()
      .then((result) => {
        if (result.modeChanged) setStatus(tt("modeSwitched"), "ok");
        return refreshAll();
      })
      .catch((err) => alert(t("failed") + err.message)),
  );

  const canvas = $("preview");
  let drag = null;
  canvas.addEventListener("pointerdown", (e) => {
    if (!state.source) return;
    drag = { x: e.clientX, y: e.clientY, ox: state.offsetX, oy: state.offsetY };
    canvas.setPointerCapture(e.pointerId);
    canvas.classList.add("dragging");
  });
  canvas.addEventListener("pointermove", (e) => {
    if (!drag) return;
    const factor = state.width / canvas.clientWidth;
    state.offsetX = drag.ox + (e.clientX - drag.x) * factor;
    state.offsetY = drag.oy + (e.clientY - drag.y) * factor;
    renderPreview();
  });
  const endDrag = () => {
    drag = null;
    canvas.classList.remove("dragging");
  };
  canvas.addEventListener("pointerup", endDrag);
  canvas.addEventListener("pointercancel", endDrag);
}

applyTranslations();
setupEditor();
document.querySelectorAll("#screenTabs button").forEach((b) => {
  b.classList.toggle("active", b.dataset.target === state.target);
  b.addEventListener("click", () => selectTab(b.dataset.target));
});
refreshAll();
