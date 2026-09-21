Cet appareil faisait pourtant « ce qu'il fallait » : sa mise à jour OTA
**vérifie la signature** du firmware. Il embarque la clé publique du fabricant
et rejette toute image non signée — on l'a constaté quand `evil.bin` brut a été
refusé.

Et malgré ça, l'appareil a été compromis. Pourquoi ? Parce que la **clé privée
de signature avait fuité** dans l'historique du dépôt public. À partir de là, la
vérification ne protège plus rien : n'importe qui pouvait signer un firmware
malveillant comme s'il était officiel.

La leçon n'est donc pas « il faut vérifier les signatures » (c'était fait), mais
**la vérification ne vaut que par le secret de la clé privée**. Une signature
prouve seulement qu'on possède la clé — pas qu'on est légitime.

## Le vrai problème : la gestion des secrets

Une clé privée committée dans un dépôt Git **fuit pour toujours** :

- Un `git rm` (ou une suppression via l'interface) ne l'efface **pas** de
  l'historique : le commit qui l'a ajoutée reste consultable.
- Sur un dépôt public, tout le monde peut parcourir cet historique — via
  l'interface web ou un simple `git clone` + `git log`.
- Des outils automatisés (`trufflehog`, `git-secrets`, `gitleaks`) trouvent ces
  secrets en quelques secondes.

Bonnes pratiques :

- **Ne jamais** placer une clé privée dans un dépôt de code. La garder hors du
  dépôt (secret manager, HSM/KMS), et l'exclure via `.gitignore`.
- Scanner le code et l'historique en continu (secret scanning en CI).
- En cas de fuite : considérer la clé comme définitivement compromise, la
  **révoquer** et en **déployer une nouvelle** (rotation) — la supprimer du
  dépôt ne suffit pas.
- Prévoir dès la conception la **révocation de clé** côté appareil (embarquer
  plusieurs clés de confiance, pouvoir pousser une mise à jour qui change la clé
  acceptée), pour invalider une clé fuitée sans jeter le parc.

## Détection : une interface saine ne prouve rien

Rappel de l'étape post-exploitation : le firmware malveillant ressert des pages
web **identiques**, mais a ouvert une backdoor sur un port haut (`54321`),
invisible à un scan `nmap` par défaut ; seul un scan complet (`nmap -p-`) la
révèle. Face à un objet dont on ne maîtrise pas le firmware, on cartographie
**tous** les ports, pas seulement ceux attendus.

L'idée clé : signer et vérifier, c'est nécessaire — mais inutile si la clé de
signature n'est pas protégée. La sécurité de tout le mécanisme se réduit à la
confidentialité d'un seul secret.
