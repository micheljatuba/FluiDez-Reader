---
title: Sincronização com o CrossInk
parent: Development
nav_order: 5
---

# Sincronização com o CrossInk

O histórico do FluiDez Reader é independente. Ele começa em um único commit de importação da base do [CrossInk](https://github.com/uxjulia/crossink) (desenvolvimento após a v1.5.1, commit `b0eb0aa6`), e os commits seguintes são do FluiDez. As mudanças do CrossPoint Reader chegam através do CrossInk, então o FluiDez sincroniza apenas com o CrossInk.

Para manter o histórico independente, as atualizações do CrossInk entram como **um commit compactado (squash)** por sincronização. Cada commit de sincronização registra a revisão do upstream no trailer `Upstream-Commit:`, assim como o commit de importação.

> **Nunca** envie (`push`) merges cujos pais incluam commits do CrossInk ou do CrossPoint, não use `git pull` do upstream e nunca rode `git push --tags`. Qualquer um desses passos traz de volta milhares de commits, tags e contribuidores do upstream para o repositório.

## Configuração (uma vez por clone)

```sh
git remote add crossink https://github.com/uxjulia/crossink.git
git remote set-url --push crossink DISABLED
git config remote.crossink.tagOpt --no-tags
```

Se o remoto já existir com outro nome (por exemplo `julia`), use esse nome nos comandos abaixo e aplique as mesmas duas últimas configurações a ele.

## Procedimento

Os comandos usam a sintaxe do Git Bash, Linux ou macOS.

1. Busque o upstream e escolha a revisão a incorporar. Normalmente é a branch `development` ou `release/<versão>` do CrossInk:

   ```sh
   git fetch crossink
   UPSTREAM=$(git rev-parse crossink/development)
   ```

   Para usar uma tag de versão sem copiá-la para as tags locais:

   ```sh
   git fetch --no-tags crossink refs/tags/v1.6.0
   UPSTREAM=$(git rev-parse FETCH_HEAD)
   ```

2. Crie enxertos locais e temporários. Cada commit com o trailer `Upstream-Commit:` (a importação e as sincronizações anteriores) passa a ter também a revisão do upstream como pai. Assim, o Git sabe o que o FluiDez já contém e usa a base comum correta no merge de 3 vias, mesmo quando você alterna entre `development` e `release/<versão>`:

   ```sh
   for c in $(git log --format=%H --grep='^Upstream-Commit: '); do
     git replace -f --graft "$c" $(git rev-parse "$c^@") \
       $(git log -1 --format='%(trailers:key=Upstream-Commit,valueonly)' "$c")
   done
   ```

3. Guarde uma cópia dos arquivos de IA, que ficam só no seu computador (`AGENTS.md`, `CLAUDE.md` e `.claude/`). O Git ignora esses arquivos e, se o upstream alterá-los, o merge os sobrescreve sem avisar. Depois, veja o que vai chegar e traga as mudanças sem criar um merge:

   ```sh
   mkdir -p ../fluidez-ia && cp -r AGENTS.md CLAUDE.md .claude ../fluidez-ia/
   git log --oneline HEAD.."$UPSTREAM"
   git diff --stat HEAD..."$UPSTREAM"
   git merge --squash "$UPSTREAM"
   ```

4. Resolva os conflitos (veja abaixo), compile e teste. Em seguida, restaure a cópia dos arquivos de IA, faça o commit registrando a revisão do upstream e remova os enxertos:

   ```sh
   cp -r ../fluidez-ia/. .
   git commit -m "chore: sync with CrossInk ${UPSTREAM:0:8}" -m "Upstream-Commit: $UPSTREAM"
   git replace -d $(git replace -l)
   ```

   Para desistir antes do commit, rode `git reset --hard` (descarta as mudanças trazidas), restaure a cópia com `cp -r ../fluidez-ia/. .` e remova os enxertos com o mesmo `git replace -d $(git replace -l)`.

5. Confira antes de enviar. O commit novo deve ter um único pai, não deve sobrar nenhum enxerto e o histórico deve continuar pequeno (dezenas de commits, não milhares):

   ```sh
   git rev-list --parents -n 1 HEAD
   git replace -l
   git rev-list --count HEAD
   git push origin main
   ```

### Alternativa sem enxerto

Quando `UPSTREAM` descende da última revisão sincronizada (por exemplo, sempre a branch `development`), também é possível aplicar a diferença do upstream como patch:

```sh
BASE=$(git log -1 --grep='^Upstream-Commit: ' --format='%(trailers:key=Upstream-Commit,valueonly)')
git diff --binary "$BASE" "$UPSTREAM" | git apply -3
git commit -m "chore: sync with CrossInk ${UPSTREAM:0:8}" -m "Upstream-Commit: $UPSTREAM"
```

O `git apply` falha por inteiro se o upstream alterar um arquivo que o FluiDez removeu. Nesse caso, exclua esses caminhos, por exemplo `git apply -3 --exclude='site/*'`.

### Correções pontuais

Para trazer só uma correção do CrossInk, sem sincronizar tudo, aplique o commit sem criar merge (`git cherry-pick -n <commit>` ou à mão) e registre a origem com o trailer `Ported-From:`:

```sh
git commit -m "fix: ..." -m "Ported-From: uxjulia/crossink@<commit>"
```

Não use `Upstream-Commit:` nesses commits. Esse trailer indica uma sincronização completa, e os enxertos do passo 2 fariam o Git considerar que todo o histórico anterior do upstream já está no FluiDez. Na próxima sincronização, o merge de 3 vias reconhece as correções já aplicadas.

## Conflitos comuns

Antes de resolver um conflito, leia o commit do upstream e o PR citado nele (por exemplo, `#2608`) para entender a intenção da mudança. Não mantenha automaticamente a versão do FluiDez nem descarte a mudança do upstream inteira: preserve ou adapte a intenção dela, a menos que já esteja implementada, cause uma regressão ou mude sem justificativa o comportamento do FluiDez. Ao rejeitar uma mudança, registre o motivo.

| Onde | O que fazer |
| --- | --- |
| `lib/I18n/translations/*.yaml` | Mantenha os textos da marca FluiDez (`STR_CROSSINK` = "FluiDez Reader", modo de renderização "FluiDez Default") e aceite as chaves novas do upstream. |
| `docs/` | Mantenha o nome FluiDez Reader e aproveite o conteúdo técnico novo. |
| `CHANGELOG.md` | Não copie o changelog do CrossInk. Adicione em `[Unreleased]` uma linha resumindo a sincronização, com link para o changelog do CrossInk. Em `NOVIDADES.md`, resuma em `[Próxima versão]`, em português, o que a sincronização muda para quem usa o leitor. |
| `platformio.ini` | Preserve a versão FluiDez em `[crossink] version`, os idiomas de `custom_i18n_builtin_langs` e os nomes USB `FluiDez_*`. |
| `src/network/OtaUpdater.cpp` | Mantenha as atualizações apontando para `micheljatuba/FluiDez-Reader`. |
| `README.md`, `SCOPE.md`, `.github/`, `docs/brand/` | São do FluiDez: mantenha a versão local. |
| Arquivos removidos no FluiDez (`site/`, `docs/catalog`, `docs/CNAME`, `.github/FUNDING.yml`, `.github/ISSUE_TEMPLATE/`, `.github/aw/`, `.github/skills/`, `scripts/generate_release_catalog.py`, `src/images/crossink.png`, `src/images/crossink-white.png`, `src/images/Logo120.h`) | Mantenha-os removidos com `git rm`. |
| `AGENTS.md`, `CLAUDE.md`, `.claude/` | Ficam só no seu computador. Se o upstream alterá-los, tire-os do índice com `git rm -r -q --cached --ignore-unmatch AGENTS.md CLAUDE.md .claude` e restaure a cópia do passo 3. |

## Versão depois da sincronização

A atualização pelo aparelho compara primeiro a versão numérica e, em caso de empate, o número após `fluidez` (veja `src/network/OtaVersion.h`). Ao publicar a próxima versão:

- se a versão base do CrossInk não mudou, aumente o número do FluiDez (`1.6-fluidez11` → `1.6-fluidez12`);
- se a base mudou, recomece a contagem na nova base (`1.6-fluidez12` → `1.7-fluidez1`).

Os passos para publicar estão em [Publicar uma versão](./releasing.md).
