<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="docs/brand/fluidez-lockup-dark.png">
    <img src="docs/brand/fluidez-lockup.png" alt="FluiDez Reader" width="420">
  </picture>
</p>

<p align="center">
  <strong>A leitura flui.</strong><br>
  Firmware de leitura para e-readers Xteink, com foco em leitura fluida, economia de bateria e atualização pelo próprio aparelho.
</p>

<p align="center">
  <a href="https://github.com/micheljatuba/FluiDez-Reader/releases/latest"><img alt="Última versão" src="https://img.shields.io/github/v/release/micheljatuba/FluiDez-Reader?label=vers%C3%A3o"></a>
  <a href="https://github.com/micheljatuba/FluiDez-Reader/actions/workflows/ci.yml"><img alt="CI" src="https://github.com/micheljatuba/FluiDez-Reader/actions/workflows/ci.yml/badge.svg?branch=main"></a>
  <a href="LICENSE"><img alt="Licença MIT" src="https://img.shields.io/badge/licen%C3%A7a-MIT-blue"></a>
</p>

> **Testado apenas no Xteink X4 Pro.** Os firmwares dos outros leitores (X3, X4, X4 Classic e Seeed Studio Sticky) são gerados a partir do mesmo código, mas não foram testados.

## Sobre

O **FluiDez Reader** é um firmware de código aberto para leitores de tinta eletrônica (e-ink). Ele nasceu como fork do [CrossInk](https://github.com/uxjulia/crossink), que por sua vez é baseado no [CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader), e é modificado e mantido de forma independente por Michel Jatubá.

O nome junta *flui* (a leitura flui) e *dez* (nota dez). A identidade visual, os recursos e as correções descritos abaixo são do FluiDez Reader.

## Capturas de tela

Imagens do simulador do X4 Pro, com a interface em português. Os livros de exemplo são clássicos brasileiros em domínio público.

<table>
  <tr>
    <td align="center"><img src="docs/images/fluidez/home-grid.png" alt="Início no tema Lyra Grade, com seis livros, barras de progresso e fitas nos livros fixados" width="200"><br><sub>Início (Lyra Grade)</sub></td>
    <td align="center"><img src="docs/images/fluidez/home-carousel.png" alt="Início no tema Lyra Carousel, com a capa de Dom Casmurro em destaque" width="200"><br><sub>Início (Lyra Carousel)</sub></td>
    <td align="center"><img src="docs/images/fluidez/reader.png" alt="Primeira página do capítulo I de Dom Casmurro" width="200"><br><sub>Leitura</sub></td>
  </tr>
  <tr>
    <td align="center"><img src="docs/images/fluidez/stats.png" alt="Estatísticas de leitura de todos os livros, por hora do dia e dia da semana" width="200"><br><sub>Estatísticas de leitura</sub></td>
    <td align="center"><img src="docs/images/fluidez/settings.png" alt="Aba Sistema das Configurações, com a versão do FluiDez Reader no rodapé" width="200"><br><sub>Configurações</sub></td>
    <td align="center"><img src="docs/images/fluidez/sleep.png" alt="Tela de repouso com o logotipo do FluiDez Reader" width="200"><br><sub>Tela de repouso</sub></td>
  </tr>
</table>

## O que o FluiDez Reader traz

### Início e biblioteca

- **Livros fixados:** fixe até seis livros no topo de *Livros recentes*. Eles ficam na ordem em que foram fixados, logo depois de *Continuar lendo*, e continuam na lista quando livros mais antigos ou concluídos saem dela.
- **Menu do livro no Início:** segure a capa de um livro para *Fixar no topo* ou *Desafixar*, *Marcar como concluído* ou *Remover dos Livros recentes*, sem sair do *Início*. Em leitores sem toque, segure *Confirmar* sobre o livro selecionado.
- **Tema Lyra Grade:** grade 3x2 com seis livros (o atual, os fixados e os recentes), cada um com barra de progresso e uma fita nos fixados.
- **Lyra Carousel:** mostra até cinco livros, dois de cada lado da capa selecionada.
- **Painel:** estatísticas com rótulos curtos e maiores ("Leitura", "Restante", "Previsão") que quebram linha em vez de serem cortados, em um layout que não invade a capa, o título nem o rodapé.

### Bateria e resposta

- Teclado, escolha de Wi-Fi, transferência e sincronização Nearby e o resultado do download de fontes respeitam o *Tempo para repousar* quando ficam parados.
- Telas de espera (limpeza de cache, backup de estatísticas, ajuste do relógio, atualização de firmware, download de fontes), o *Bloqueio rápido* e os atalhos não deixam a CPU em velocidade máxima.
- O toque responde mais rápido depois que a tela fica parada.
- A contagem para repousar só começa quando downloads longos e limpezas terminam, para o resultado ficar visível.

### Confiabilidade

- As estatísticas de cada livro sobrevivem a uma falha do cartão SD durante a gravação.
- O gerenciador de arquivos web aceita pastas com `%` no nome, e o envio de arquivos diferencia arquivos otimizados, originais enviados após falha na otimização e envios com erro.
- Renomear pelos metadados do EPUB escolhe o autor, e não o tradutor.

### E mais

- **Identidade própria:** símbolo e logotipo nas telas de inicialização e de repouso, no rodapé das Configurações, no nome do aparelho e no portal web. Veja a [identidade visual](docs/brand/README.md).
- **Cinco famílias de fontes extras** para o cartão SD: Gelasio, EB Garamond, Crimson Pro, Jost e Arimo ([como gerar](#fontes-extras)).
- **Português e inglês** embutidos no firmware.
- **Atualizações por este repositório:** *Verificar atualizações* instala as versões publicadas aqui.

A lista completa de mudanças está no [CHANGELOG](CHANGELOG.md).

## Recursos herdados

Do CrossInk e do CrossPoint Reader, o FluiDez Reader herda, entre outros:

- leitura de EPUB (com hifenização, tabelas, imagens e modos de renderização), TXT e XTC;
- fontes de leitura embutidas (Lexend Deca e Bitter) e fontes extras no cartão SD;
- dicionário offline, marcadores, recortes, *Leitura focada* e Guide Dots;
- estatísticas de leitura e telas de repouso personalizáveis;
- transferência de arquivos por Wi-Fi (portal web com otimizador de EPUB, WebDAV, Calibre e OPDS), *Unidade USB* e transferência Nearby;
- sincronização de progresso com o KOReader e entre dois leitores;
- atalhos, mapeamento de botões e gestos de toque configuráveis.

Os detalhes estão na [documentação](docs/index.md) (em inglês).

## Aparelhos

| Aparelho | Arquivo do firmware | Situação |
| --- | --- | --- |
| Xteink X4 Pro | `firmware-x4-pro-v<versão>.bin` | Testado |
| Xteink X4 Classic | `firmware-x4-classic-v<versão>.bin` | Não testado |
| Xteink X3 e X4 | `firmware-x3-x4-v<versão>.bin` | Não testado |
| Seeed Studio Sticky | `firmware-sticky-v<versão>.bin` | Não testado |

## Instalação

1. Baixe o arquivo do seu aparelho na [última versão](https://github.com/micheljatuba/FluiDez-Reader/releases/latest).
2. Copie o `.bin` para qualquer pasta do cartão SD.
3. No leitor, abra **Configurações > Sistema > Atualização de firmware do cartão SD**, escolha o arquivo e confirme.

Se o leitor ainda usa o firmware original do CrossInk, a primeira instalação precisa ser feita assim (ou por USB), porque o firmware original procura atualizações no repositório do CrossInk. Para gravar pelo computador, veja o [guia de instalação](docs/installation.md) (em inglês).

## Atualizações

Depois da primeira instalação, use **Configurações > Sistema > Verificar atualizações**. O leitor consulta a versão mais recente publicada neste repositório e instala o firmware do seu aparelho.

## Fontes extras

As famílias Gelasio, EB Garamond, Crimson Pro, Jost e Arimo não estão no catálogo de download de fontes do aparelho. Gere-as no computador, a partir deste repositório:

```sh
python3 -m pip install -r lib/EpdFont/scripts/requirements.txt
python3 lib/EpdFont/scripts/build-sd-fonts.py --only Gelasio,EBGaramond,CrimsonPro,Jost,Arimo --output-dir ./generated-fonts
```

Depois, copie as pastas das famílias geradas para `/.fonts/` (ou `/fonts/`) no cartão SD. Mais detalhes em [SD card fonts](docs/sd-card-fonts.md).

## Dicas para uma boa leitura

- Mantenha as pastas com menos de 200 arquivos (o ideal é de 50 a 100). Mil livros ou mais funcionam bem se estiverem divididos em pastas, por autor, série ou gênero.
- Evite deixar todos os livros na raiz do cartão: o navegador de arquivos precisa ler e ordenar a pasta inteira antes de mostrá-la.
- EPUBs com foco em texto funcionam melhor. Arquivos com muitas imagens grandes, livros escaneados, quadrinhos e coletâneas enormes podem ficar lentos ou falhar por falta de memória.
- Como referência, EPUBs com menos de 20 MB funcionam melhor; acima de 50 MB, a chance de lentidão aumenta.
- Se um EPUB estiver lento, envie-o pelo otimizador de EPUB do portal web (em *Transferência de arquivos*).
- Use um cartão SD confiável e deixe espaço livre: configurações, progresso, cache e estatísticas ficam no cartão.

## Desenvolvimento

O FluiDez Reader usa o [PlatformIO](https://platformio.org/). Clone com os submódulos (o `freeink-sdk` é um submódulo):

```sh
git clone --recursive https://github.com/micheljatuba/FluiDez-Reader.git
cd FluiDez-Reader
pio run -e x4-pro              # compila o firmware do X4 Pro
pio run -e x4-pro -t upload    # grava pelo USB
```

| Ambiente | Aparelho |
| --- | --- |
| `x4-pro` | Xteink X4 Pro |
| `x4-classic` | Xteink X4 Classic |
| `default` | Xteink X3 e X4 |
| `sticky` | Seeed Studio Sticky |
| `x4-pro-simulator` | Simulador do X4 Pro no computador |

Guias (em inglês): [primeiros passos](docs/development/getting-started.md), [arquitetura](docs/development/architecture.md), [testes e depuração](docs/development/testing-debugging.md) e [simulador](docs/simulator.md). Com Nix, entre no ambiente de desenvolvimento com `nix develop -f nix` ou `nix-shell nix`.

### Publicar uma versão

1. Atualize `[crossink] version` no `platformio.ini` (por exemplo, `1.6-fluidez9`) e registre as mudanças no [CHANGELOG](CHANGELOG.md).
2. Faça o commit em `main` e envie a tag da versão:

   ```sh
   git tag -a v1.6-fluidez9 -m "FluiDez Reader v1.6-fluidez9"
   git push origin main v1.6-fluidez9
   ```

3. O workflow *Release* compila os quatro firmwares e publica a versão, que os leitores encontram em *Verificar atualizações*.

Para trazer novidades do CrossInk sem misturar o histórico, siga a [sincronização com o CrossInk](docs/development/upstream-sync.md).

## Contribuições

Relatos de problemas e sugestões são bem-vindos nas [issues](https://github.com/micheljatuba/FluiDez-Reader/issues). O escopo do projeto está em [SCOPE.md](SCOPE.md).

## Créditos

O FluiDez Reader existe graças a:

- [CrossInk](https://github.com/uxjulia/crossink), de Julia Nguyen, a base direta deste projeto;
- [CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader), de Dave Allie e colaboradores, o projeto original.

O histórico completo do código herdado está nesses repositórios. Aqui, o histórico começa na importação da base do CrossInk.

## Licença

[MIT](LICENSE). O aviso de copyright original foi mantido.
