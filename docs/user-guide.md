---
title: Guia do usuário
nav_order: 1.5
---

# Guia do usuário do FluiDez Reader

Este guia cobre o uso diário do aparelho. Para referências específicas, veja [Recursos do leitor](./reader-features.md), [Controles](./controls.md), [Fontes no cartão SD](./sd-card-fonts.md), [Transferência de arquivos](./webserver.md) e [Solução de problemas](./troubleshooting.md). Para gestos no Painel e na tela inicial Minimal, veja [Navegação por toque](./touch-navigation.md).

- [Guia do usuário do FluiDez Reader](#guia-do-usuário-do-fluidez-reader)
  - [1. Visão geral do hardware](#1-visão-geral-do-hardware)
    - [Layout dos botões](#1-visão-geral-do-hardware)
    - [Capturando a tela](#capturando-a-tela)
  - [2. Energia e inicialização](#2-energia-e-inicialização)
    - [Ligar / desligar](#ligar--desligar)
    - [Primeira abertura](#primeira-abertura)
  - [3. Telas](#3-telas)
    - [3.1 Tela Início](#31-tela-início)
    - [3.2 Modo de leitura](#32-modo-de-leitura)
    - [3.3 Tela Explorar arquivos](#33-tela-explorar-arquivos)
    - [3.4 Tela Biblioteca](#34-tela-biblioteca)
    - [3.5 Tela Transferência de arquivos](#35-tela-transferência-de-arquivos)
    - [3.5.1 Transferências sem fio do Calibre](#351-transferências-sem-fio-do-calibre)
    - [3.6 Configurações](#36-configurações)
      - [3.6.1 Tela](#361-tela)
      - [3.6.2 Leitor](#362-leitor)
      - [3.6.3 Controles](#363-controles)
      - [3.6.4 Sistema](#364-sistema)
      - [3.6.5 Servidores OPDS (várias bibliotecas)](#365-servidores-opds-várias-bibliotecas)
      - [3.6.6 Configurações web (Wi-Fi + OPDS)](#366-configurações-web-wi-fi--opds)
      - [3.6.7 Configuração rápida da Sincronização KOReader](#367-configuração-rápida-da-sincronização-koreader)
        - [Opção A: servidor de sincronização CrossPoint (`sync.crosspointreader.com`, padrão)](#opção-a-servidor-de-sincronização-crosspoint-synccrosspointreadercom-padrão)
        - [Opção B: servidor público legado do KOReader (`sync.koreader.rocks`)](#opção-b-servidor-público-legado-do-koreader-synckoreaderrocks)
        - [Opção C: servidor próprio (Docker Compose)](#opção-c-servidor-próprio-docker-compose)
    - [3.7 Tela de repouso](#37-tela-de-repouso)
      - [Configurações de capa](#configurações-de-capa)
      - [Imagens personalizadas](#imagens-personalizadas)
    - [3.8 Tela inicial](#38-tela-inicial)
    - [3.9 Fontes personalizadas (cartão SD)](#39-fontes-personalizadas-cartão-sd)
  - [4. Modo de leitura](#4-modo-de-leitura)
    - [Virada de página](#virada-de-página)
    - [Navegação por capítulos](#navegação-por-capítulos)
    - [Virada automática de página](#virada-automática-de-página)
    - [Virar página por inclinação (X3 e Sticky)](#virar-página-por-inclinação-x3-e-sticky)
    - [Controles de toque no leitor](#controles-de-toque-no-leitor)
    - [Navegação por notas de rodapé](#navegação-por-notas-de-rodapé)
    - [Navegação do sistema](#navegação-do-sistema)
    - [Idiomas compatíveis](#idiomas-compatíveis)
  - [5. Menu do leitor](#5-menu-do-leitor)
    - [5.1 Escolha de capítulo](#51-escolha-de-capítulo)
    - [5.2 Marcadores](#52-marcadores)
  - [6. Limitações atuais e roteiro](#6-limitações-atuais-e-roteiro)
  - [7. Solução de problemas e saída de bootloop](#7-solução-de-problemas-e-saída-de-bootloop)

## 1. Visão geral do hardware

O FluiDez Reader é compatível com vários aparelhos com diferentes layouts de botões. Nos aparelhos X3/X4 com botões frontais, por padrão eles usam o layout abaixo, da esquerda para a direita, e este guia se refere a eles por estes nomes.

<table>
  <thead>
    <tr>
      <th colspan="4"><center>Mapeamento padrão dos botões frontais</center></th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Voltar</td>
      <td>Confirmar</td>
      <td>Esquerda</td>
      <td>Direita</td>
    </tr>
  </tbody>
</table>

Em aparelhos com botões nas laterais, este guia pode se referir aos botões assim:

| Localização        | Mapeamento do botão       |
| ------------------ | ------------------------- |
| **Lado esquerdo**  | **Esquerda**/**Cima**     |
| **Lado direito**   | **Direita**/**Baixo**     |

O layout dos botões pode ser personalizado em **Configurações > Controles**.

### Capturando a tela

Quando o botão liga/desliga e o botão Baixo são pressionados ao mesmo tempo, o leitor captura a tela e salva a imagem na pasta `screenshots/`.

Como alternativa, durante a leitura de um livro, pressione o botão **Confirmar** para abrir o menu do leitor e selecione **Capturar tela**.

---

## 2. Energia e inicialização

### Ligar / desligar

Por padrão, **mantenha o botão liga/desliga pressionado por cerca de meio segundo** para colocar o aparelho em repouso ou despertá-lo. Em **Configurações > Controles > Botão liga/desliga**, você configura separadamente as ações do toque curto e do toque longo. Um toque curto só desperta o aparelho quando a **Ação do toque curto** está como **Repouso** ou **Despertar**; nos demais casos, segure o botão por cerca de meio segundo para despertá-lo. Veja [Controles](#363-controles) para mais detalhes.

Para reiniciar o aparelho (por exemplo, depois de uma atualização de firmware ou se ele travar), pressione e solte o botão Reset e, em seguida, pressione rapidamente e mantenha o botão liga/desliga pressionado por alguns segundos.

### Primeira abertura

Ao ligar o aparelho pela primeira vez, você será colocado na tela **[Início](#31-tela-início)**.

> [!NOTE]
> Em reinicializações posteriores, o firmware reabrirá automaticamente o último livro que você estava lendo.

---

## 3. Telas

### 3.1 Tela Início

A tela Início é o ponto principal de entrada do firmware. A partir dela, você pode navegar para o **[Modo de leitura](#4-modo-de-leitura)** com o livro lido mais recentemente, a tela **[Explorar arquivos](#33-tela-explorar-arquivos)**, a tela **[Biblioteca](#34-tela-biblioteca)**, a tela **[Transferência de arquivos](#35-tela-transferência-de-arquivos)** ou **[Configurações](#36-configurações)**.

### 3.2 Modo de leitura

Veja [Modo de leitura](#4-modo-de-leitura) abaixo para mais informações.

### 3.3 Tela Explorar arquivos

A tela Explorar arquivos funciona como navegador de arquivos e pastas. O caminho completo do diretório atual é mostrado no topo da tela. As extensões aparecem ao lado de cada nome de arquivo, e os diretórios são mostrados entre colchetes (por exemplo, `[folder-name]`). Diretórios ocultos podem ser exibidos nas configurações.

- **Navegar pela lista:** use **Esquerda** (ou **Cima**) ou **Direita** (ou **Baixo**) para mover o cursor de seleção para cima e para baixo por pastas e livros. Você também pode manter esses botões pressionados para rolar uma página inteira para cima ou para baixo.
- **Abrir seleção:** pressione **Confirmar** para abrir uma pasta ou começar a ler o livro selecionado. Selecionar um arquivo `.bmp` abre o visualizador de imagens.
- **Excluir arquivos ou pastas:** segure e solte **Confirmar** para abrir o menu de ações do arquivo ou da pasta selecionada e escolha **Excluir**. Você receberá a opção de confirmar ou cancelar. A exclusão de pastas é limitada a pastas vazias.
- **Ações de livros:** arquivos EPUB e XTC também podem mostrar opções como **Excluir cache do livro**, **Marcar como concluído** e **Registrar estatísticas de leitura** no mesmo menu de ações. Desligar o registro de um livro mantém as estatísticas já salvas; só interrompe os registros novos daquele livro.

### 3.4 Tela Biblioteca

A **Biblioteca** substitui a antiga tela Livros recentes e lista os livros compatíveis de todo o cartão SD. Na primeira visita, ela lê o cartão para montar a lista; nas visitas seguintes, reaproveita a lista e a atualiza depois de mudanças em arquivos ou transferências.

- **Encontrar um livro:** use **Buscar** para filtrar por título ou autor. Com **Usar metadados do livro** desligado, os livros são mostrados e buscados pelo nome do arquivo.
- **Ordenar:** escolha **Data de adição**, **Título**, **Autor (sobrenome)**, **Autor (nome)**, **Abertos recentemente**, **Série** ou **Gênero**. A ordem pode ser invertida. **Abertos recentemente** mostra só o seu histórico de leitura, e não todos os livros do cartão.
- **Botões:** use **Cima/Baixo** para selecionar um livro e **Confirmar** para abri-lo. Segure **Confirmar** para as ações do livro. **Esquerda** abre a ordenação; **Direita** abre o menu com **Buscar**, **Configurações** e **Atualizar biblioteca**.
- **Toque:** toque em um livro para abri-lo, mantenha-o pressionado para as ações do livro e deslize para rolar. Use os controles de busca, configurações, atualização e ordenação no topo.
- **Configurações da Biblioteca:** abra as **Configurações** da Biblioteca para escolher entre metadados do EPUB ou nomes de arquivo, linhas compactas ou expandidas, e lista ou grade de capas em **Visualização de abertos recentemente**. Também é possível ocultar livros concluídos, mostrar série e gênero e filtrar os formatos exibidos.
- **Ações de livros:** livros EPUB e XTC podem oferecer **Estatísticas de leitura** sem abrir o livro, quando o registro está ativado no aparelho e no livro. O menu também traz controles do livro, como **Marcar como concluído** e **Registrar estatísticas de leitura**, quando disponíveis.

Se os arquivos foram alterados por fora enquanto o leitor ficou ligado, use **Atualizar biblioteca** para ler o cartão de novo. **Data de adição** usa a data de criação do arquivo quando disponível, então um programa de cópia que preserva datas antigas pode afetar essa ordem.

#### Livros fixados

- **Fixar livros:** no menu de ações de um livro do seu histórico de leitura (segurando **Confirmar** ou mantendo o livro pressionado), escolha **Fixar no topo**. Até seis livros fixados aparecem primeiro em **Abertos recentemente**, na ordem em que foram fixados. Escolha **Desafixar** para devolver o livro à posição cronológica.
- **Manter livros fixados:** livros fixados permanecem no histórico quando livros mais novos empurram os antigos para fora da lista de 18 livros e quando **Limpar livros lidos da lista de recentes** está ativado. Escolha **Remover dos Livros recentes** ou exclua o livro para removê-lo.
- **Tela Início:** **Continuar lendo** ainda abre o livro que você leu mais recentemente. Livros fixados aparecem depois dele nos temas de Início com várias capas (**Lyra Extended** mostra 3 livros, **Lyra Carousel** mostra 5 e **Lyra Grade** mostra 6). Em temas de uma capa, trocar o livro do Início (por exemplo, deslizando para a esquerda ou segurando **Confirmar**) alterna entre o livro atual e até dois livros fixados; com um livro fixado, o livro lido anteriormente permanece no ciclo.
- **Fixar a partir do Início:** em qualquer tema de Início, mantenha a capa de um livro pressionada (ou, em temas com várias capas sem toque, segure **Confirmar** no livro selecionado) para abrir **Fixar no topo** / **Desafixar**, **Marcar como concluído** e **Remover dos Livros recentes** sem sair do Início.

### 3.5 Tela Transferência de arquivos

A tela Transferência de arquivos permite enviar e gerenciar arquivos no aparelho.
Escolha **Entrar em uma rede**, **Calibre sem fio** ou **Criar hotspot** para
iniciar o servidor web no modo selecionado.

Veja o [guia de Transferência de arquivos](./webserver.md) para detalhes de conexão e envio.

O gerenciador de arquivos web pode enviar, baixar, renomear, mover e excluir arquivos no aparelho.

A interface web também é compatível com **WebDAV**, permitindo montar o aparelho como unidade de rede e gerenciar arquivos diretamente pelo gerenciador de arquivos do computador.

Links de download para arquivos que já estão no aparelho ficam disponíveis na interface web, para você recuperar livros ou capturas de tela por Wi-Fi sem conectar um cabo.

Um **indicador de intensidade do sinal Wi-Fi** (dBm) é exibido na tela durante sessões do servidor web em rede conectada.

A mesma tela também tem **Receber arquivo**, que recebe um livro ou imagem
compatível diretamente de outro aparelho FluiDez Reader próximo, sem entrar em
uma rede Wi-Fi. Veja [Transferência por proximidade](./nearby-file-transfer.md)
para o fluxo completo de envio e recebimento.

No X4 Pro, a tela também inclui **Unidade USB**. Isso expõe o cartão SD do
leitor a um computador via USB para gerenciamento direto de arquivos. Veja as
[instruções de Unidade USB](./installation.md#unidade-usb) para o comportamento
seguro de ejeção e desconexão.

> [!TIP]
> Usuários avançados podem gerenciar arquivos de forma programática com os
> mesmos endpoints HTTP usados pela interface web. A interface do navegador é
> o caminho compatível para gerenciamento normal de arquivos.

### 3.5.1 Transferências sem fio do Calibre

O FluiDez Reader permite enviar livros pelo Calibre com seu próprio plugin de
dispositivo FluiDez Reader.

1. Baixe `fluidez-reader-calibre-plugin.zip` na
   [versão mais recente do FluiDez Reader](https://github.com/micheljatuba/FluiDez-Reader/releases/latest/download/fluidez-reader-calibre-plugin.zip).
2. No Calibre, abra **Preferências > Plugins > Carregar plugin a partir de arquivo** e selecione
   esse ZIP sem extraí-lo. Reinicie o Calibre se ele pedir.
3. No aparelho, abra **Transferência de arquivos > Calibre sem fio** e entre na
   mesma rede Wi-Fi de 2,4 GHz do computador.
4. Espere a seção **Status** da tela mostrar "Conectado ao Calibre". Mantenha a tela
   Calibre sem fio aberta e use a ação **Enviar para o dispositivo** do Calibre. A tela do
   aparelho mostra o progresso da transferência, o livro recebido ou a falha no envio e
   quantos livros chegaram.

O plugin encontra o leitor automaticamente e também tenta `fluidez.local` e o
endereço do hotspot `192.168.4.1`. Se você sair da tela Calibre sem fio, o Calibre
mostra o leitor como desconectado em cerca de 15 segundos e conecta de novo sozinho
quando a tela é reaberta. Configurações e solução de problemas estão
descritas no [README do plugin](../calibre-plugin/README.md) (em português).

### 3.6 Configurações

A tela Configurações agrupa opções por finalidade. As opções exatas podem variar
conforme o modelo do aparelho e a compilação.

#### 3.6.1 Tela

- **Tela de repouso**: qual tela de repouso exibir quando o aparelho entra em repouso:
  - "Escuro" (padrão) - a tela de repouso padrão com o logotipo escuro do FluiDez Reader
  - "Claro" - a mesma tela de repouso padrão, em fundo branco
  - "Personalizado" - imagens personalizadas do cartão SD; veja [Tela de repouso](#37-tela-de-repouso) abaixo para mais informações
  - "Capa" - a imagem da capa do livro (observação: isto é experimental e pode não funcionar como esperado)
  - "Nenhum" - uma tela em branco
  - "Capa + personalizado" - a imagem da capa do livro durante a leitura ativa, usando o comportamento "Personalizado" nos demais casos
  - "Sobreposição da página" - usa uma imagem para sobrepor a página atual. Funciona melhor com arquivos `.png` transparentes ou arquivos `.bmp` em preto e branco.
  - "Estatísticas de leitura" - estatísticas recentes de leitura na tela de repouso; usa **Minimal** enquanto o registro estiver desligado no aparelho ou no livro recente
  - "Minimal" - uma tela de repouso minimalista
  - "Minimal Stats" - uma tela de repouso minimalista com estatísticas em aparelhos compatíveis; usa **Minimal** enquanto o registro estiver desligado no aparelho ou no livro recente
  - "Painel" - uma tela de repouso no estilo painel, baseada no tema Painel
  - "Retomada rápida" - mantém o conteúdo atual visível durante o repouso

- **Modo da capa da tela de repouso**: como exibir a capa do livro quando a tela de repouso "Capa" está selecionada:
  - "Ajustar" (padrão) - reduz a imagem para caber centralizada na tela, preenchendo com bordas brancas conforme necessário
  - "Recortar" - reduz a imagem e recorta conforme necessário para tentar preencher a tela (observação: isto é experimental e pode não funcionar como esperado)

- **Filtro capa tela repouso**: qual filtro será aplicado à capa do livro quando a tela de repouso "Capa" está selecionada:
  - "Nenhum" (padrão) - a imagem da capa será convertida para tons de cinza e exibida como está
  - "Contraste" - a imagem será exibida em preto e branco sem conversão para tons de cinza
  - "Invertido" - a imagem será invertida, como branco e preto, e exibida sem conversão para tons de cinza

- **Retomada rápida após tempo limite**: ativa a tela de repouso "Retomada rápida" quando o aparelho entra em repouso por inatividade (Sistema > Tempo para repousar). Isso é útil para retomar a leitura rapidamente sem esperar o aparelho despertar completamente e carregar o livro. Quando ativado, substitui o Modo da capa da tela de repouso.

- **Ocultar % da bateria**: configure onde suprimir a exibição da porcentagem da bateria na barra de status; o ícone da bateria continuará sendo mostrado:
  - "Nunca" (padrão) - sempre mostra a porcentagem da bateria
  - "No leitor" - mostra a porcentagem da bateria em todos os lugares, exceto no modo de leitura
  - "Sempre" - sempre oculta a porcentagem da bateria

- **Ocultar relógio**: em aparelhos com relógio em tempo real, escolha se o
  relógio aparece em todos os lugares, fica oculto apenas no leitor ou fica sempre oculto.

- **Freq. atualiz.**: define com que frequência a tela faz uma atualização completa durante a leitura para reduzir fantasmas; as opções são a cada 1, 5, 10, 15 ou 30 páginas.

- **Tema da interface**: define qual tema da interface usar:
  - "FluiDez Estante" (padrão) - o tema próprio do FluiDez: a capa do livro atual com o progresso num anel, o botão **Continuar** e uma estante com as lombadas dos livros recentes; o livro selecionado sobe na estante e o nome dele aparece embaixo
  - "FluiDez Cartões" - o tema próprio do FluiDez em cartões: um cartão grande com a capa e o progresso do livro atual, três cartões com tempo de leitura, dias seguidos e livros concluídos, e as capas recentes
  - "FluiDez Fluxo" - o tema próprio do FluiDez só com texto: título grande, autor, progresso e a lista **A seguir**. Não lê nenhuma capa do cartão, por isso é a tela Início mais rápida

  Nos três temas FluiDez, o menu fica numa barra de ícones no rodapé. O nome do item selecionado aparece logo acima da barra.
  - "Clássico" - o tema original
  - "Minimal" - um tema minimalista com capa grande do livro
  - "Painel" - layout de início no estilo painel, com estatísticas de leitura ao lado da capa; os rótulos encolhem ou quebram linha para que o texto nunca cubra o livro
  - "Lyra" - um tema com ícones simples destacando o livro atual
  - "Lyra Extended" - Lyra, mas mostra 3 livros em vez de 1 na **[tela Início](#31-tela-início)**
  - "Lyra Carousel" - layout inicial Lyra em carrossel, com até 5 livros
  - "Lyra Grade" - menu de ícones do Lyra Carousel com uma grade 3x2 de 6 livros (atual, fixados e recentes), cada um com barra de progresso e fita nos fixados
  - "RoundedRaff" - um tema arredondado com estilo visual adicional
  - "Cover Grid" (aparelhos com PSRAM, como Sticky e X4 Pro) - mostra o livro atual e até seis outras capas; toque em uma capa para abrir o livro

  Temas que mostram estatísticas de leitura as ocultam enquanto **Registrar estatísticas de leitura** estiver desligado.

- As opções de exibição e filtro da lista de livros agora ficam em **Biblioteca > Configurações**; veja [Tela Biblioteca](#34-tela-biblioteca).

- **Ajuste desbotamento ao sol**: configure se ativa uma correção por software para o problema em que modelos X4 brancos podem desbotar quando usados sob luz solar direta:
  - "DESL." (padrão) - desativa a correção
  - "LIG." - ativa a correção

- **Luz frontal e agendamento de despertar** (em aparelhos compatíveis com luz frontal e
  relógio em tempo real): abra **Configurações > Tela > Luz frontal**.
  - **Restaurar luz ao despertar**: quando ativado, a luz frontal que estava ligada antes
    do repouso volta a ligar quando o aparelho desperta. Se a luz estava desligada antes
    do repouso, um agendamento completo ainda pode decidir se ela deve ligar.
  - **Agendamento**: ative o agendamento diário de despertar e defina **Início**
    e **Fim** no horário local. Os horários usam incrementos de um minuto. O agendamento
    inclui o horário de Início e exclui o horário de Fim, e pode cruzar a meia-noite
    (por exemplo, de 21:00 a 7:00). Início e Fim devem estar definidos e ser diferentes;
    caso contrário, o agendamento fica inativo.
  - O agendamento é verificado quando o aparelho inicia ou desperta, não continuamente
    enquanto ele já está acordado. Restaurar luz ao despertar tem precedência quando a luz
    estava ligada antes do repouso. Defina o horário local do aparelho e o deslocamento UTC em
    **Configurações > Sistema > Dispositivo** para que o agendamento use o relógio esperado.
    Quando o agendamento está desativado ou uma ponta não foi definida, o valor da ponta
    aparece como `--`; horários salvos das pontas são mantidos para reativação posterior.

> [!NOTE]
> Um indicador de carregamento aparece no ícone da bateria sempre que o aparelho está carregando ativamente.

#### 3.6.2 Leitor

**Configurações globais e livros individuais:** as escolhas em **Configurações > Leitor** são o padrão para seus livros EPUB. Escolhas de layout, como família e tamanho da fonte e margens, podem ser salvas para um único livro nas abas **Fonte** e **Layout** do menu do livro. Uma escolha salva no livro prevalece sobre a configuração global correspondente; o que você não alterou naquele livro continua seguindo o padrão global. Por exemplo, um livro com tamanho de fonte próprio ainda recebe uma mudança posterior nas margens globais. Se você abrir as configurações globais pelo painel deslizante de um leitor com toque, mudanças herdadas de fonte e layout também valem para o livro aberto quando você voltar. **Modo escuro** e **Números de página estáveis** são configurações globais.

Se uma mudança global não afetar um EPUB, abra a aba **Configurações** do menu do leitor e escolha **Redefinir configurações de leitura do livro**, ou use o menu de ações do livro em **Explorar arquivos** ou **Biblioteca**. Isso remove as escolhas salvas daquele livro para que ele volte a seguir os padrões globais atuais; mantém progresso, marcadores, recortes e estatísticas, e não redefine outros livros nem as configurações globais. Livros salvos em versões antigas podem precisar dessa redefinição antes de herdar mudanças globais. **Excluir cache do livro** não redefine essas escolhas.

- **Família da fonte**: escolha a fonte usada para leitura:
  - "Lexend Deca" (padrão)
  - "Bitter"

- **Tamanho da fonte**: escolha entre os tamanhos disponíveis para a fonte selecionada e o firmware. Leitores ESP32-S3 usam fontes TTF escaláveis com todos os tamanhos inteiros de **8 a 22 pt**; veja [Fontes TTF escaláveis](./scalable-fonts.md).

- **Espaçamento entre linhas**: ajusta a altura das linhas como porcentagem.

- **Espaçamento entre palavras**: em livros EPUB, escolha **Normal** ou um dos quatro níveis
  mais largos de espaçamento entre palavras. Abra o menu do leitor e selecione **Fonte > Espaçamento entre linhas/palavras > Espaçamento entre palavras**. Alterar essa opção redistribui o livro
  atual, então as posições das páginas podem mudar; ela não está disponível para livros TXT.

- **Margem da tela**: controla as margens da tela no Modo de leitura entre 5 e 40 pixels, em incrementos de 5 pixels.

- **Alinhamento do parágrafo**: define o alinhamento dos parágrafos; as opções são "Justificar" (padrão), "Esquerda", "Centralizar", "Direita" ou "Estilo do livro".

- **Números de página da editora**: mostra números de página fornecidos pelo EPUB quando o
  livro os inclui.

  Observação: isto reserva 5 px da margem esquerda da tela para abrir espaço para os números de página. Só é perceptível se suas margens `Esquerda/Direita` estiverem definidas como `5`. Se a página não tiver número de página da editora, suas margens podem parecer desiguais.

- **Hifenização**: define se o texto será hifenizado no Modo de leitura; as opções são "LIG." ou "DESL.". Com "DESL.", o texto coreano quebra nos espaços; com "LIG.", uma palavra coreana também pode ser dividida no fim da linha, sem desenhar hífen.

- **Orientação**: define a orientação da tela para leitura de arquivos EPUB:
  - "Retrato" (padrão) - orientação retrato padrão
  - "Paisagem horário" - paisagem, girada no sentido horário
  - "Retrato 180°" - retrato, de cabeça para baixo
  - "Paisagem anti-horário" - paisagem, girada no sentido anti-horário

- **Espaçamento extra entre parágrafos**: define como tratar quebras de parágrafo:
  - "LIG." - espaço vertical será adicionado entre parágrafos no Modo de leitura
  - "DESL." - os parágrafos não terão espaço vertical adicional; o recuo da primeira linha definido pelo livro é exibido quando existir

- **Estilo embutido**, **Imagens**, **Leitura focada** e
  **Pontos guia** estão disponíveis diretamente nas configurações do Leitor. Veja
  [Recursos do leitor](./reader-features.md) para o comportamento, incluindo o guia de
  [Leitura focada](./reader-features.md#focus-reading).

- **Desativar tela sensível ao toque**: bloqueia a entrada por toque enquanto um livro está aberto, mantendo o toque disponível nos menus do leitor para você poder reativá-la. Para desativar só as viradas de página por toque, use **Próxima página** e **Página anterior** em **Configurações > Controles > Toques e gestos**.

- **Barras de status**: configure as barras superior e inferior separadamente para leitura de EPUB, TXT e XTC. Abra **Configurações > Leitor > Barras de status** ou **Configurações > Barras de status** no menu do leitor EPUB. Selecione uma barra para pré-visualizá-la na posição de leitura e escolher seus itens:
  - As barras superior e inferior têm três espaços à esquerda, um no centro e três à direita.
  - Os itens incluem contagem de páginas do capítulo, porcentagem de progresso do livro (`10%`, `10.1%` ou `10.12%`), barra de progresso, título, tempo restante, bateria e, em aparelhos com relógio, hora e data.
  - Números de página estáveis - adiciona uma contagem de páginas de referência a um espaço quando o EPUB tem metadados de referência; veja [Números de página estáveis](./reader-features.md#stable-page-numbers).

Em leitores com tela sensível ao toque, quando **Toque para ocultar barra de status** está ativado (o padrão em **Configurações > Controles > Toques e gestos**), toque na área da barra de status durante a leitura para mostrar ou ocultar a barra inteira na sessão de leitura atual. Esse toque fica disponível enquanto a entrada por toque do leitor estiver ativada. A alternância rápida não altera o layout da página nem as quebras de página; toque de novo na mesma região da barra de status para restaurar uma barra oculta. Use **Barras de status** para escolher quais itens da barra aparecem.

#### 3.6.3 Controles

- **Botão liga/desliga**: configure as ações de toque curto e toque longo de forma independente. Em aparelhos compatíveis, também é possível configurar o atalho **Liga/desliga + Cima**.

- **Botões frontais**: configure o remapeamento dos botões frontais, a sensibilidade à orientação, o comportamento de toque longo apenas no leitor, a ação Voltar e a ação Menu.

- **Botões laterais**: atribua uma **Ação do toque curto** e uma **Ação do toque longo** separadamente para **Esquerda/Cima** e **Direita/Baixo** durante a leitura. Escolha virar página ou capítulo, mudar o tamanho da fonte, girar a página (anti-horário, horário ou 180°) ou outros atalhos disponíveis. Defina uma ação como **Ignorar** para deixar aquele toque sem função. Layouts antigos são migrados para ações individuais equivalentes. **Sensível à orientação** e o atalho combinado dos botões laterais continuam disponíveis em aparelhos compatíveis. Ações de rotação e de tamanho da fonte valem para texto EPUB/TXT; elas não giram nem redimensionam páginas XTC pré-renderizadas.

- **Comportamento de toque longo**: define se manter botões frontais de virar página pressionados não faz nada, pula para o capítulo seguinte/anterior ou altera a orientação do leitor.

- **Ação do toque curto / Ação do toque longo do botão liga/desliga**: escolha o que fazem um toque curto e um toque de cerca de 0,4 segundo. As ações disponíveis incluem:
  - "Ignorar" (padrão do toque curto) - não faz nada para essa duração de toque
  - "Repouso" (padrão do toque longo) - coloca em repouso o aparelho acordado; quando escolhida como **Ação do toque curto**, também permite despertar com um toque curto
  - "Repouso" (a segunda opção com esse nome) - coloca o aparelho em repouso sem habilitar o despertar por toque curto
  - "Despertar" - quando escolhida como **Ação do toque curto**, permite despertar o aparelho com um toque curto; não faz nada enquanto o aparelho está acordado
  - "Próxima página" - vira para a próxima página durante a leitura
  - "Alternar marcador", "Estatísticas de leitura", "Marcar como concluído", "Atualizar tela", "Alterar fonte", "Pontos guia", "Leitura focada", "Virada automática", "Sincronizar progresso", "Transferência de arquivos", "Calibre sem fio", "Entrar em uma rede", "Criar hotspot", "Capturar tela", "Modo escuro", "Explorar arquivos", "Biblioteca" ou "Criar recorte" - executa a ação correspondente
  - "Notas de rodapé" - escolhe uma referência de nota na página atual, com uma lista quando necessário; veja [Navegação por notas de rodapé](#navegação-por-notas-de-rodapé).

- **Retorno rápido das notas**: aparece em **Configurações > Controles > Botão liga/desliga** depois que você atribui **Notas de rodapé** à ação de toque curto ou longo do botão liga/desliga, ou à ação de toque longo de Voltar ou Menu. Quando ativado, um toque curto no botão liga/desliga volta da página da nota para a página de leitura original.

- **Toques e gestos** (aparelhos com tela sensível ao toque): configure as interações de toque
  disponíveis durante a leitura em **Configurações > Controles > Toques e gestos**. O
  submenu inclui:
  - **Próxima página** e **Página anterior**: escolha quais toques e deslizes avançam ou
    voltam uma página. Cada direção pode ser definida de forma independente como **Toque e deslize** (padrão),
    **Somente toque**, **Somente deslize**, **Toque invertido** ou **Desativado**.
  - **Tam. fonte por pinça** (aparelhos multitoque): ativa ou desativa a alteração do
    tamanho da fonte com pinça de dois dedos nos leitores EPUB e TXT.
  - **Girar com dois dedos** (aparelhos multitoque): ativa ou desativa
    girar a orientação de leitura torcendo dois dedos nos leitores EPUB e TXT.
  - **Toque para ocultar barra de status**: ativa ou desativa tocar na área visível da barra de status
    para mostrá-la ou ocultá-la na sessão de leitura atual.
  - **Deslizar com dois dedos** (aparelhos multitoque): atribui uma ação a cada
    direção de deslize com dois dedos. As ações disponíveis dependem do aparelho e
    do formato do leitor.
  - **Gestos nas bordas**: atribua ações a deslizes para cima e para baixo nas bordas esquerda e direita
    da tela; veja [Gestos nas bordas](./controls.md#edge-gestures).
    O submenu não aparece em aparelhos sem tela sensível ao toque, e as entradas
    multitoque aparecem apenas quando o hardware oferece suporte. Veja [Controles de toque no leitor](#controles-de-toque-no-leitor) para os detalhes dos gestos.

#### 3.6.4 Sistema

- **Tempo para repousar**: define a duração de inatividade antes de o aparelho entrar automaticamente em repouso. Os valores são em minutos, com uma opção "Nunca" no fim da faixa. Teclados, a lista de redes Wi-Fi, as telas de prontidão e resultado de proximidade e os resultados de download também seguem esse tempo limite quando ficam ociosos. O aparelho permanece acordado enquanto procura, conecta, transfere, sincroniza ou baixa; texto digitado mas ainda não confirmado é descartado se ele entrar em repouso.

- **Tela inicial personalizada**: ativa ou desativa telas iniciais personalizadas (ativadas por
  padrão). Quando desativado, o FluiDez Reader usa o logotipo padrão em uma inicialização fria e mantém a tela de repouso atual visível ao despertar pelo botão liga/desliga, mesmo se uma imagem personalizada ou pasta de tela inicial estiver configurada. Desativar isso não remove a imagem selecionada nem as pastas; ative novamente para usá-las.

- **Dispositivo**: define o nome do aparelho e o tempo para repousar. Aparelhos com
  relógio em tempo real também expõem formato do relógio, deslocamento UTC e uma ação de sincronização.

- **Arquivos e cache**: configure arquivos ocultos, extensões de arquivo, visualização do navegador de arquivos, comportamento de livros concluídos e limpeza do cache de leitura.

- **Estatísticas de leitura**: ligue ou desligue o registro de estatísticas no aparelho todo, configure o filtro de tempo ocioso e acesse ações de backup/redefinição de estatísticas de todo o período. Desligar o registro pausa novas estatísticas por livro e de todo o período, mas mantém o histórico e as estimativas de Tempo restante já salvas no aparelho. Entradas de estatísticas e exibições de estatísticas registradas em menus e temas ficam ocultas com o registro desligado; o Painel ainda pode mostrar uma estimativa salva de Tempo restante. Ligue o registro de novo para ver o histórico salvo e voltar a registrar. Tempo e páginas do período desligado não são somados depois. Veja [Recursos do leitor](./reader-features.md#turn-tracking-on-or-off) para os controles por livro.

- **Redes Wi‑Fi**: conecte-se a redes Wi-Fi para transferências de arquivos e atualizações de firmware.

- **Sincronização KOReader**: opções para configurar o KOReader para sincronizar progresso de leitura. **Inteligente** é o padrão para novas configurações e resolve automaticamente decisões simples de envio/recebimento. Arquivos de credenciais existentes mantêm **Perguntar sempre** quando migrados; você pode alterar o Modo de sincronização a qualquer momento se preferir confirmação manual.

- **Servidores OPDS**: gerencie uma ou mais bibliotecas OPDS [(Open Publication Distribution System)](https://en.wikipedia.org/wiki/Open_Publication_Distribution_System) para navegar e baixar livros. Veja [Servidores OPDS (várias bibliotecas)](#365-servidores-opds-várias-bibliotecas) abaixo.

- **Verificar atualizações** e **Atualização de firmware do cartão SD**: verificam atualizações de firmware
  via Wi-Fi ou instalam um `firmware.bin` colocado no cartão SD. O FluiDez Reader
  verifica as [versões do FluiDez Reader](https://github.com/micheljatuba/FluiDez-Reader/releases)
  e oferece uma versão apenas quando seu número de compilação `fluidez` é mais novo. As atualizações
  são instaladas por sua conta e risco; veja [Instalação](./installation.md).

- **Idioma**: define o idioma da interface. As versões do FluiDez Reader incluem inglês e
  português brasileiro. O código-fonte inclui traduções para 28 idiomas: inglês,
  espanhol, francês, alemão, tcheco, português brasileiro, russo, sueco,
  romeno, catalão, ucraniano, bielorrusso, italiano, polonês, finlandês, dinamarquês,
  holandês, turco, cazaque, húngaro, lituano, esloveno, valenciano, hebraico,
  vietnamita, eslovaco, português (Portugal) e árabe. Uma compilação oferece apenas os
  idiomas listados em `custom_i18n_builtin_langs` em `platformio.ini`.

#### 3.6.5 Servidores OPDS (várias bibliotecas)

O FluiDez Reader permite salvar vários servidores OPDS e alternar entre eles ao navegar por catálogos.

1. Abra **Configurações -> Sistema -> Servidores OPDS**.

2. Selecione **Adicionar servidor** para criar uma nova entrada ou selecione um servidor existente para editá-lo.

3. Configure estes campos:
   - **Nome do servidor**: nome de exibição opcional (por exemplo, "Calibre de casa" ou "Catálogo público").

   - **URL do servidor OPDS**: URL raiz completa do catálogo (para o Servidor de conteúdo do Calibre, geralmente termina com `/opds`).

   - **Nome de usuário / Senha**: credenciais opcionais para servidores autenticados.

4. Use **Excluir servidor** dentro de uma entrada de servidor para removê-la.

Notas de comportamento:

- Você pode armazenar até 8 servidores OPDS.
- A autenticação OPDS oferece suporte a HTTP Basic auth. Se você usa o Servidor de conteúdo do Calibre com autenticação ativada, defina como Basic (não Digest).

Você também pode gerenciar servidores OPDS pela interface web enquanto estiver no modo Transferência de arquivos:

1. Conecte-se à interface web do aparelho.
2. Abra `http://<device-ip>/settings`.
3. Use o cartão **Servidores OPDS** para adicionar, editar ou excluir entradas.

Para gerenciamento de redes Wi-Fi pela web, veja [Transferência de arquivos](./webserver.md).

#### 3.6.6 Configurações web (Wi-Fi + OPDS)

Enquanto estiver no modo **Transferência de arquivos**, a página de configurações web inclui cartões de gerenciamento para **Redes Wi‑Fi** e **Servidores OPDS**.

1. No aparelho: abra **Transferência de arquivos** e conecte por **Entrar em uma rede** ou **Criar hotspot**.
2. Em um navegador, abra `http://<device-ip>/settings` ou `http://fluidez.local/settings`.
3. Em **Redes Wi‑Fi**, adicione, edite ou exclua entradas de rede salvas (SSID + senha opcional).
4. Em **Servidores OPDS**, adicione, edite ou exclua catálogos OPDS.

Notas de comportamento:

- Senhas nunca são mostradas de volta na interface web depois de salvas.
- Deixar Senha em branco durante a edição mantém a senha salva existente sem alterações.
- A interface web pode salvar SSIDs de redes ocultas, mas conectar a redes ocultas ainda depende do fluxo de conexão Wi-Fi no aparelho.

#### 3.6.7 Configuração rápida da Sincronização KOReader

O FluiDez Reader pode sincronizar o progresso de leitura com servidores de sincronização compatíveis com KOReader.
Ele também interoperará com aplicativos/aparelhos KOReader quando eles usarem o mesmo servidor e as mesmas credenciais.

##### Opção A: servidor de sincronização CrossPoint (`sync.crosspointreader.com`, padrão)

Quando **URL do servidor de sincronização** fica vazio, o FluiDez Reader usa o servidor gratuito de sincronização CrossPoint em `https://sync.crosspointreader.com`. Ele fala o protocolo padrão de sincronização KOReader (então aplicativos KOReader também podem usá-lo) e também armazena uma posição exata de spine/página para sincronização sem perdas entre leitores.

1. Em cada aparelho FluiDez Reader:
   - Vá para **Configurações -> Sistema -> Sincronização KOReader**.

   - Defina **Nome de usuário** e **Senha** (digite a senha em texto puro; o FluiDez Reader calcula o MD5 internamente, e use os mesmos valores em todos os aparelhos).

   - Deixe **URL do servidor de sincronização** vazia (ou defina como `https://sync.crosspointreader.com`).

   - No primeiro aparelho, execute **Criar conta** uma vez para criar a conta diretamente pelo aparelho. Em todos os outros aparelhos, execute apenas **Autenticar**.

As contas são por servidor. Credenciais existentes de `sync.koreader.rocks` não existem no servidor CrossPoint; crie a conta novamente com o mesmo nome de usuário/senha ou use a Opção B para continuar usando o servidor legado.

##### Opção B: servidor público legado do KOReader (`sync.koreader.rocks`)

Use esta opção se você já sincroniza aparelhos KOReader com o servidor público oficial.

1. Em cada aparelho FluiDez Reader:
   - Vá para **Configurações -> Sistema -> Sincronização KOReader**.

   - Defina **URL do servidor de sincronização** como `https://sync.koreader.rocks` (obrigatório; uma URL vazia agora aponta para o servidor CrossPoint).

   - Defina **Nome de usuário** e **Senha** com suas credenciais existentes da Sincronização KOReader.

   - Execute **Autenticar**.

2. Se você ainda não tem uma conta, execute **Criar conta** no aparelho ou registre uma vez com curl:

```bash
USERNAME="user"
******
PASSWORD_MD5="$(printf '%s' "$PASSWORD" | openssl md5 | awk '{print $2}')"

curl -i "https://sync.koreader.rocks/users/create" \
  -H "Accept: application/vnd.koreader.v1+json" \
  -H "Content-Type: application/json" \
  --data "{\"username\":\"$USERNAME\",\"password\":\"$PASSWORD_MD5\"}"
```

Quando isso retornar `HTTP 402` com `{"code":2002,"message":"Username is already registered."}`, escolha outro nome de usuário ou use essa conta existente.

##### Opção C: servidor próprio (Docker Compose)

1. Inicie um servidor de sincronização:

```bash
mkdir -p kosync-quickstart
cd kosync-quickstart

cat > compose.yaml <<'YAML'
services:
  kosync:
    image: koreader/kosync:latest
    ports:
      - "7200:7200"
      - "17200:17200"
    volumes:
      - ./data/redis:/var/lib/redis
    environment:
      - ENABLE_USER_REGISTRATION=true
    restart: unless-stopped
YAML

# Docker
docker compose up -d

# Podman (alternative)
podman compose up -d
```

> [!NOTE]
> `ENABLE_USER_REGISTRATION=true` é conveniente para a primeira configuração. Depois de criar seus usuários, defina como `false` (ou remova) para evitar registros inesperados.

2. Verifique o servidor:

```bash
curl -H "Accept: application/vnd.koreader.v1+json" "http://<server-ip>:17200/healthcheck"
# Expected: {"state":"OK"}
```

3. Registre um usuário uma vez.
   O FluiDez Reader autentica na Sincronização KOReader (`koreader/kosync`) usando uma chave MD5, então registre usando o MD5 da sua senha:

> [!WARNING]
> Enviar uma senha reutilizável derivada de MD5 por HTTP puro é inseguro.
> Crie credenciais exclusivas apenas para sincronização e não reutilize senhas de contas principais.
> Prefira `https://<server-ip>:7200` sempre que o tráfego sair de uma LAN totalmente confiável ou quando usar redes não confiáveis.
> Use `curl -k` apenas para testes com certificado autoassinado.

```bash
USERNAME="user"
******
PASSWORD_MD5="$(printf '%s' "$PASSWORD" | openssl md5 | awk '{print $2}')"

curl -i "http://<server-ip>:17200/users/create" \
  -H "Accept: application/vnd.koreader.v1+json" \
  -H "Content-Type: application/json" \
  --data "{\"username\":\"$USERNAME\",\"password\":\"$PASSWORD_MD5\"}"
```

Se isso retornar `HTTP 402` com `{"code":2002,"message":"Username is already registered."}`, a conta já existe.

4. Em cada aparelho:
   - Vá para **Configurações -> Sistema -> Sincronização KOReader**.

   - Defina **Nome de usuário** e **Senha** (digite a senha em texto puro; o FluiDez Reader calcula o MD5 internamente, e use os mesmos valores em todos os aparelhos).

   - Defina **URL do servidor de sincronização** como `http://<server-ip>:17200`.

   - Execute **Autenticar**.

Se você usar o listener HTTPS, use `https://<server-ip>:7200` (`curl -k` apenas para testes com certificado autoassinado).

##### Sincronizando durante a leitura

Depois que qualquer uma das opções acima estiver configurada, pressione **Confirmar** durante a leitura para abrir o menu do leitor e selecione **Sincronizar progresso**. Como alternativa, defina **Configurações -> Controles -> Menu de toque longo** como **KOSync** e segure Confirmar para iniciar a sincronização diretamente.

- Com **Modo de sincronização** definido como **Perguntar sempre**, escolha **Aplicar progresso remoto** para pular para o progresso remoto ou **Enviar progresso local** para enviar o progresso atual.
- Com **Modo de sincronização** definido como **Inteligente**, o FluiDez Reader resolve automaticamente casos simples: envia quando não existe progresso remoto, confirma e deixa ambos inalterados quando o progresso local e o remoto já estão sincronizados, envia quando o progresso local está mais adiantado ou aplica o remoto quando o progresso remoto está mais adiantado.

### 3.7 Tela de repouso

A configuração **Tela de repouso** controla o que é exibido quando o aparelho entra em repouso:

| Modo                         | Comportamento                                                                                                                                             |
| ---------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Escuro** (padrão)          | O logotipo do FluiDez Reader em fundo escuro.                                                                                                            |
| **Claro**                    | O logotipo do FluiDez Reader em fundo branco.                                                                                                            |
| **Personalizado**            | Uma imagem personalizada do cartão SD (veja abaixo). Usa **Escuro** se nenhuma imagem personalizada for encontrada.                                      |
| **Capa**                     | A capa do livro aberto no momento. Usa **Escuro** se nenhum livro estiver aberto.                                                                         |
| **Capa + personalizado**     | A capa do livro aberto no momento, mostrada apenas durante a leitura ativa. Usa o comportamento **Personalizado** quando não estiver lendo.              |
| **Sobreposição da página**   | Mantém a página atual do leitor visível e desenha um papel de parede de repouso sobre ela. Se não houver papel de parede, a página permanece visível durante a leitura. |
| **Minimal**                  | Uma tela de repouso compacta baseada no layout inicial Minimal.                                                                                           |
| **Minimal Stats**            | Uma tela de repouso compacta com estatísticas recentes de leitura, em aparelhos compatíveis; usa **Minimal** enquanto o registro estiver desligado.      |
| **Nenhum**                   | Uma tela em branco.                                                                                                                                       |

#### Configurações de capa

Ao usar **Capa** ou **Capa + personalizado**, duas configurações adicionais se aplicam:

- **Modo da capa da tela de repouso**: **Ajustar** (dimensiona para caber, com bordas brancas) ou **Recortar** (dimensiona e recorta para preencher a tela).
- **Filtro capa tela repouso**: **Nenhum** (tons de cinza), **Contraste** (preto e branco) ou **Invertido** (preto e branco invertido).

#### Imagens personalizadas

Para usar imagens de repouso personalizadas, defina o modo da tela de repouso como **Personalizado**, **Capa + personalizado** ou **Sobreposição da página** e coloque as imagens no cartão SD:

- **Pela interface web (mais fácil):** com a transferência de arquivos ativa, abra `http://fluidez.local/sleep` (ou o IP mostrado no aparelho), na aba **Tela de descanso** da página **Telas**. Escolha qualquer foto ou desenho, ajuste o enquadramento, o zoom, o brilho e o contraste vendo a prévia em 4 tons de cinza, e toque em **Salvar e usar no aparelho**. A imagem é convertida no navegador para o tamanho exato da tela, salva em `/sleep` e fixada; o modo da tela de repouso muda sozinho para **Personalizado**. Na mesma página você vê as imagens já salvas, escolhe outra ou apaga, e o botão **Sortear entre todas as imagens** tira a imagem fixada para o aparelho mostrar uma diferente a cada descanso.
- **Várias imagens:** crie um diretório `.sleep` na raiz do cartão SD e coloque qualquer número de imagens `.bmp` dentro dele. No modo **Personalizado**, fotos `.jpg` e `.png` também funcionam: na primeira vez que forem usadas, o aparelho converte cada uma para BMP e guarda o resultado em `/.crosspoint/sleep-converted`, então das próximas vezes elas abrem tão rápido quanto um BMP. No modo **Sobreposição da página**, imagens `.png` também são compatíveis. Uma imagem será selecionada aleatoriamente toda vez que o aparelho entrar em repouso. (Um diretório chamado `sleep` também é aceito como fallback.)
- **Imagem única:** coloque um arquivo chamado `sleep.bmp` no diretório raiz. No modo **Sobreposição da página**, `sleep.png` também é compatível. Esses arquivos são usados como fallback se nenhuma imagem válida for encontrada no diretório `.sleep`/`sleep`.

No modo **Sobreposição da página**, pixels brancos de BMP e pixels transparentes de PNG deixam a página atual do leitor aparecer; os demais pixels do papel de parede são desenhados sobre a página. Papéis de parede PNG são compatíveis apenas nesse modo.

> [!TIP]
> Para melhores resultados:
>
> - Use arquivos BMP sem compressão, de preferência BMP indexado de 4 ou 8 bits
> - X4: use resolução de 480x800 pixels para corresponder à resolução da tela do aparelho.
> - X3: use resolução de 528x792 pixels para corresponder à resolução da tela do aparelho.

> [!TIP]
> Você pode definir uma imagem como capa da tela de repouso diretamente pelo visualizador de imagens BMP na tela **[Explorar arquivos](#33-tela-explorar-arquivos)**.

---

### 3.8 Tela inicial

A alternância **Tela inicial personalizada** fica em **Configurações -> Sistema** e vem ativada por
padrão. Quando ligada, você pode substituir o logotipo padrão do FluiDez Reader mostrado durante
uma inicialização fria por imagens BMP armazenadas no cartão SD. Uma tela inicial personalizada
configurada também é mostrada depois de despertar pelo botão liga/desliga. Isso é separado da
tela de repouso.

O FluiDez Reader oferece três formas de escolher uma tela inicial personalizada:

- **Pela interface web (mais fácil):** abra `http://fluidez.local/sleep#boot`, na aba **Tela de inicialização** da página **Telas**. O editor é o mesmo da tela de descanso: escolha a imagem, ajuste e toque em **Salvar e usar no aparelho**. Ela vai para a pasta `/bootscreen`, fica fixada e a **Tela inicial personalizada** é ligada. **Sortear entre todas as imagens** tira a imagem fixada para o aparelho alternar entre as imagens da pasta.

- **Uma imagem fixa:** em **[Explorar arquivos](#33-tela-explorar-arquivos)**, abra uma
  imagem BMP de qualquer pasta, abra seu menu de contexto e escolha **Tela inicial**.
  Essa imagem selecionada sempre tem prioridade sobre uma pasta de tela inicial.
  Para parar de usá-la, abra a mesma imagem e escolha **Limpar tela**.

- **Imagens rotativas:** crie uma pasta `/.bootscreen` na raiz do cartão SD
  e coloque arquivos BMP diretamente dentro dela. O FluiDez Reader seleciona uma imagem
  aleatoriamente a cada inicialização e evita imagens usadas recentemente quando possível. Uma
  pasta `/bootscreen` também é compatível. Nomes de pasta não diferenciam maiúsculas de minúsculas; se
  ambas existirem, `/.bootscreen` tem prioridade.

O FluiDez Reader armazena o conteúdo da pasta em cache para acelerar a inicialização. Se você adicionar arquivos BMP
diretamente a uma pasta já usada enquanto o aparelho está desligado, os novos arquivos talvez
não sejam selecionados até o índice ser reconstruído. Envie-os pelo gerenciador de arquivos web de **Transferência de arquivos** ou pela **Transferência por proximidade** para invalidar o
índice automaticamente.

A ordem de seleção e fallback é:

1. Um BMP selecionado com **Tela inicial**.
2. Um BMP utilizável escolhido na pasta ativa de tela inicial no nível raiz.
3. O logotipo padrão do FluiDez Reader.

Em uma inicialização fria, se uma imagem selecionada ou a pasta ativa estiver ausente, ilegível,
vazia ou sem BMP utilizável, o FluiDez Reader usa a próxima opção na ordem acima. Se os dois nomes
de pasta existirem, `/.bootscreen` mascara `/bootscreen` mesmo quando a pasta oculta está vazia ou inutilizável; remova ou renomeie-a para usar
`/bootscreen` em vez dela.

Ao despertar pelo botão liga/desliga, um BMP selecionado ou uma pasta de tela inicial existente (mesmo
vazia ou inutilizável) executa o fluxo da tela inicial e depois cai para
o logotipo padrão se nenhuma imagem puder ser mostrada. Se o arquivo selecionado foi removido
e não existe pasta de tela inicial, o despertar não mostra splash e mantém a tela de repouso atual
visível. Sem imagem selecionada nem pasta de tela inicial, o despertar pelo
botão liga/desliga também não mostra splash.

> [!TIP]
> Use um BMP sem compressão na resolução da tela do seu aparelho para o melhor resultado: 480x800 pixels no X4 ou 528x792 pixels no X3. Imagens com outras dimensões são centralizadas e reduzidas conforme necessário.

---

### 3.9 Fontes personalizadas (cartão SD)

O FluiDez Reader permite carregar fontes adicionais do cartão SD, indo além das famílias integradas Lexend Deca e Bitter. Fontes personalizadas podem incluir cobertura Unicode estendida, habilitando CJK (chinês, japonês, coreano) e outros sistemas de escrita.

Há três formas de instalar fontes:

1. **Baixar pelo aparelho (recomendado):** vá para **Configurações -> Leitor -> Opções de fonte -> Baixar fontes**, navegue pelas famílias disponíveis e selecione uma para baixar via Wi-Fi. O catálogo é hospedado pelo FluiDez e traz 29 famílias, incluindo Gelasio, EB Garamond, Crimson Pro, Jost e Arimo.
2. **Cópia manual para o cartão SD:** copie arquivos `.cpfont` (por exemplo, do [Inky](https://inky.crossink.dev/#downloads) ou do [repositório crossink-fonts](https://github.com/uxjulia/crossink-fonts/tree/main/cpfonts)) para `/.fonts/` (preferido) ou `/fonts/` no cartão SD.
3. **Enviar pela interface web:** enquanto estiver no modo **Transferência de arquivos**, abra a interface web em um navegador e acesse a aba **Fontes** para enviar arquivos `.cpfont`. Em aparelhos compatíveis (ESP32-S3), arquivos `.ttf` também podem ser enviados.

Depois de instaladas, fontes personalizadas aparecem em **Configurações -> Leitor -> Opções de fonte -> Família da fonte** junto das fontes integradas.

Veja [Fontes no cartão SD](./sd-card-fonts.md) para detalhes completos de instalação e estrutura de pastas do cartão SD.

---

## 4. Modo de leitura

Depois que você abre um livro, o layout dos botões muda para facilitar a leitura.

### Virada de página

| Ação                  | Botões                              |
| --------------------- | ----------------------------------- |
| **Página anterior**   | Pressione **Esquerda** _ou_ **Cima** |
| **Próxima página**    | Pressione **Direita** _ou_ **Baixo** |

Altere as ações de cada botão lateral em **Configurações > Controles > Botões laterais**.

Se a **Ação do toque curto** estiver definida como "Próxima página", você também pode virar para a próxima página pressionando rapidamente o botão liga/desliga.

### Navegação por capítulos

- **Próximo capítulo:** pressione e **segure** o botão **Direita** (ou **Baixo**) brevemente e solte.
- **Capítulo anterior:** pressione e **segure** o botão **Esquerda** (ou **Cima**) brevemente e solte.

Essas são as ações padrão de pular capítulo. Altere os toques longos dos botões frontais em **Configurações > Controles > Botões frontais**, ou atribua o toque longo de cada botão lateral separadamente em **Configurações > Controles > Botões laterais**.

### Virada automática de página

A Virada automática de página avança páginas automaticamente em um intervalo definido, útil para leitura sem as mãos. Esse recurso pode ser ativado e configurado pelo **[Menu do leitor](#5-menu-do-leitor)** durante a leitura de um EPUB.

### Virar página por inclinação (X3 e Sticky)

No **Xteink X3** e no **Sticky**, o giroscópio pode ser usado para virar páginas inclinando o aparelho. Esse recurso e sua direção esquerda-direita ou frente-trás ficam disponíveis em **Configurações -> Controles**.

### Controles de toque no leitor

Em aparelhos compatíveis com tela sensível ao toque, as viradas de página por toque vêm ativadas por
padrão. **Próxima página** e **Página anterior**, em **Configurações > Controles > Toques e gestos**, são
configurados de forma independente e ambos usam **Toque e deslize** por padrão:

| Opção                | Toques                                   | Deslizes    |
| -------------------- | ---------------------------------------- | ----------- |
| **Toque e deslize**  | Ativado                                  | Ativado     |
| **Somente toque**    | Ativado                                  | Desativado  |
| **Somente deslize**  | Desativado                               | Ativado     |
| **Toque invertido**  | Ativado, com zonas de toque invertidas   | Desativado  |
| **Desativado**       | Desativado                               | Desativado  |

Deslize para a esquerda para a próxima página quando **Próxima página** permite deslizes, ou deslize para a direita
para a página anterior quando **Página anterior** permite deslizes. Em EPUBs da direita para a esquerda,
as zonas normais de toque e as direções dos deslizes horizontais seguem o sentido de leitura do livro. Em livros
da esquerda para a direita, quando ambas as direções permitem toques, as zonas normais são o terço esquerdo para a página anterior e os
dois terços direitos para a próxima página. Se qualquer uma dessas configurações for **Toque invertido**, as zonas compartilhadas passam a ser os dois terços esquerdos para a próxima página e o
terço direito para a página anterior. Se apenas uma direção permite toques, toques
em toda a página viram nessa direção. As faixas de gesto superior e inferior são
reservadas para gestos verticais, então toques nessas faixas não viram páginas.

Para **leitores EPUB**, os gestos verticais dependem do aparelho:

- No **Sticky**, deslize para cima para abrir o menu do leitor. Deslize para baixo para abrir o
  painel de detalhes do leitor/luz frontal; use o cabeçalho desse painel para voltar ao Início.
- No **X4 Pro**, deslize para cima para abrir o menu do leitor e deslize para baixo para abrir o
  painel de luz frontal, que também mostra as páginas do capítulo e o progresso do livro durante a
  leitura. A tecla capacitiva Início volta ao Início com toque curto e
  abre o menu do leitor com toque longo por padrão. Configure essas ações, ou
  desative a tecla durante a leitura, em **Configurações > Controles > Botão Início**.
- Em outros aparelhos com tela sensível ao toque, deslize para baixo para abrir o menu do leitor e deslize para cima
  para voltar ao Início.

Leitores **XTC** e **TXT** usam roteamento vertical mais estreito e específico do formato. Por
exemplo, no Sticky TXT há o painel de detalhes do leitor/luz frontal ao deslizar para baixo, mas não há
ação de deslizar para o menu nem deslizar para Início; no X4 Pro, XTC mantém o menu do leitor por deslize para cima
enquanto seu painel de luz frontal por deslize para baixo funciona apenas a partir da borda superior, e TXT não tem
ação de deslizar para o menu. Esses gestos verticais são separados das configurações de virada de página acima.

Escolha **Desativado** em **Próxima página** ou **Página anterior** para interromper viradas de página por toque
nessa direção sem desativar os gestos verticais de menu do leitor ou luz frontal da tela sensível ao toque. **Desativar tela sensível ao toque**, em **Configurações > Leitor**,
impede entrada por toque na tela enquanto um livro está aberto, mas mantém o toque disponível nos
menus do leitor. Para os diferentes gestos de seleção por toque usados na [consulta de dicionário](./dictionary.md#looking-up-a-word) e em [recortes](./reader-features.md#clippings-and-highlights), veja esses guias de recursos.

Em aparelhos com suporte multitoque, você também pode atribuir ações a deslizes com dois dedos
em **Configurações > Controles > Toques e gestos > Deslizar com dois dedos**. Defina uma
ação para **Deslizar para cima**, **Deslizar para baixo**, **Deslizar para a esquerda** ou **Deslizar para a direita**, então
mova dois dedos juntos nessa direção durante a leitura. As ações disponíveis
são **Não definido**, **Aumentar brilho**, **Diminuir brilho**, **Aumentar tom quente**,
**Diminuir tom quente**, **Próximo capítulo**, **Capítulo anterior**, **Aumentar fonte**
e **Diminuir fonte**. Opções de brilho e tom quente aparecem
apenas quando o hardware oferece suporte; opções de capítulo se aplicam a EPUBs, e opções de fonte
se aplicam a livros EPUB e TXT. Cada direção começa como **Não definido**,
e cada ação só pode ser atribuída a uma direção; escolhê-la de novo move
para a nova direção. Em livros XTC baseados em imagem, ações de capítulo e tamanho da fonte
são consumidas, mas não conseguem alterar as páginas pré-renderizadas. Veja [Ações de deslizar com dois dedos](./controls.md#two-finger-swipe-actions) para a lista completa e
limitações específicas de cada leitor.

Em aparelhos multitoque compatíveis, ative **Tam. fonte por pinça** no mesmo
menu e afaste dois dedos para aumentar a fonte ou aproxime-os para diminuí-la.
Cada pinça concluída altera um passo de tamanho de fonte disponível. O redimensionamento por pinça
funciona em leitores EPUB e TXT; páginas XTC são pré-renderizadas e não podem ser
redimensionadas. A entrada por pinça também exige que a entrada por toque do leitor permaneça ativada.

### Controles deslizantes

Em controles deslizantes com incrementos de cinco unidades no Menu do leitor e nas Configurações, tocar
na trilha do controle arredonda o valor selecionado para o múltiplo de cinco mais próximo.

### Navegação por notas de rodapé

Ao ler um EPUB que contém notas de rodapé, toque em uma referência ou escolha **Notas de rodapé** na aba **Mais** do menu do leitor (ou em um atalho atribuído). Se houver uma nota, ela abre direto. Com várias referências visíveis, o FluiDez Reader as destaca na página: use os botões de direção para escolher uma e **Confirmar** para abri-la, ou toque na referência. Links sem destino visível usam uma lista. Pressione **Voltar** para retornar à posição original de leitura.

Depois de seguir um link interno até um capítulo inteiro, entrar em repouso ou fechar o livro reabre na última página lida. Voltar guarda as três origens de link mais recentes, mesmo depois de reabrir o livro e da sincronização KOReader. Se você fechar uma pré-visualização temporária de nota, o livro retoma na página que abriu a pré-visualização, com as origens de link anteriores ainda disponíveis em Voltar.

### Navegação do sistema

- **Voltar ao Início:** pressione o botão **Voltar** para fechar o livro e voltar à tela **[Início](#31-tela-início)**.
- **Voltar para Explorar arquivos:** mantenha o botão **Voltar** pressionado para fechar o livro e voltar à tela **[Explorar arquivos](#33-tela-explorar-arquivos)**.
- **Menu do leitor:** pressione **Confirmar** para abrir o **[Menu do leitor](#5-menu-do-leitor)**, que inclui navegação por capítulos, opções de leitura e mais.

### Idiomas compatíveis

O FluiDez Reader renderiza texto usando os seguintes blocos de caracteres Unicode, habilitando suporte a uma ampla variedade de idiomas:

- **Escrita latina (básica, suplemento, estendida A/B):** cobre inglês, alemão, francês, espanhol, português, italiano, holandês, sueco, norueguês, dinamarquês, finlandês, polonês, tcheco, húngaro, romeno, eslovaco, esloveno, turco, catalão e outros.
- **Escrita cirílica (padrão e estendida):** cobre russo, ucraniano, bielorrusso, búlgaro, sérvio, macedônio, cazaque, quirguiz, mongol e outros.
- **Vietnamita:** compatível via cobertura estendida de glifos latinos nas fontes integradas do leitor.

O que não é compatível com as fontes integradas do leitor: chinês, japonês, coreano, árabe, grego, hebraico e farsi. Porém, **CJK, hebraico, grego e outros sistemas de escrita estendidos podem ser habilitados instalando fontes personalizadas no cartão SD** — veja [Fontes personalizadas (cartão SD)](#39-fontes-personalizadas-cartão-sd).

---

## 5. Menu do leitor

Pressione **Confirmar** durante a leitura para abrir o Menu do leitor. A partir dele, você pode acessar utilitários de leitura e opções de navegação sem sair do livro.

Livros EPUB usam as mesmas cinco abas com ícones em aparelhos com toque e com botões:

| Aba (ícone)                | Principais opções                                                                                                                                                         |
| -------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Fonte** (letras)         | Fonte do leitor, fonte do dicionário, espaçamento entre linhas/palavras, suavização do texto, Leitura focada, Pontos guia                                                  |
| **Layout** (linhas)        | Margens, orientação, alinhamento, imagens, hifenização, números de página da editora, espaçamento de parágrafos e estilo da editora                                        |
| **Mais** (três pontos)     | Consulta de palavras, Escolher capítulo, Ir para %, Ir para página estável quando disponível, Virada automática, Notas de rodapé; Estatísticas de leitura em aparelhos com botões quando o registro está ativado |
| **Marcadores** (marcador)  | Adicionar/remover e ver marcadores, criar/ver recortes, captura de tela, QR da posição; Sincronizar progresso, sincronização de posição por proximidade e envio de livro por proximidade em aparelhos com botões |
| **Configurações** (engrenagem) | Barras de status, Controles, Dicionário do livro, modo de renderização EPUB, método de indexação, status de concluído, Registrar estatísticas de leitura, redefinições de cache/estatísticas, Redefinir configurações de leitura do livro |

Algumas ações aparecem só quando o livro ou o aparelho oferece suporte. Em telas sensíveis ao toque, toque em uma aba e depois em uma opção. Em aparelhos com botões, **Esquerda/Direita** trocam de aba, **Cima/Baixo** selecionam linhas e **Confirmar** abre a opção selecionada. Mudanças de fonte, espaçamento e margem têm pré-visualização ao vivo. As configurações globais usam a mesma navegação por abas e linhas.

**Redefinir configurações de leitura do livro** restaura os padrões globais herdados do EPUB atual sem apagar progresso, marcadores, recortes ou estatísticas. **Excluir cache do livro** reconstrói os dados em cache do livro e mantém essas escolhas de leitura.

Livros TXT e XTC têm menus próprios, com menos opções; o layout de cinco abas acima é para EPUBs. Para o registro de estatísticas, use **Registrar estatísticas de leitura** na aba **Configurações** do EPUB ou no menu do leitor XTC. Para recursos específicos de cada formato, veja [Recursos do leitor](./reader-features.md).

Pressione **Voltar** a qualquer momento para fechar o menu e voltar à página atual.

### 5.1 Escolha de capítulo

Em livros EPUB, abra a aba **Mais** do menu do leitor e selecione **Escolher capítulo**.

1. Use **Esquerda** (ou **Cima**) ou **Direita** (ou **Baixo**) para destacar o capítulo desejado.
2. Pressione **Confirmar** para pular para esse capítulo.
3. _Como alternativa, pressione **Voltar** para cancelar e retornar à página atual._

---

### 5.2 Marcadores

Marcadores podem ser criados para salvar e restaurar rapidamente sua posição em um livro.

Para criar um marcador, segure **Confirmar** por 1 segundo dentro de um livro. Um pop-up aparecerá informando que o marcador foi criado. A mensagem do pop-up desaparecerá automaticamente em alguns segundos.

Para abrir os marcadores, pressione **Confirmar** dentro de um livro. Depois navegue até o menu **Marcadores**. Marcadores podem ser abertos navegando até eles e pressionando **Confirmar**, o que redirecionará você para aquele ponto do livro. Você pode excluir marcadores segurando **Confirmar** por 1 segundo e depois pressionando **Confirmar** novamente para confirmar a exclusão, ou **Voltar** para cancelar.

Marcadores são armazenados como arquivos `.bin` por livro na pasta `.crosspoint/bookmarks`.

### 5.3 Dicionário

A consulta de dicionário oferece seleção de palavras, histórico recente por livro, consultas encadeadas a partir de definições e substituições de dicionário por livro. Veja o [guia do dicionário](./dictionary.md) para instruções de instalação e preparação.

## 6. Limitações atuais e roteiro

Observe que este firmware ainda está em desenvolvimento ativo. Os seguintes recursos **ainda não são compatíveis**, mas estão planejados para atualizações futuras:

- **Imagens de capa:** imagens de capa grandes embutidas em EPUB podem levar vários
  segundos para converter para a tela de repouso e a miniatura da tela inicial. Use a
  [otimização de EPUB](./webserver.md#epub-optimization) integrada antes do envio
  se um livro estiver lento ou for sensível à memória.
- **Formatos de imagem não compatíveis:** a maioria das imagens JPG e PNG em EPUBs é renderizada corretamente. GIFs e JPEGs progressivos não são compatíveis e cairão para um espaço reservado `[Image]`.

---

## 7. Solução de problemas e saída de bootloop

Se ocorrer um problema ou travamento durante o uso do FluiDez Reader, os logs abaixo ajudam a encontrar a causa.

**Relatórios de falha no cartão SD:** depois de uma falha, o FluiDez Reader salva automaticamente um relatório de falha no cartão SD (sem precisar de conexão USB). Verifique a raiz do cartão SD para encontrar um arquivo de log de falha.

**Logs do monitor serial:** para depuração mais detalhada, conecte o aparelho a um computador e execute o script personalizado de monitoramento de depuração (requer Python 3 com `pyserial`, `colorama` e `matplotlib`; instale com `pip3 install pyserial colorama matplotlib`):

```
python3 scripts/debugging_monitor.py
```

O script detecta automaticamente a porta serial. Você também pode especificar uma explicitamente:

```
python3 scripts/debugging_monitor.py /dev/ttyACM0        # Linux
python3 scripts/debugging_monitor.py /dev/tty.usbmodem1  # macOS
python3 scripts/debugging_monitor.py COM7                # Windows
```

**Recursos:**

- Saída de log colorida por categoria (erros, memória, tela, análise de EPUB etc.)
- Gráfico ao vivo de uso de memória (RAM livre, RAM total, maior alocação contígua) atualizado a cada segundo
- Prompt de comando interativo — digite um comando e pressione Enter para enviá-lo ao aparelho
- Captura de tela — salva a tela atual em `screenshot.bmp` quando acionada pelo aparelho

**Opções:**

| Opção                | Descrição                                                   |
| -------------------- | ----------------------------------------------------------- |
| `--baud RATE`        | Taxa de transmissão (padrão: 115200)                        |
| `--filter KEYWORD`   | Mostra apenas linhas que contêm a palavra-chave (sem diferenciar maiúsculas/minúsculas) |
| `--suppress KEYWORD` | Oculta linhas que contêm a palavra-chave (sem diferenciar maiúsculas/minúsculas)        |

**Exemplos:**

```
# Show only memory-related log lines
python3 scripts/debugging_monitor.py --filter MEM

# Hide noisy SD card log lines
python3 scripts/debugging_monitor.py --suppress "[SD]"
```

Pressione **Ctrl-C** ou feche a janela do gráfico para sair.

Se o aparelho ficar preso em um bootloop, pressione e solte o botão Reset. Depois, pressione e mantenha pressionados o botão Voltar configurado e o botão liga/desliga para iniciar na tela Início.

Podem ocorrer problemas com cache ou configuração corrompidos. Nesse caso, exclua o diretório `.crosspoint` do cartão SD (ou considere excluir apenas `settings.json`, `state.json` ou os diretórios de cache `epub_*` na pasta `.crosspoint/`).
