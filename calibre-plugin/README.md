# Plugin FluiDez Reader para Calibre

Este plugin faz o Calibre reconhecer o **FluiDez Reader** como um dispositivo sem fio e enviar arquivos EPUB diretamente para o leitor por WebSocket. Ele é mantido pela **MJ Cloud Tecnologia** e foi testado apenas no **Xteink X4 Pro**.

## Download e instalação

1. Baixe o ZIP mais recente em: <https://github.com/micheljatuba/FluiDez-Reader/releases/latest/download/fluidez-reader-calibre-plugin.zip>.
2. No Calibre, abra **Preferências > Plugins > Carregar plugin a partir de arquivo**.
3. Selecione o arquivo `fluidez-reader-calibre-plugin.zip` sem extrair.
4. Confirme o aviso de segurança do Calibre e reinicie o Calibre.

## Como conectar

1. No leitor, abra **Transferência de arquivos** e escolha:
   - **Calibre sem fio**, para usar a mesma rede Wi-Fi do computador; ou
   - **Criar hotspot**, e conecte o computador à rede `FluiDez-Reader` criada pelo leitor.
2. No Calibre, aguarde o dispositivo **FluiDez Reader** aparecer. O plugin procura o leitor automaticamente na rede e também tenta `fluidez.local` e o IP do hotspot, `192.168.4.1`.
3. Envie os livros com **Enviar para o dispositivo** ou pelo menu de contexto do Calibre. Mantenha a tela do leitor aberta durante o envio.

A procura automática também encontra leitores com versões anteriores do FluiDez Reader.

## Configurações principais

Abra **Preferências > Plugins**, expanda **Interface do dispositivo**, selecione **FluiDez Reader** e clique em **Configurar plugin**.

- **Host**: endereço de fallback quando a descoberta automática não encontrar o leitor. O padrão é `fluidez.local`.
- **Porta**: porta WebSocket de upload. O padrão do firmware é `81`.
- **Caminho de envio**: pasta base no leitor. O padrão é `/`.
- **Modelo de caminho de envio**: deixe em branco para usar o modelo padrão do Calibre; preencha para controlar subpastas e nomes de arquivos.
- **Tamanho do bloco**: tamanho dos blocos WebSocket. Valores acima de 2048 são limitados automaticamente para compatibilidade com o firmware.
- **Confiabilidade do envio**: número de tentativas, atraso entre tentativas, pausa entre livros e timeout do socket.
- **Ativar log de depuração**: mostra detalhes da descoberta e da transferência no campo **Log**, no fim da janela de configuração.
- **Buscar metadados de livros carregados manualmente**: baixa cada EPUB não reconhecido uma vez para tentar associá-lo à biblioteca. Normalmente não é necessário.
- **Enviar para a raiz**: ignora o modelo de envio e manda todos os livros para a pasta base.
- **Sobrescrever arquivo se ele já existir no dispositivo**: remove a cópia antiga antes de reenviar o mesmo arquivo.

## Otimizador de EPUB

O plugin pode otimizar EPUBs antes da transferência (opção **Otimizar EPUBs antes da transferência**), de forma semelhante ao otimizador da página web do leitor.

Quando ativado, cada EPUB pode passar por estas etapas antes do envio:

- redimensionar imagens para a tela do dispositivo;
- converter imagens para tons de cinza;
- recompactar imagens como JPEG, com qualidade configurável;
- recortar margens uniformes, se a opção estiver ativada;
- reescrever o contêiner EPUB para corrigir referências de imagens, capas SVG, metadados OPF/NCX e estilos defensivos;
- dividir capítulos/parágrafos grandes, remover fontes embutidas e limpar itens que podem estourar a memória do firmware.

O alvo de tela é detectado pelo `/api/status` do leitor. As opções do campo **Dispositivo alvo** são:

- **Detectar automaticamente**;
- **X4 / X4 Pro / X4 Classic / Sticky (480×800)**;
- **X3 (528×792)**.

Se a detecção falhar, o plugin usa o perfil X4 por padrão. Durante a transferência, uma janela mostra o progresso da otimização e um resumo final; se a otimização falhar para um livro, o arquivo original é enviado para não bloquear a transferência.

## Solução de problemas

- **O dispositivo não aparece**: confirme que o leitor está na tela **Calibre sem fio** (ou em **Criar hotspot**, com o computador conectado à rede `FluiDez-Reader`) e que computador e leitor estão na mesma rede. Depois, tente novamente.
- **`fluidez.local` não funciona na sua rede**: coloque no campo **Host** o IP mostrado na tela do leitor. No hotspot, o IP é `192.168.4.1`, que o plugin sempre tenta.
- **Transferência falha**: reduza o tamanho do bloco para 2048, aumente o timeout do socket e mantenha o leitor acordado durante o envio.
- **Dois plugins aparecem para o mesmo leitor**: em **Preferências > Plugins**, remova ou desative o outro plugin de dispositivo sem fio e deixe apenas o **FluiDez Reader** ativo.
- **Livro já existe no leitor**: ative **Sobrescrever arquivo se ele já existir no dispositivo** ou apague o arquivo antigo no leitor antes de reenviar.
- **Livros carregados fora do Calibre não aparecem como “no dispositivo”**: ative **Buscar metadados de livros carregados manualmente** apenas se a correspondência por nome não for suficiente.

## Risco e responsabilidade

Uso por sua conta e risco. A MJ Cloud Tecnologia não se responsabiliza por perda de arquivos, mau funcionamento, interrupções, danos ao dispositivo ou qualquer outro prejuízo decorrente do uso deste plugin ou do firmware.

## Créditos e licença

Este plugin é uma bifurcação do plugin **CrossPoint Reader**: <https://github.com/crosspoint-reader/calibre-plugins>. O otimizador de EPUB se baseia no trabalho original de [@zgredex](https://github.com/zgredex), portado para Python nesse projeto. O código original é distribuído sob a licença MIT. As modificações de marca, compatibilidade e empacotamento para o FluiDez Reader também são distribuídas sob a licença MIT; veja [LICENSE](LICENSE).
