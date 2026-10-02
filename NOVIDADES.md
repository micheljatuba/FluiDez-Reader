# Novidades

O que muda para quem usa o FluiDez Reader em cada versão, da mais recente para a mais antiga. A seção de cada versão abre a página dela em [Releases](https://github.com/micheljatuba/FluiDez-Reader/releases). O histórico técnico completo, em inglês, está no [CHANGELOG](CHANGELOG.md).

## [Próxima versão]

## [v1.6-fluidez13] - 2026-10-01

- **Status do Calibre:** a seção *Status* da tela *Calibre sem fio* não fica mais em branco. Ela mostra "Aguardando o Calibre..." até o Calibre encontrar o leitor e depois "Conectado ao Calibre", com o IP do computador. Durante um envio aparecem o livro que está chegando e o progresso; em seguida, o livro recebido ou a falha no envio e quantos livros chegaram.
- **Plugin do Calibre 1.1.0:** ao sair da tela *Calibre sem fio*, o Calibre mostra o leitor como desconectado em cerca de 15 segundos e conecta de novo sozinho quando a tela é reaberta. Instale o plugin desta versão junto com o firmware.
- **Capítulo que não abre:** se um capítulo de um livro EPUB não carregar por um erro inesperado, o leitor mostra "Falha ao indexar - livro inválido" em vez de deixar a tela como estava.
- **Troca de página com suavização de texto:** se você virar a página enquanto o texto ainda aparece em negrito, antes de clarear, o leitor passa direto para a próxima página, sem terminar a suavização da página que você está deixando. A página em que você parar recebe a suavização normalmente.
- **Texto desenhado com menos processamento:** as letras dos livros e dos menus são desenhadas com menos trabalho do processador.

## [v1.6-fluidez12] - 2026-09-30

- **Autenticação KOReader mais estável:** se o servidor ou um proxy responder com uma página grande no lugar dos dados de login, o leitor avisa que o servidor de sincronização respondeu com dados inválidos e não trava por falta de memória.
- **Mais memória ao sair de um livro:** ao sair de um livro EPUB ou TXT, o leitor libera a memória temporária das fontes para as outras telas, como as capas do *Início*.
- **Relatório de falha mais completo:** o arquivo `crash_report.txt` identifica a compilação exata do firmware e, no X4 Pro, no X4 Classic e no Sticky, traz os dados dos dois núcleos do processador e o nome da tarefa que estava rodando. Isso ajuda a encontrar a causa de um travamento.
- **Identificação na internet:** ao baixar fontes, abrir catálogos OPDS e verificar atualizações, o leitor se identifica para os servidores como FluiDez Reader.
- **Guias em português:** o [guia de instalação](https://github.com/micheljatuba/FluiDez-Reader/blob/main/docs/installation.md) e o [guia do usuário](https://github.com/micheljatuba/FluiDez-Reader/blob/main/docs/user-guide.md) estão em português, com os mesmos nomes de menu do leitor.

## [v1.6-fluidez11] - 2026-09-30

- **Plugin FluiDez Reader para o Calibre:** envie livros do Calibre para o leitor pelo Wi-Fi. O plugin encontra o leitor automaticamente na rede e vem anexado às versões como `fluidez-reader-calibre-plugin.zip`. [Como instalar](https://github.com/micheljatuba/FluiDez-Reader/blob/main/calibre-plugin/README.md).
- **Nomes na rede:** o portal do leitor fica em `http://fluidez.local/`. Em *Criar hotspot*, a rede se chama `FluiDez-Reader`, e no roteador o leitor aparece como `FluiDez-Reader-` seguido do código do aparelho.
- A tela *Calibre sem fio* indica o plugin FluiDez Reader.

## [v1.6-fluidez10] - 2026-09-30

- **Interface toda em português:** os 132 textos que ainda apareciam em inglês foram traduzidos, entre eles os da transferência por proximidade, do dicionário, dos toques e gestos, da luz frontal, da conta e da sincronização do KOReader e das configurações de data.
- **Meses em português** na data do cabeçalho, nas estatísticas de leitura e no tema Painel, por exemplo "30 set 2026".
- **Datas das estatísticas na ordem do formato escolhido:** para ver "29 set" em vez de "set 29", escolha "31 dez 2026" em *Configurações > Sistema > Formato da data*.

## [v1.6-fluidez9] - 2026-09-29

Primeira versão publicada do FluiDez Reader, baseada no CrossInk depois da v1.5.1.

- **Identidade própria:** símbolo e logotipo nas telas de inicialização e de repouso, no rodapé das Configurações, no nome do aparelho e no portal web.
- **Atualizações por este repositório:** *Verificar atualizações* instala as versões publicadas aqui. Antes de instalar, a tela avisa que a atualização é por sua conta e risco. O firmware inclui português e inglês.
- **Livros fixados:** fixe até seis livros no topo de *Livros recentes*.
- **Menu do livro no Início:** segure a capa de um livro para *Fixar no topo* ou *Desafixar*, *Marcar como concluído* ou *Remover dos Livros recentes*.
- **Tema Lyra Grade:** seis livros no *Início*, com barra de progresso e uma fita nos fixados. O Lyra Carousel passa a mostrar até cinco livros.
- **Painel:** rótulos das estatísticas mais curtos e maiores, que não invadem a capa, o título nem o rodapé.
- **Cinco famílias de fontes extras** para o cartão SD: Gelasio, EB Garamond, Crimson Pro, Jost e Arimo.
- **Bateria e resposta:** mais telas respeitam o *Tempo para repousar*, as telas de espera não deixam a CPU em velocidade máxima e o toque responde mais rápido depois que a tela fica parada.
- **Confiabilidade:** as estatísticas de cada livro sobrevivem a uma falha do cartão SD durante a gravação, o gerenciador de arquivos web aceita pastas com `%` no nome e renomear pelos metadados do EPUB escolhe o autor, e não o tradutor.
- **Correções:** *Verificar atualizações* e a *Autenticação KOReader* não travam ao conectar ao Wi-Fi no X4 Pro, no X4 Classic e no Sticky, e a tela de atualização concluída mostra por inteiro a instrução para ligar o leitor de novo.
