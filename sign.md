# Générer les clés
## Prérequis

Minisign :
```bash
# Windows
winget install jedisct1.minisign
# MacOS
brew install minisign
# Linux
apt install minisign
```

## Utiliser Minisign

Les clés se génère avec :

```bash
minisign -G -p minisign.pub -s minisign.key
```

# Signer le firmware

```bash
minisign -S -s minisign.key -m legit.bin
```

minisign écrit `legit.bin.minisig` (signature détachée). 

L'appareil attend la
signature collée en fin d'image. Le script `sign.py` s'en charge :

```bash
python sign.py legit.bin legit.bin.minisig legit_signed.bin
```

Résultat : `legit_signed.bin`, firmware à déployer

# Vérifier la signature avant de deployer

```bash
minisign -Vm legit.bin -p minisign.pub
```