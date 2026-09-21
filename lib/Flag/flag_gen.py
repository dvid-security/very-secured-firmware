#!/usr/bin/env python3
"""Offline flag-obfuscation generator for DVID trainings.

Given a clear flag, produce:
  - a noisy `message` string with the flag chars scattered inside random noise
  - the index table pointing at those chars
  - ready-to-paste C++ for use with extractFlag() (snipet/ia/flag.h)

Usage:
    python flag_gen.py "GH_Comput3rs"
    python flag_gen.py "myflag" --noise 3 --seed 42

--noise N : average number of noise chars inserted between two flag chars.
--seed S  : fix the RNG for reproducible output.
"""

import argparse
import random
import string


def obfuscate(flag: str, noise: int, rng: random.Random):
    """Scatter `flag` chars inside random lowercase noise.

    Returns (message, indices) so that ''.join(message[i] for i in indices) == flag.
    """
    alphabet = string.ascii_lowercase
    message = []
    indices = []
    for ch in flag:
        # random run of noise before each flag char
        for _ in range(rng.randint(0, max(0, noise * 2))):
            message.append(rng.choice(alphabet))
        indices.append(len(message))
        message.append(ch)
    # trailing noise
    for _ in range(rng.randint(0, max(0, noise * 2))):
        message.append(rng.choice(alphabet))
    return "".join(message), indices


def cpp_snippet(message: str, indices: list) -> str:
    idx = ", ".join(str(i) for i in indices)
    return (
        f'const uint8_t idx[] = {{{idx}}};\n'
        f'String flag = extractFlag(F("{message}"), idx, sizeof(idx));'
    )


def main():
    p = argparse.ArgumentParser(description="Generate an obfuscated flag message.")
    p.add_argument("flag", help="clear flag text to hide")
    p.add_argument("--noise", type=int, default=2,
                   help="avg noise chars between flag chars (default 2)")
    p.add_argument("--seed", type=int, default=None, help="RNG seed")
    args = p.parse_args()

    rng = random.Random(args.seed)
    message, indices = obfuscate(args.flag, args.noise, rng)

    # sanity check
    rebuilt = "".join(message[i] for i in indices)
    assert rebuilt == args.flag, f"mismatch: {rebuilt!r} != {args.flag!r}"

    print(f"message : {message}")
    print(f"indices : {indices}")
    print(f"length  : {len(message)}")
    print()
    print(cpp_snippet(message, indices))


if __name__ == "__main__":
    main()
