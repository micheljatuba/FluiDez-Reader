# Visão e escopo: FluiDez Reader

O FluiDez Reader é um fork mantido do [CrossInk](https://github.com/uxjulia/crossink), que por sua vez é baseado no [CrossPoint Reader](https://github.com/crosspoint-reader/crosspoint-reader). O objetivo é oferecer uma leitura fluida e confiável, com melhorias próprias de interface, economia de energia e robustez, sem abrir mão dos princípios do projeto original: firmware leve, estável e focado em leitura.

O FluiDez Reader foi testado apenas no Xteink X4 Pro. As correções do CrossInk são incorporadas periodicamente, seguindo [docs/development/upstream-sync.md](docs/development/upstream-sync.md).

## 1. Missão

Oferecer um firmware leve e de alto desempenho que aproveite ao máximo o leitor, priorizando legibilidade e usabilidade em vez de funcionalidades de "canivete suíço".

## 2. Escopo

### Dentro do escopo

*Recursos que melhoram diretamente o propósito principal do aparelho.*

* **Experiência de uso:** interfaces e interações simples, tanto no leitor quanto na navegação pelo firmware: mapeamento de botões, tela inicial, abertura de livros, marcadores e navegação no livro.
* **Renderização de documentos:** suporte a documentos (principalmente EPUB) e melhorias no motor de renderização.
* **Otimização de formatos:** leitura eficiente de EPUB (CSS e imagens) dentro das capacidades do aparelho.
* **Tipografia e legibilidade:** fontes personalizadas, hifenização e espaçamento ajustável.
* **Tela e-ink:** menos atualizações completas da tela (controle de *ghosting*) e melhorias gerais de renderização.
* **Biblioteca:** formas simples e intuitivas de organizar e navegar pela coleção, como fixar livros na tela inicial.
* **Transferência local:** envio de livros pelo servidor web local ou por padrões abertos e amplamente usados.
* **Idiomas:** interface em vários idiomas; os builds do FluiDez Reader incluem português e inglês.
* **Ferramentas de referência:** dicionário offline para consultas rápidas sem interromper a leitura.
* **Bateria e confiabilidade:** repouso, bloqueio rápido e tolerância a falhas do cartão SD.
* **Relógio (depende do aparelho):** aparelhos com RTC dedicado mantêm a hora durante o repouso; os que usam o RTC interno do ESP32 perdem precisão no sono profundo.

### Fora do escopo

*Itens recusados porque comprometem a estabilidade ou a missão do aparelho.*

* **Aplicativos interativos:** nada de bloco de notas, calculadora ou jogos. É um leitor, não um PDA.
* **Conectividade ativa:** nada de leitores de RSS, agregadores de notícias ou navegadores. Tarefas de Wi-Fi em segundo plano gastam bateria e disputam a CPU.
* **Mídia:** nada de áudio ou audiolivros.
* **Anotações complexas:** nada de notas digitadas; isso combina mais com aparelhos com melhor entrada de texto e chips mais potentes.

### Dentro do escopo, mas inviável no hardware atual

* **PDF:** PDFs têm layout fixo. Exibi-los exige renderizar páginas como imagens, com zoom e rolagem constantes, o que resulta em uma leitura ruim em e-ink.

## 3. Avaliação de ideias

Pergunta-guia: a ideia melhora a experiência central de leitura para a maioria das pessoas, sem desviar a atenção dessa leitura? Recursos que comprometam a leveza, a estabilidade ou a autonomia da bateria não entram.

---

Escopo adaptado da visão do CrossPoint Reader, também adotada pelo CrossInk.
