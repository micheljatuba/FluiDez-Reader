const assert = require("node:assert/strict");
const { Blob, File } = require("node:buffer");
const fs = require("node:fs");
const path = require("node:path");
const { test } = require("node:test");
const vm = require("node:vm");

const sourcePath = path.join(__dirname, "../../web/pages/files.js");
const source = fs.readFileSync(sourcePath, "utf8");

function loadSection(context, start, end) {
  const first = source.indexOf(start);
  assert.notEqual(first, -1, `Missing source section: ${start}`);
  const last = source.indexOf(end, first + start.length);
  assert.notEqual(last, -1, `Missing section boundary: ${end}`);
  vm.runInContext(source.slice(first, last), context, {
    filename: sourcePath,
    lineOffset: source.slice(0, first).split("\n").length - 1,
  });
}

function element(attributes = {}, textContent = "") {
  const classes = new Set();
  return {
    textContent,
    style: {},
    children: [],
    disabled: false,
    checked: false,
    getAttribute: (name) => attributes[name] ?? null,
    getAttributeNS: (_, name) => attributes[`namespace:${name}`] ?? null,
    appendChild(child) {
      this.children.push(child);
    },
    replaceChildren(...children) {
      this.children = children;
    },
    classList: {
      add: (name) => classes.add(name),
      remove: (name) => classes.delete(name),
      contains: (name) => classes.has(name),
      toggle: (name, enabled) => (enabled ? classes.add(name) : classes.delete(name)),
    },
  };
}

test("folder paths are decoded exactly once and round-trip through API queries", async () => {
  const declaration = source.match(/^const currentPath = .+$/m);
  assert.ok(declaration);
  for (const expected of [
    "/",
    "/Books",
    "/100% Read",
    "/A%20B",
    "/A%2FB",
    "/A%252FB",
    "/A+B",
    "/Fic\u00e7\u00e3o/\u65e5\u672c\u8a9e",
  ]) {
    let requestedPath;
    const context = vm.createContext({
      window: { location: { search: `?${new URLSearchParams({ path: expected })}` } },
      URLSearchParams,
      fetch: async (url) => {
        requestedPath = new URL(url, "http://reader.local").searchParams.get("path");
        return { ok: true, json: async () => [] };
      },
    });
    vm.runInContext(declaration[0], context);
    assert.equal(vm.runInContext("currentPath", context), expected);
    loadSection(context, "async function fetchExistingUploadNames()", "/**");
    await context.fetchExistingUploadNames();
    assert.equal(requestedPath, expected);
  }

  for (const search of ["", "?path="]) {
    const context = vm.createContext({ window: { location: { search } }, URLSearchParams });
    vm.runInContext(`${declaration[0]}\nresult = currentPath;`, context);
    assert.equal(context.result, "/");
  }
});

// Stub the ZIP and parsed DOM boundaries, but execute the production metadata selection.
async function metadataFilename(creators, refinements = []) {
  const document = {
    querySelector: () => null,
    getElementsByTagNameNS: (_, name) =>
      ({ title: [element({}, "Example Book")], creator: creators, meta: refinements })[name] || [],
  };
  const context = vm.createContext({
    JSZip: { loadAsync: async () => ({ files: { "content.opf": {} } }) },
    findOPFPath: async () => "content.opf",
    safeReadText: async () => "<package/>",
    DOMParser: class {
      parseFromString() {
        return document;
      }
    },
  });
  loadSection(context, "function sanitizeMetadataFilenamePart(", "async function maybeRenameEbookFile(");
  return context.getMetadataFilenameForEpub({});
}

const role = (id, value) => element({ property: "role", refines: `#${id}` }, value);

test("EPUB 2 author role attributes remain supported", async () => {
  for (const attribute of ["role", "opf:role", "namespace:role"]) {
    assert.equal(
      await metadataFilename([
        element({ [attribute]: "trl" }, "Translator"),
        element({ [attribute]: " AUT " }, "Author"),
      ]),
      "Example Book - Author.epub",
    );
  }
});

