#!/usr/bin/env python3
"""Print the Brazilian Portuguese release notes of a FluiDez Reader version.

The Release workflow opens the GitHub release page with the "## [v<version>]"
section of NOVIDADES.md, and CI checks that the version in platformio.ini has
one. Exits with status 1 when the section is missing or empty.

Usage: python scripts/release_notes.py 1.6-fluidez10
"""

from __future__ import annotations

import argparse
import re
import sys
from pathlib import Path

DEFAULT_NOTES_FILE = Path(__file__).resolve().parent.parent / "NOVIDADES.md"


def normalize_version(version: str) -> str:
    version = version.strip()
    return version[1:] if version[:1] in ("v", "V") else version


def extract_notes(text: str, version: str) -> str | None:
    """Return the section body for version, or None when there is no section."""
    heading = re.compile(r"^## \[v" + re.escape(normalize_version(version)) + r"\](?:\s|$)")
    lines = text.splitlines()
    for index, line in enumerate(lines):
        if not heading.match(line):
            continue
        body = []
        for following in lines[index + 1 :]:
            if following.startswith("## "):
                break
            body.append(following)
        return "\n".join(body).strip()
    return None


def main() -> int:
    parser = argparse.ArgumentParser(description="Print the NOVIDADES.md section of a FluiDez Reader version.")
    parser.add_argument("version", help="release version, with or without the leading v (e.g. 1.6-fluidez10)")
    parser.add_argument("--file", type=Path, default=DEFAULT_NOTES_FILE, help="notes file (default: NOVIDADES.md)")
    args = parser.parse_args()

    notes = extract_notes(args.file.read_text(encoding="utf-8"), args.version)
    if not notes:
        problem = "is empty" if notes == "" else "is missing"
        print(
            f"error: the '## [v{normalize_version(args.version)}]' section of {args.file.name} {problem}. "
            "Describe what this version changes for readers there, in Brazilian Portuguese.",
            file=sys.stderr,
        )
        return 1
    sys.stdout.buffer.write((notes + "\n").encode("utf-8"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
