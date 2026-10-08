// FluiDez portal translations. The pages are written in English; in Portuguese
// this script swaps each known English text for its translation as it appears,
// including text the page scripts create later, so they need no i18n code.
// The language follows the browser unless the footer switch saved a choice.
(function () {
  "use strict";

  let saved = null;
  try {
    saved = localStorage.getItem("fz.lang");
  } catch (e) {
    saved = null;
  }
  const lang = saved || ((navigator.language || "en").toLowerCase().startsWith("pt") ? "pt" : "en");
  window.FZ_LANG = lang;

  const PT = {
    // Navigation and page titles
    Home: "Início",
    "File Manager": "Arquivos",
    Files: "Arquivos",
    Settings: "Configurações",
    Fonts: "Fontes",
    "Sleep Screen": "Tela de descanso",
    Screens: "Telas",
    "FluiDez Reader • Open Source": "FluiDez Reader • Código aberto",
    // Home
    "Device Status": "Estado do aparelho",
    "Serial #": "Nº de série",
    Version: "Versão",
    "WiFi Status": "Wi-Fi",
    Connected: "Conectado",
    "IP Address": "Endereço IP",
    "Free Memory": "Memória livre",
    "Not found": "Não encontrado",
    "N/A": "N/D",
    // File manager
    Upload: "Enviar",
    "New Folder": "Nova pasta",
    "Delete Selected": "Apagar selecionados",
    "Upload results": "Resultado do envio",
    "Dismiss upload results": "Fechar o resultado do envio",
    "Some files failed to upload": "Alguns arquivos não foram enviados",
    Dismiss: "Fechar",
    "Retry All Failed Uploads": "Tentar de novo os que falharam",
    Retry: "Tentar de novo",
    Contents: "Conteúdo",
    Name: "Nome",
    Type: "Tipo",
    Size: "Tamanho",
    Actions: "Ações",
    FOLDER: "PASTA",
    "This folder is empty": "Esta pasta está vazia",
    "An error occurred while loading the files": "Ocorreu um erro ao carregar os arquivos",
    "Delete folder": "Apagar pasta",
    "Move file": "Mover arquivo",
    "Rename file": "Renomear arquivo",
    "Delete file": "Apagar arquivo",
    "Upload file": "Enviar arquivo",
    "Select a file to upload to": "Escolha os arquivos para enviar para",
    "Drop files here — or click to browse": "Solte os arquivos aqui ou clique para escolher",
    "Optimize EPUB": "Otimizar EPUB",
    "Advanced Mode": "Modo avançado",
    "Rename from Book Metadata": "Renomear pelos dados do livro",
    "Use Title - Author.epub when available": "Usa Título - Autor.epub quando disponível",
    "True-Grayscale": "Tons de cinza reais",
    "Fix SVG": "Corrigir SVG",
    "Split Long Sections": "Dividir seções longas",
    "Break oversized chapters into smaller reader-friendly sections":
      "Divide capítulos muito grandes em seções menores, mais leves para o leitor",
    "Keep Cover in Color": "Manter a capa colorida",
    "Resize the cover safely without grayscale conversion":
      "Redimensiona a capa sem convertê-la para tons de cinza",
    "JPEG Quality": "Qualidade do JPEG",
    "Minimum quality": "Qualidade mínima",
    "Low quality": "Qualidade baixa",
    "Medium-low quality": "Qualidade média-baixa",
    "Medium quality": "Qualidade média",
    "High quality": "Qualidade alta",
    "Maximum quality": "Qualidade máxima",
    "Characters per Page": "Caracteres por página",
    "Character count used for stable page-number metadata":
      "Quantidade de caracteres usada para numerar as páginas de forma estável",
    "Target Device": "Aparelho de destino",
    Auto: "Automático",
    "Rotation Direction": "Sentido da rotação",
    CCW: "Anti-horário",
    CW: "Horário",
    "Min Overlap": "Sobreposição mínima",
    "Auto-download Log": "Baixar o registro automaticamente",
    "Export detailed log with statistics": "Exporta um registro detalhado, com estatísticas",
    "Remember Settings": "Lembrar estas opções",
    "Store these upload options in this browser": "Guarda estas opções de envio neste navegador",
    "Image Processing Options": "Opções de processamento das imagens",
    Cover: "Capa",
    Separator: "Separador",
    Normal: "Normal",
    "H-Split": "Dividir na horizontal",
    "V-Split": "Dividir na vertical",
    Rotate: "Girar",
    "Converting will modify files and can break hash‑based sync.":
      "A conversão altera os arquivos e pode quebrar a sincronização baseada em hash.",
    "Please back up or disable sync before proceeding.":
      "Faça um backup ou desative a sincronização antes de continuar.",
    "Optimize & Upload": "Otimizar e enviar",
    Cancel: "Cancelar",
    "Conversion Log": "Registro da conversão",
    "Create a new folder in": "Criar uma pasta nova em",
    "Folder name... (use . for hidden)": "Nome da pasta... (comece com . para ocultá-la)",
    "Create Folder": "Criar pasta",
    "Delete Item(s)": "Apagar itens",
    "This action cannot be undone. Folders and everything inside them will be deleted.":
      "Esta ação não pode ser desfeita. As pastas e tudo o que estiver dentro delas serão apagados.",
    "Are you sure you want to delete the following item(s)?": "Tem certeza de que quer apagar estes itens?",
    Delete: "Apagar",
    "Rename File": "Renomear arquivo",
    Renaming: "Renomeando",
    "New file name...": "Novo nome do arquivo...",
    Rename: "Renomear",
    "Move File": "Mover arquivo",
    Moving: "Movendo",
    "Destination/Folder": "Pasta/de/destino",
    Move: "Mover",
    Preview: "Pré-visualização",
    "Previous image": "Imagem anterior",
    "Next image": "Próxima imagem",
    Download: "Baixar",
    "Network connection restored": "Conexão de rede restabelecida",
    "Network connection lost": "Conexão de rede perdida",
    "Upload cancelled by User!": "Envio cancelado.",
    "Optimized and uploaded": "Otimizado e enviado",
    "Original uploaded": "Original enviado",
    "Upload failed": "Falha no envio",
    "JSZip library not available. Conversion will proceed without image picker.":
      "A biblioteca JSZip não está disponível. A conversão vai continuar sem a escolha de imagens.",
    "Please select at least one item to delete.": "Selecione pelo menos um item para apagar.",
    "Please select at least one file!": "Selecione pelo menos um arquivo.",
    "Failed to delete - network error": "Falha ao apagar: erro de rede",
    "Please enter a folder name!": "Digite o nome da pasta.",
    "Failed to create folder - network error": "Falha ao criar a pasta: erro de rede",
    "Please enter a new name.": "Digite o novo nome.",
    "Failed to rename - network error": "Falha ao renomear: erro de rede",
    "Please enter a destination folder.": "Digite a pasta de destino.",
    "Failed to move - network error": "Falha ao mover: erro de rede",
    "Deleting...": "Apagando...",
    // Settings
    "Save Settings": "Salvar configurações",
    "Saving...": "Salvando...",
    "No changes to save.": "Nada para salvar.",
    "Settings saved successfully!": "Configurações salvas!",
    "Save failed": "Falha ao salvar",
    "Failed to load settings": "Falha ao carregar as configurações",
    "Save Status Bars": "Salvar barras de status",
    "Failed to load status bars": "Falha ao carregar as barras de status",
    "Status bars saved.": "Barras de status salvas.",
    "Book title": "Título do livro",
    "Chapter title": "Título do capítulo",
    "Last connected network": "Última rede conectada",
    Password: "Senha",
    Save: "Salvar",
    "Wi-Fi Networks": "Redes Wi-Fi",
    "No Wi-Fi networks saved": "Nenhuma rede Wi-Fi salva",
    "Add Network": "Adicionar rede",
    "SSID is required.": "Informe o SSID.",
    "Wi-Fi network saved!": "Rede Wi-Fi salva!",
    "Delete this Wi-Fi network?": "Apagar esta rede Wi-Fi?",
    "Wi-Fi network deleted": "Rede Wi-Fi apagada",
    "Failed to load": "Falha ao carregar",
    "Server Name": "Nome do servidor",
    Username: "Usuário",
    Filename: "Nome do arquivo",
    "Author - Title": "Autor - Título",
    "Title - Author": "Título - Autor",
    "OPDS Servers": "Servidores OPDS",
    "No OPDS servers configured": "Nenhum servidor OPDS configurado",
    "Add Server": "Adicionar servidor",
    "OPDS server saved!": "Servidor OPDS salvo!",
    "Delete this OPDS server?": "Apagar este servidor OPDS?",
    "OPDS server deleted": "Servidor OPDS apagado",
    "unchanged)": "sem alteração)",
    // Fonts
    "Installed Fonts": "Fontes instaladas",
    "Loading...": "Carregando...",
    "Upload Font": "Enviar fonte",
    "No fonts installed": "Nenhuma fonte instalada",
    "Failed to load font list": "Falha ao carregar a lista de fontes",
    "No .cpfont or .ttf files found in the selected folder.":
      "Nenhum arquivo .cpfont ou .ttf na pasta escolhida.",
    "No .cpfont or .ttf files selected.": "Nenhum arquivo .cpfont ou .ttf escolhido.",
    "Please select files from a single font family.": "Escolha arquivos de uma única família de fontes.",
  };

  const plural = (n, one, many) => (Number(n) === 1 ? one : many);
  // [pattern, replacement] for texts built from variables.
  const PT_PATTERNS = [
    [/^(\d+) folders?, (\d+) files?, (.+)$/, (m) =>
      `${m[1]} ${plural(m[1], "pasta", "pastas")}, ${m[2]} ${plural(m[2], "arquivo", "arquivos")}, ${m[3]}`],
    [/^Max (\d+)×(\d+)px$/, (m) => `Máx. ${m[1]}×${m[2]} px`],
    [/^Auto \((.+)\)$/, (m) => `Automático (${m[1]})`],
    [/^(\d+) images? \(all selectable\)$/, (m) => `${m[1]} ${plural(m[1], "imagem", "imagens")} (todas selecionáveis)`],
    [/^(\d+) images?$/, (m) => `${m[1]} ${plural(m[1], "imagem", "imagens")}`],
    [/^(\d+×\d+) - Cover image \(locked\)$/, (m) => `${m[1]} - Imagem da capa (fixa)`],
    [/^(Converting & uploading|Uploading) (.+) \((\d+)\/(\d+)\)(.*)$/, (m) =>
      `${m[1] === "Uploading" ? "Enviando" : "Convertendo e enviando"} ${m[2]} (${m[3]}/${m[4]})${m[5]}`],
    [/^Reading metadata for (.+) \((\d+)\/(\d+)\)\.\.\.$/, (m) => `Lendo os dados de ${m[1]} (${m[2]}/${m[3]})...`],
    [/^Converting (.+) \((\d+)\/(\d+)\)\.\.\.$/, (m) => `Convertendo ${m[1]} (${m[2]}/${m[3]})...`],
    [/^Conversion failed, uploading original (.+)\.\.\.$/, (m) => `A conversão falhou; enviando o original ${m[1]}...`],
    [/^Upload complete: (.+)$/, (m) => `Envio concluído: ${m[1]}`],
    [/^(\d+)\/(\d+) uploaded: (\d+) optimized, (\d+) original; (\d+) failed(; (\d+) optimization warnings?)?\.$/, (m) =>
      `${m[1]}/${m[2]} enviados: ${m[3]} ${plural(m[3], "otimizado", "otimizados")}, ` +
      `${m[4]} ${plural(m[4], "original", "originais")}; ${m[5]} com falha` +
      (m[6] ? `; ${m[7]} ${plural(m[7], "aviso", "avisos")} de otimização` : "") + "."],
    [/^(.*): (Optimized and uploaded|Original uploaded|Upload failed)( - .*)?$/, (m) =>
      `${m[1]}: ${PT[m[2]]}${m[3] ? m[3].replace(" - Optimization failed: ", " - Falha na otimização: ") : ""}`],
    [/^Error: ([\s\S]*)$/, (m) => `Erro: ${m[1]}`],
    [/^Failed to load settings: ([\s\S]*)$/, (m) => `Falha ao carregar as configurações: ${m[1]}`],
    [/^Failed to delete (.+)$/, (m) => `Falha ao apagar ${m[1]}`],
    [/^Failed to preview images: ([\s\S]*)\n\nConversion will proceed normally\.$/, (m) =>
      `Não foi possível mostrar as imagens: ${m[1]}\n\nA conversão vai continuar normalmente.`],
    [/^Failed to check existing files: ([\s\S]*)$/, (m) => `Falha ao verificar os arquivos existentes: ${m[1]}`],
    [/^Folder name cannot contain (.*)$/, (m) => `O nome da pasta não pode ter ${m[1].replace("and must not be . or ..", "e não pode ser . ou ..")}`],
    [/^File name cannot contain (.*)$/, (m) => `O nome do arquivo não pode ter ${m[1].replace("and must not be . or ..", "e não pode ser . ou ..")}`],
    [/^Failed to create folder: ([\s\S]*)$/, (m) => `Falha ao criar a pasta: ${m[1]}`],
    [/^Failed to rename: ([\s\S]*)$/, (m) => `Falha ao renomear: ${m[1]}`],
    [/^Failed to move: ([\s\S]*)$/, (m) => `Falha ao mover: ${m[1]}`],
    [/^Delete font family "(.*)"\?$/, (m) => `Apagar a família de fontes "${m[1]}"?`],
    [/^Deleting (.+)\.\.\.$/, (m) => `Apagando ${m[1]}...`],
    [/^Deleted "(.*)"\.$/, (m) => `"${m[1]}" apagada.`],
    [/^Failed to delete "(.*)"\.$/, (m) => `Falha ao apagar "${m[1]}".`],
    [/^Delete error: ([\s\S]*)$/, (m) => `Erro ao apagar: ${m[1]}`],
    [/^Uploading (\d+)\/(\d+): (.+)$/, (m) => `Enviando ${m[1]}/${m[2]}: ${m[3]}`],
    [/^Failed on (.+): (.*)$/, (m) => `Falha em ${m[1]}: ${m[2].replace("unknown error", "erro desconhecido")}`],
    [/^Upload error on (.+): (.*)$/, (m) => `Erro ao enviar ${m[1]}: ${m[2]}`],
    [/^(\d+) files? → family "(.*)"$/, (m) => `${m[1]} ${plural(m[1], "arquivo", "arquivos")} → família "${m[2]}"`],
    [/^Uploaded (\d+) files? to family "(.*)"\.$/, (m) =>
      `${m[1]} ${plural(m[1], "arquivo enviado", "arquivos enviados")} para a família "${m[2]}".`],
  ];

  function translateCore(core) {
    if (Object.prototype.hasOwnProperty.call(PT, core)) return PT[core];
    for (const [re, fn] of PT_PATTERNS) {
      const m = core.match(re);
      if (m) return fn(m);
    }
    return null;
  }

  // Leading icons/symbols and trailing sort arrows stay as they are.
  const SPLIT = /^([\s\p{Extended_Pictographic}️‍⬇▶↻↺⚫📏+\/(]*)([\s\S]*?)(\s*[▲▼↑↓]?\s*)$/u;
  function translateString(text) {
    if (!text || !/[A-Za-z]/.test(text)) return null;
    const parts = text.match(SPLIT);
    if (!parts || !parts[2]) return null;
    const translated = translateCore(parts[2]);
    return translated === null ? null : parts[1] + translated + parts[3];
  }

  window.fzT = (text) => (lang === "pt" ? (translateString(String(text)) ?? String(text)) : String(text));
  if (lang !== "pt") return;

  document.documentElement.lang = "pt-BR";
  // File and folder names, user input and the technical conversion log are never translated.
  const SKIP = "script,style,textarea,code,pre,.log-container,.file-link,.folder-link,.failed-file-name," +
    "#directory-breadcrumbs,#deleteItemList,#renameItemName,#moveItemName,#imagePreviewName,#uploadPathDisplay," +
    "#folderPathDisplay,.family-info h3,[data-no-i18n]";

  function translateText(node) {
    const parent = node.parentElement;
    if (!parent || parent.closest(SKIP)) return;
    const out = translateString(node.nodeValue);
    if (out !== null && out !== node.nodeValue) node.nodeValue = out;
  }

  const ATTRS = ["placeholder", "title", "aria-label"];
  function translateElement(el) {
    if (el.closest(SKIP)) return;
    for (const a of ATTRS) {
      const v = el.getAttribute(a);
      if (v) {
        const out = translateString(v);
        if (out !== null && out !== v) el.setAttribute(a, out);
      }
    }
  }

  function translateTree(root) {
    if (root.nodeType === Node.TEXT_NODE) return translateText(root);
    if (root.nodeType !== Node.ELEMENT_NODE) return;
    translateElement(root);
    root.querySelectorAll("*").forEach(translateElement);
    const walker = document.createTreeWalker(root, NodeFilter.SHOW_TEXT);
    for (let n = walker.nextNode(); n; n = walker.nextNode()) translateText(n);
  }

  function translateTitle() {
    const parts = document.title.split(" - ");
    const out = parts.map((p) => translateCore(p) ?? p).join(" - ");
    if (out !== document.title) document.title = out;
  }

  new MutationObserver((mutations) => {
    for (const m of mutations) {
      if (m.type === "characterData") translateText(m.target);
      else m.addedNodes.forEach(translateTree);
    }
    translateTitle();
  }).observe(document.documentElement, { childList: true, subtree: true, characterData: true });

  for (const name of ["alert", "confirm", "prompt"]) {
    const original = window[name].bind(window);
    window[name] = (message, ...rest) => original(window.fzT(message), ...rest);
  }
})();

// Footer switch between Portuguese and English.
document.addEventListener("DOMContentLoaded", () => {
  const link = document.getElementById("fz-lang");
  if (!link) return;
  link.textContent = window.FZ_LANG === "pt" ? "English" : "Português";
  link.addEventListener("click", (event) => {
    event.preventDefault();
    try {
      localStorage.setItem("fz.lang", window.FZ_LANG === "pt" ? "en" : "pt");
    } catch (e) {
      // Without storage the browser language keeps deciding.
    }
    location.reload();
  });
});