test("EPUB 3 refinements select the author instead of an earlier translator or untyped creator", async () => {
  assert.equal(
    await metadataFilename(
      [element({ id: "translator" }, "Translator"), element({}, "Unknown"), element({ id: "author" }, "Author")],
      [role("translator", "trl"), role("author", " AUT \n")],
    ),
    "Example Book - Author.epub",
  );
});

test("all EPUB 3 roles are considered when a creator has more than one", async () => {
  assert.equal(
    await metadataFilename(
      [element({ id: "editor" }, "Editor"), element({ id: "author" }, "Author")],
      [role("editor", "edt"), role("author", "trl"), role("author", "aut")],
    ),
    "Example Book - Author.epub",
  );
});

test("creators tagged only as translator or editor are not used as the author", async () => {
  assert.equal(
    await metadataFilename(
      [element({ id: "translator" }, "Translator"), element({ role: "edt" }, "Editor")],
      [role("translator", "trl")],
    ),
    "Example Book.epub",
  );
});

test("untyped creators and file-as keep their existing fallback behavior", async () => {
  assert.equal(
    await metadataFilename([element({}, "First Author"), element({}, "Second Author")], [role("missing", "trl")]),
    "Example Book - First Author.epub",
  );
  assert.equal(
    await metadataFilename([element({ id: "author", "opf:file-as": "Fallback Author" }, " ")], [role("author", "aut")]),
    "Example Book - Fallback Author.epub",
  );
});

function uploadHarness(names, options = {}) {
  const nodes = new Map();
  const getElementById = (id) => {
    if (!nodes.has(id)) nodes.set(id, element());
    return nodes.get(id);
  };
  const files = names.map((name) => new File(["original"], name));
  getElementById("fileInput").files = files;
  getElementById("convertBeforeUpload").checked = options.convert !== false;
  const calls = { converted: [], uploaded: [], batch: [], errors: [], refreshed: 0, closed: 0, finalized: 0 };
  const timers = [];
  let complete;
  const finished = new Promise((resolve) => {
    complete = resolve;
  });
  const context = vm.createContext({
    document: { getElementById, createElement: () => element() },
    File,
    isUploadInProgress: false,
    operationCancelled: false,
    uploadGeneration: 0,
    failedUploadsGlobal: [],
    exportLogCheckbox: { checked: Boolean(options.exportLog) },
    fetchExistingUploadNames: async () => new Set(options.existing || []),
    alert: (message) => assert.fail(message),
    console: { log() {}, error: (...args) => calls.errors.push(args) },
    setTimeout: (callback, delay) => timers.push({ callback, delay }),
    window: { location: { reload: () => assert.fail("Upload results must survive completion") } },
    startBatchLog: (count) => {
      calls.batchSize = count;
    },
    showLog() {},
    clearLog() {},
    log() {},
    logError: (message) => calls.errors.push(message),
    exportLogToFile() {},
    saveToFileBatchLog: (...args) => calls.batch.push(args),
    finalizeBatchLog: () => calls.finalized++,
    closeUploadModal: () => calls.closed++,
    refreshFileList: () => calls.refreshed++,
    showFailedUploadsBanner: () => {
      calls.retryBanner = true;
    },
    restoreAfterCancel: () => {
      context.isUploadInProgress = false;
      context.operationCancelled = false;
      calls.cancelled = true;
      complete();
    },
    convertEpubFile: async (file, onProgress) => {
      calls.converted.push(file.name);
      if (options.cancel) {
        context.operationCancelled = true;
        throw new Error("Cancelled by user");
      }
      if (options.conversionErrors?.[file.name]) throw new Error(options.conversionErrors[file.name]);
      onProgress(100);
      return new Blob(["new"]);
    },
  });
  for (const [method, errorKey] of [["WebSocket", "wsErrors"], ["HTTP", "httpErrors"]]) {
    context[`uploadFile${method}`] = async (file, onProgress) => {
      calls.uploaded.push({ method, file });
      if (options[errorKey]?.[file.name]) throw new Error(options[errorKey][file.name]);
      onProgress(file.size, file.size);
    };
  }
  loadSection(context, "function reserveAvailableUploadFilename(", "async function fetchExistingUploadNames()");
  loadSection(context, "function dismissUploadResults()", "async function uploadFile()");
  const renderResults = context.showUploadResults;
  context.showUploadResults = (results) => {
    calls.results = JSON.parse(JSON.stringify(results));
    const summary = renderResults(results);
    complete();
    return summary;
  };
  loadSection(context, "async function uploadFile()", "function showFailedUploadsBanner()");
  return {
    context,
    calls,
    files,
    nodes,
    async run() {
      await context.uploadFile();
      await finished;
      timers.sort((a, b) => a.delay - b.delay).forEach(({ callback }) => callback());
    },
  };
}

