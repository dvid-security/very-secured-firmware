# Signer un firmware pour l'OTA de ce training

Sur cet appareil, l'OTA **vérifie la signature** avant d'installer une image :
il embarque la clé *publique* du fabricant et rejette toute image non signée par
la clé *privée* correspondante. Ce document montre, en pratique, comment
**signer** un firmware pour qu'il soit accepté, et comment **vérifier** cette
signature depuis le PC.

Le fabricant signe ses firmwares avec [minisign](https://jedisct1.github.io/minisign/)
(signature **Ed25519**). La signature — 64 octets — est calculée avec la **clé
privée** ; l'appareil la recalcule avec la **clé publique** embarquée.

## Prérequis

```bash
# Windows
winget install jedisct1.minisign
# macOS / Linux
brew install minisign      # ou: apt install minisign
```

Images de travail :

- `legit.bin` — le firmware légitime (déjà signé par le fabricant),
- `evil.bin` — votre firmware, **à signer** avant de pouvoir le pousser.

## 1. La clé de signature

Seul le détenteur de la clé **privée** peut produire une signature acceptée par
l'appareil ; la clé **publique** correspondante est embarquée dans le firmware.
Dans ce training, la clé privée du fabricant a fuité dans l'historique du dépôt
public (voir l'étape Exploitation) : c'est elle, `minisign.key`, que l'on
utilise ci-dessous. Elle est stockée sans mot de passe : utilisable directement.

Pour référence, une telle paire de clés se génère avec :

```bash
minisign -G -p minisign.pub -s minisign.key
```

## 2. Signer le firmware

```bash
minisign -S -s minisign.key -m evil.bin
```

```
Signature and comment signature verified
Trusted comment: timestamp:... file:evil.bin hashed
```

minisign écrit `evil.bin.minisig` (signature détachée). L'appareil attend la
signature **collée en fin d'image** ; le petit script `tools/append_sig.py` s'en
charge :

```bash
python tools/append_sig.py evil.bin evil.bin.minisig evil_signed.bin
```

Résultat : `evil_signed.bin` = `evil.bin` + 64 octets de signature, l'image à
pousser via `/update`.

## 3. Vérifier la signature avant de pousser

```bash
minisign -Vm evil.bin -p minisign.pub
```

```
Signature and comment signature verified
```

La signature est **présente et valide**. À l'inverse, l'`evil.bin` non signé (ou
signé avec une autre clé) est refusé par l'appareil.

## Le point important

Ici, signer **sert** : l'appareil vérifie la signature côté device (clé publique
embarquée + rejet de toute image non signée) avant d'écrire l'image. C'est le
contrôle qui manquait dans le cas d'école.

Mais ce contrôle ne vaut que par le **secret de la clé privée**. Comme cette clé
a fuité, n'importe qui peut signer un firmware malveillant qui passera la
vérification. Une signature prouve la **possession de la clé**, pas la
légitimité de celui qui l'emploie — d'où l'importance de garder la clé privée
hors du dépôt (voir la Conclusion).
