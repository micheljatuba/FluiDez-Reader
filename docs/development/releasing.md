---
title: Publicar uma versão
parent: Development
nav_order: 6
---

# Publicar uma versão

O leitor procura atualizações na versão mais recente (*Latest*) publicada em [Releases](https://github.com/micheljatuba/FluiDez-Reader/releases). Quem publica é o workflow *Release* (`.github/workflows/release.yml`), a partir de uma tag criada em `main`.

## Passos

1. Escolha o número da versão, por exemplo `1.6-fluidez10`, seguindo a seção "Versão depois da sincronização" de [Sincronização com o CrossInk](./upstream-sync.md). O workflow recusa números fora do formato `<versão base>-fluidez<número>`, porque o leitor não os reconheceria.
2. Atualize `[fluidez] version` no `platformio.ini`. No [CHANGELOG](../../CHANGELOG.md), mova as entradas de `[Unreleased]` para uma seção nova, como `## [v1.6-fluidez10] - AAAA-MM-DD`. Faça o mesmo em [NOVIDADES](../../NOVIDADES.md) com as entradas de `[Próxima versão]`: essa seção, em português e escrita para quem usa o leitor, abre a página da release. Para ver o texto que vai para a página, rode `python scripts/release_notes.py 1.6-fluidez10`. O CI falha se o NOVIDADES não tiver a seção da versão do `platformio.ini`.
3. Faça o commit em `main`, envie com `git push origin main` e espere o CI passar.
4. Crie a tag anotada e envie apenas ela:

   ```sh
   git tag -a v1.6-fluidez10 -m "FluiDez Reader v1.6-fluidez10"
   git push origin v1.6-fluidez10
   ```

   Não use `git push --tags` (o motivo está em [Sincronização com o CrossInk](./upstream-sync.md)).

5. O workflow *Release* confere se o commit está em `main` e se o NOVIDADES tem a seção da versão, compila os quatro firmwares (X4 Pro, X4 Classic, X3/X4 e Sticky), gera o plugin do Calibre (`scripts/build_calibre_plugin.py`) e publica a versão como *Latest*, com as novidades, o aviso de risco e a tabela de arquivos. A partir daí, os leitores a encontram em *Verificar atualizações*.

Também é possível rodar o workflow manualmente em `main` (*Actions > Release > Run workflow*), informando a versão sem o `v`, por exemplo `1.6-fluidez10`. Nesse caso, a tag é criada na publicação.

## Conferir a publicação

A versão nova deve aparecer como *Latest*, começar pelas novidades e ter os quatro arquivos `.bin` e o `fluidez-reader-calibre-plugin.zip`:

```sh
gh release view --repo micheljatuba/FluiDez-Reader
```

O leitor só instala arquivos `firmware-<leitor>*.bin`; o ZIP do plugin não interfere na atualização. Não publique uma release só com o plugin: ela viraria a *Latest* e os leitores não achariam firmware nela.

## Versão com problema

Se uma versão publicada tiver um bug grave:

- registre-o em `### Known issues`, na seção da versão no CHANGELOG, e como **Problema conhecido** no início da seção da versão no NOVIDADES;
- avise no início das notas da release no GitHub. Se o bug impedir a atualização pelo leitor, avise também no README e no texto padrão das notas (em `release.yml`), para que as versões seguintes indiquem o cartão SD;
- ao editar as notas de uma versão antiga, use `gh release edit <tag> --latest=false`, para que ela não se torne a *Latest*: o leitor instala sempre a *Latest*.
- para retirar a versão, publique antes a correção e só depois apague a release e a tag com `gh release delete <tag> --cleanup-tag --repo micheljatuba/FluiDez-Reader`. Tire também as menções a ela do README, das notas das outras releases e da documentação.
