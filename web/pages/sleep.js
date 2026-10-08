// Sleep screen editor: converts any picture in the browser into an 8-bit
// grayscale BMP at the panel's exact portrait size, uploads it to /sleep and
// pins it. The device then only streams rows from the SD card at sleep time and
// applies its own panel-tuned dithering, so nothing heavy runs on the reader.

const I18N = {
  en: {
    currentTitle: "Current sleep image",
    noneShort: "None",
    loading: "Loading...",
    unpin: "Stop using this image",
    newTitle: "New sleep image",
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
    galleryHelp:
      "Images in the /sleep folder. JPG and PNG copied straight to the card also work: the device converts them once. When none is pinned, it picks one at random each time it sleeps.",
    pinnedNow: "The device shows this image when it sleeps.",
    noPinned: "No image pinned. The device uses its sleep screen setting.",
    modeCustom: "Sleep screen mode: Custom image.",
    modeOther: "Pinning an image switches the sleep screen to Custom image.",
    modeSwitched: "Sleep screen switched to Custom image.",
    use: "Use",
    remove: "Delete",
    inUse: "In use",
    emptyGallery: "No images yet.",
    confirmDelete: "Delete this image from the device?",
    converting: "Converting...",
    uploading: "Sending to the device...",
    saved: "Done! Lock the device to see it.",
    failed: "Something went wrong: ",
    badImage: "This file could not be opened as an image.",
  },
  pt: {
    currentTitle: "Imagem de descanso atual",
    noneShort: "Nenhuma",
    loading: "Carregando...",
    unpin: "Parar de usar esta imagem",
    newTitle: "Nova imagem de descanso",
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
    galleryHelp:
      "Imagens da pasta /sleep. JPG e PNG copiados direto para o cartão também funcionam: o aparelho converte uma vez. Sem imagem fixada, ele sorteia uma a cada vez que entra em descanso.",
    pinnedNow: "O aparelho mostra esta imagem ao entrar em descanso.",
    noPinned: "Nenhuma imagem fixada. O aparelho usa o modo de tela de descanso configurado.",
    modeCustom: "Modo da tela de descanso: Imagem personalizada.",
    modeOther: "Ao fixar uma imagem, a tela de descanso muda para Imagem personalizada.",
    modeSwitched: "Tela de descanso alterada para Imagem personalizada.",
    use: "Usar",
    remove: "Apagar",
    inUse: "Em uso",
    emptyGallery: "Nenhuma imagem ainda.",
    confirmDelete: "Apagar esta imagem do aparelho?",
    converting: "Convertendo...",
    uploading: "Enviando para o aparelho...",
    saved: "Pronto! Bloqueie o aparelho para ver.",
    failed: "Algo deu errado: ",
    badImage: "Não foi possível abrir este arquivo como imagem.",
  },
};
const LANG = window.FZ_LANG === "pt" ? "pt" : "en";
const t = (key) => I18N[LANG][key] || I18N.en[key] || key;

const SLEEP_FOLDER = "/sleep";
const state = {
  width: 480,
  height: 800,
  pinned: "",
  source: null,
  fit: "fill",
  zoom: 1,
  offsetX: 0,
  offsetY: 0,
  brightness: 0,
  contrast: 10,
};

const $ = (id) => document.getElementById(id);
const work = document.createElement("canvas");

function applyTranslations() {
  document.documentElement.lang = LANG === "pt" ? "pt-BR" : "en";
  document.querySelectorAll("[data-i18n]").forEach((el) => {
    el.textContent = t(el.dataset.i18n);
  });
}

function setStatus(text, kind) {
  const el = $("status");
  el.textContent = text || "";
  el.className = "status" + (kind ? " " + kind : "");
}

function downloadUrl(path) {
  return "/download?path=" + encodeURIComponent(path);
}

// ---------------------------------------------------------------------------
// Device state
// ---------------------------------------------------------------------------