test("successful optimization uploads the converted file and refreshes without reloading", { timeout: 2000 }, async () => {
  const harness = uploadHarness(["book.epub"]);
  await harness.run();
  assert.deepEqual(harness.calls.results, [{ name: "book.epub", status: "optimized", message: "" }]);
  assert.equal(await harness.calls.uploaded[0].file.text(), "new");
  assert.equal(await harness.files[0].text(), "original");
  assert.equal(harness.calls.closed, 1);
  assert.equal(harness.calls.refreshed, 1);
  assert.equal(harness.context.isUploadInProgress, false);
  assert.equal(harness.nodes.get("uploadModalClose").classList.contains("disabled"), false);
  assert.equal(harness.nodes.get("uploadResults").style.display, "block");
});

test("uploads without optimization are originals without warnings", { timeout: 2000 }, async () => {
  const harness = uploadHarness(["book.epub", "notes.txt"], { convert: false });
  await harness.run();
  assert.equal(harness.calls.converted.length, 0);
  assert.deepEqual(harness.calls.results.map((result) => result.status), ["original", "original"]);
  assert.equal(harness.nodes.get("uploadResultsSummary").textContent, "2/2 uploaded: 0 optimized, 2 original; 0 failed.");
  assert.equal(harness.nodes.get("uploadResults").classList.contains("has-warnings"), false);
});

test("optimization failure uploads the original but retains an explicit warning", { timeout: 2000 }, async () => {
  const harness = uploadHarness(["book.epub"], { conversionErrors: { "book.epub": "Invalid ZIP" } });
  await harness.run();
  assert.deepEqual(harness.calls.results, [
    { name: "book.epub", status: "original", message: "Optimization failed: Invalid ZIP" },
  ]);
  assert.equal(harness.calls.uploaded[0].file, harness.files[0]);
  assert.match(harness.nodes.get("uploadResultsSummary").textContent, /1 optimization warning/);
  assert.match(harness.nodes.get("uploadResultsList").children[0].textContent, /Original uploaded.*Invalid ZIP/);
  assert.equal(harness.nodes.get("uploadResults").classList.contains("has-warnings"), true);
  assert.equal(harness.context.failedUploadsGlobal.length, 0);
});

test("mixed batches distinguish conversion warnings from upload failures and retain retries", { timeout: 2000 }, async () => {
  const harness = uploadHarness(["good.epub", "fallback.epub", "failed.epub", "notes.txt"], {
    exportLog: true,
    conversionErrors: { "fallback.epub": "Image error" },
    wsErrors: { "failed.epub": "SD full" },
  });
  await harness.run();
  assert.deepEqual(harness.calls.results.map((result) => result.status), ["optimized", "original", "failed", "original"]);
  assert.equal(
    harness.nodes.get("uploadResultsSummary").textContent,
    "3/4 uploaded: 1 optimized, 2 original; 1 failed; 1 optimization warning.",
  );
  assert.equal(harness.context.failedUploadsGlobal.length, 1);
  assert.equal(harness.context.failedUploadsGlobal[0].file, harness.files[2]);
  assert.equal(harness.calls.retryBanner, true);
  assert.equal(harness.calls.finalized, 1);
  assert.deepEqual(harness.calls.batch, [
    ["good.epub", true, 8, 3],
    ["fallback.epub", false, 0, 0],
    ["failed.epub", false, 8, 3],
  ]);
});

