# Novidades

O que muda para quem usa o FluiDez Reader em cada versão, da mais recente para a mais antiga. A seção de cada versão abre a página dela em [Releases](https://github.com/micheljatuba/FluiDez-Reader/releases). O histórico técnico completo, em inglês, está no [CHANGELOG](CHANGELOG.md).

## [Próxima versão]

## [v1.6-fluidez11] - 2026-09-30

- **Plugin próprio para o Calibre:** envie livros do Calibre para o leitor pelo Wi-Fi com o plugin FluiDez Reader, anexado às versões como `fluidez-reader-calibre-plugin.zip`. Ele substitui o plugin CrossPoint Reader, que deve ser removido do Calibre. No Windows, a busca automática não desiste antes da hora, como acontecia com o plugin CrossPoint Reader. [Como instalar](https://github.com/micheljatuba/FluiDez-Reader/blob/main/calibre-plugin/README.md).
- **Novo endereço na rede:** o portal do leitor passa a ser `http://fluidez.local/` (antes `crosspoint.local`); atualize seus favoritos. Em *Criar hotspot*, a rede se chama `FluiDez-Reader`, e no roteador o leitor aparece como `FluiDez-Reader-` seguido do código do aparelho.
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