async function loadDeviceState() {
  try {
    const res = await fetch("/api/sleep-image");
    if (!res.ok) throw new Error(res.status + " " + res.statusText);
    const data = await res.json();
    state.width = data.width || state.width;
    state.height = data.height || state.height;
    state.pinned = data.pinned || "";
    renderCurrent(data.customMode);
  } catch (err) {
    $("currentText").textContent = t("failed") + err.message;
  }
}

function renderCurrent(customMode) {
  const frame = $("currentFrame");
  frame.textContent = "";
  if (state.pinned) {
    const img = document.createElement("img");
    img.src = downloadUrl(state.pinned);
    img.alt = state.pinned;
    frame.appendChild(img);
    $("currentText").textContent = t("pinnedNow") + " (" + state.pinned + ")";
  } else {
    const span = document.createElement("span");
    span.className = "empty muted";
    span.textContent = t("noneShort");
    frame.appendChild(span);
    $("currentText").textContent = t("noPinned");
  }
  $("modeText").textContent = customMode ? t("modeCustom") : t("modeOther");
  $("unpinBtn").hidden = !state.pinned;
}

async function ensureSleepFolder() {
  // A missing folder makes the listing drop the connection, so treat any
  // failure as "create it"; an "already exists" reply is fine too.
  try {
    const res = await fetch("/api/files?path=" + encodeURIComponent(SLEEP_FOLDER));
    if (res.ok) {
      await res.json();
      return;
    }
  } catch (err) {
    console.info("Sleep folder not listed, creating it", err);
  }
  const body = new URLSearchParams({ name: "sleep", path: "/" });
  const made = await fetch("/mkdir", { method: "POST", body });
  if (!made.ok) {
    const text = await made.text();
    if (!/exists/i.test(text)) throw new Error(text);
  }
}

async function loadGallery() {
  const gallery = $("gallery");
  let items = [];
  try {
    const res = await fetch("/api/files?path=" + encodeURIComponent(SLEEP_FOLDER));
    if (res.ok) items = await res.json();
  } catch (err) {
    console.error(err);
  }
  const images = items
    .filter((it) => !it.isDirectory && /\.(bmp|jpe?g|png)$/i.test(it.name))
    .sort((a, b) => b.name.localeCompare(a.name));
  gallery.textContent = "";
  if (!images.length) {
    const p = document.createElement("p");
    p.className = "muted";
    p.textContent = t("emptyGallery");
    gallery.appendChild(p);
    return;
  }
  for (const it of images) {
    const path = SLEEP_FOLDER + "/" + it.name;
    const pinned = path === state.pinned;
    const tile = document.createElement("div");
    tile.className = "tile" + (pinned ? " pinned" : "");
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
    if (pinned) {
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

async function pinImage(path) {
  const body = new URLSearchParams({ action: "pin", path });
  const res = await fetch("/api/sleep-image", { method: "POST", body });
  if (!res.ok) throw new Error(await res.text());
  return res.json();
}

async function unpinImage() {
  const body = new URLSearchParams({ action: "unpin" });
  const res = await fetch("/api/sleep-image", { method: "POST", body });
  if (!res.ok) throw new Error(await res.text());
}

async function deleteImage(path) {
  if (!confirm(t("confirmDelete"))) return;
  try {
    if (path === state.pinned) await unpinImage();
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
    await ensureSleepFolder();
    const name = uniqueName();
    const form = new FormData();
    form.append("file", blob, name);
    const res = await fetch("/upload?path=" + encodeURIComponent(SLEEP_FOLDER), { method: "POST", body: form });
    if (!res.ok) throw new Error(await res.text());
    const result = await pinImage(SLEEP_FOLDER + "/" + name);
    setStatus(t("saved") + (result.modeChanged ? " " + t("modeSwitched") : ""), "ok");
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
  $("unpinBtn").addEventListener("click", () =>
    unpinImage()
      .then(refreshAll)
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
refreshAll();
