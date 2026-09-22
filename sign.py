#!/usr/bin/env python3
import base64
import sys

fw, sig_file, out = sys.argv[1:4]

# Ligne 2 du .minisig = base64( "ED" + keyid[8] + signature[64] ).
blob = base64.b64decode(open(sig_file).read().splitlines()[1])
signature = blob[10:]                       # 64 octets

open(out, "wb").write(open(fw, "rb").read() + signature)
print(f"{out} ecrit ({len(signature)} octets de signature ajoutes)")