test("HTTP fallback retains optimization warnings and does not duplicate results", { timeout: 2000 }, async () => {
  const harness = uploadHarness(["fallback.epub", "notes.txt"], {
    conversionErrors: { "fallback.epub": "Invalid EPUB" },
    wsErrors: { "fallback.epub": "WebSocket connection failed" },
  });
  await harness.run();
  assert.deepEqual(harness.calls.uploaded.map(({ method }) => method), ["WebSocket", "HTTP", "HTTP"]);
  assert.equal(harness.calls.results.length, 2);
  assert.equal(harness.calls.results[0].message, "Optimization failed: Invalid EPUB");
  assert.equal(harness.calls.results[1].message, "");
});

test("network interruption reports unattempted files and preserves earlier warnings", { timeout: 2000 }, async () => {
  const harness = uploadHarness(["fallback.epub", "failed.epub", "pending.epub", "notes.txt"], {
    exportLog: true,
    conversionErrors: { "fallback.epub": "Invalid EPUB" },
    wsErrors: { "failed.epub": "Upload timeout" },
  });
  await harness.run();
  assert.deepEqual(harness.calls.results.map((result) => result.status), ["original", "failed", "failed", "failed"]);
  assert.equal(harness.calls.converted.length, 2);
  assert.equal(harness.calls.uploaded.length, 2);
  assert.equal(harness.context.failedUploadsGlobal.length, 3);
  assert.equal(harness.context.failedUploadsGlobal[1].file, harness.files[2]);
  assert.equal(harness.calls.finalized, 1);
  assert.deepEqual(harness.calls.batch[2], ["pending.epub", false]);
  assert.equal(
    harness.nodes.get("uploadResultsSummary").textContent,
    "1/4 uploaded: 0 optimized, 1 original; 3 failed; 1 optimization warning.",
  );
});

test("failed HTTP fallback keeps the original file available for retry", { timeout: 2000 }, async () => {
  const harness = uploadHarness(["book.epub", "pending.epub"], {
    wsErrors: { "book.epub": "WebSocket connection failed" },
    httpErrors: { "book.epub": "Network error" },
  });
  await harness.run();
  assert.deepEqual(harness.calls.results.map((result) => result.status), ["failed", "failed"]);
  assert.equal(harness.context.failedUploadsGlobal[0].file, harness.files[0]);
  assert.equal(harness.context.failedUploadsGlobal[1].file, harness.files[1]);
});

test("results use the actual collision-free uploaded filename", { timeout: 2000 }, async () => {
  const harness = uploadHarness(["book.epub"], { existing: ["book.epub"] });
  await harness.run();
  assert.equal(harness.calls.results[0].name, "book (2).epub");
  assert.equal(harness.calls.uploaded[0].file.name, "book (2).epub");
});

test("cancellation does not report a successful upload or upload the original", { timeout: 2000 }, async () => {
  const harness = uploadHarness(["book.epub"], { cancel: true });
  await harness.run();
  assert.equal(harness.calls.cancelled, true);
  assert.equal(harness.calls.uploaded.length, 0);
  assert.equal(harness.calls.results, undefined);
  assert.equal(harness.context.failedUploadsGlobal.length, 0);
});

test("result rendering treats filenames and errors as text, replaces old rows and can be dismissed", () => {
  const harness = uploadHarness([]);
  harness.context.showUploadResults([
    { name: "<img src=x>.epub", status: "original", message: "Optimization failed: <script>bad</script>" },
  ]);
  assert.equal(
    harness.nodes.get("uploadResultsList").children[0].textContent,
    "<img src=x>.epub: Original uploaded - Optimization failed: <script>bad</script>",
  );
  harness.context.showUploadResults([{ name: "good.epub", status: "optimized", message: "" }]);
  assert.equal(harness.nodes.get("uploadResultsList").children.length, 1);
  assert.equal(harness.nodes.get("uploadResults").classList.contains("has-warnings"), false);
  harness.context.dismissUploadResults();
  assert.equal(harness.nodes.get("uploadResults").style.display, "none");
});
