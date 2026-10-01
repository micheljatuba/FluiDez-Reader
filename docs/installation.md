---
title: Instalação
nav_order: 2
---

# Instalação

O firmware do FluiDez Reader é publicado na
[página de versões do FluiDez Reader](https://github.com/micheljatuba/FluiDez-Reader/releases).
Cada versão tem um arquivo de firmware para cada leitor:

| Leitor              | Arquivo do firmware                  |
| ------------------- | ------------------------------------ |
| Xteink X4 Pro       | `firmware-x4-pro-v<version>.bin`     |
| Xteink X4 Classic   | `firmware-x4-classic-v<version>.bin` |
| Xteink X3 / X4      | `firmware-x3-x4-v<version>.bin`      |
| Seeed Studio Sticky | `firmware-sticky-v<version>.bin`     |

O FluiDez Reader é testado apenas no Xteink X4 Pro. Os outros arquivos são
gerados a partir do mesmo código e passam nas verificações automatizadas, mas
não foram testados em um aparelho.

> **Use por sua conta e risco.** Você instala e atualiza o FluiDez Reader por
> sua conta e risco. A MJ Cloud Tecnologia não se responsabiliza por danos ao
> leitor, perda de dados ou qualquer outro problema decorrente da instalação,
> da atualização ou do uso do firmware. Carregue a bateria antes e mantenha o
> leitor ligado até a atualização terminar.

## Atualizações pelo aparelho

Depois que o FluiDez Reader estiver instalado, `Configurações > Sistema > Verificar atualizações`
baixa a versão mais nova do FluiDez Reader para o seu leitor via Wi-Fi. Uma
versão só é oferecida quando é mais nova que a instalada, por exemplo
`v1.6-fluidez10` sobre `1.6-fluidez9`. Antes de instalar, o leitor avisa que as
atualizações são feitas por sua conta e risco.

## Atualização de firmware do cartão SD

Use este método quando o leitor já roda o FluiDez Reader ou o firmware CrossInk
no qual ele se baseia. Ele também funciona em leitores com transferência de
dados por USB desativada.

1. Baixe o `firmware-*.bin` do seu leitor na
   [página de versões](https://github.com/micheljatuba/FluiDez-Reader/releases).
2. Copie o arquivo para o cartão SD. Qualquer pasta serve.
3. No leitor, abra `Configurações > Sistema > Atualização de firmware do cartão SD`, escolha o
   arquivo `.bin` e confirme.

## Unidade USB

No X4 Pro, escolha `Início > Transferência de arquivos > Unidade USB` para
expor o cartão SD ao computador. Ejete a unidade pelo computador antes de
desconectá-la; o leitor reinicia no Início quando a unidade é ejetada com
segurança ou o cabo é removido.

## Gravação por USB

Use a gravação por USB em um leitor que roda outro firmware ou para recuperar
um leitor que não inicia mais. Conecte o leitor ao computador com um cabo de
dados USB-C. Se a ferramenta de gravação não conseguir conectar, coloque o
leitor em modo de download conforme descrito pelo fabricante do aparelho e
tente novamente.

### Pelo código-fonte com PlatformIO

Com o [ambiente de desenvolvimento](./development/getting-started.md)
instalado, compile e grave o ambiente do seu leitor: `x4-pro`, `x4-classic`,
`default` (X3/X4) ou `sticky`.

```sh
pio run -e x4-pro --target upload
```

O PlatformIO grava o bootloader, a tabela de partições e o firmware.

### Arquivo de versão com esptool

Instale o `esptool`:

```sh
pip3 install esptool
```

Encontre a porta serial do leitor:

```sh
# Linux
dmesg | grep tty

# macOS
ls /dev/cu.*
```

No Windows, a porta aparece como `COM<n>` no Gerenciador de Dispositivos, em
**Portas (COM e LPT)**.

Limpe o slot de atualização salvo para o leitor iniciar a imagem que você vai
gravar e, em seguida, grave o firmware:

```sh
python3 -m esptool --port /dev/ttyACM0 erase_region 0xe000 0x2000
python3 -m esptool --port /dev/ttyACM0 --baud 921600 write_flash 0x10000 /path/to/firmware.bin
```

Substitua a porta e o caminho do firmware pelos seus valores reais. Isso grava
apenas a aplicação, então use em um leitor que já roda o FluiDez Reader. Para
outros firmwares, use o PlatformIO.
