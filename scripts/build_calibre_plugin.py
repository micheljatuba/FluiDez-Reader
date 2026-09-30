#!/usr/bin/env python3
"""Build the FluiDez Reader Calibre plugin zip."""

from __future__ import annotations

import argparse
import py_compile
import sys
import tempfile
import zipfile
from pathlib import Path

FIXED_DATE = (2026, 1, 1, 0, 0, 0)


def repo_root() -> Path:
    return Path(__file__).resolve().parents[1]


def iter_files(plugin_dir: Path):
    for path in sorted(plugin_dir.rglob('*')):
        if not path.is_file():
            continue
        parts = set(path.parts)
        if '__pycache__' in parts or path.suffix == '.pyc':
            continue
        yield path


def compile_sources(plugin_dir: Path) -> None:
    with tempfile.TemporaryDirectory(prefix='fluidez-calibre-pycompile-') as tmp:
        tmpdir = Path(tmp)
        for index, path in enumerate(iter_files(plugin_dir)):
            if path.suffix == '.py':
                py_compile.compile(str(path), cfile=str(tmpdir / f'{index}.pyc'), doraise=True)


def add_file(zf: zipfile.ZipFile, source: Path, arcname: str) -> None:
    info = zipfile.ZipInfo(arcname.replace('\\', '/'), FIXED_DATE)
    info.compress_type = zipfile.ZIP_DEFLATED
    info.external_attr = 0o644 << 16
    zf.writestr(info, source.read_bytes())


def add_empty(zf: zipfile.ZipFile, arcname: str) -> None:
    info = zipfile.ZipInfo(arcname, FIXED_DATE)
    info.compress_type = zipfile.ZIP_STORED
    info.external_attr = 0o644 << 16
    zf.writestr(info, b'')


def build(output: Path) -> Path:
    root = repo_root()
    plugin_dir = root / 'calibre-plugin' / 'fluidez_reader'
    license_file = root / 'calibre-plugin' / 'LICENSE'
    if not plugin_dir.is_dir():
        raise SystemExit(f'Missing plugin package: {plugin_dir}')
    if not license_file.is_file():
        raise SystemExit(f'Missing license: {license_file}')

    compile_sources(plugin_dir)
    output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(output, 'w') as zf:
        for path in iter_files(plugin_dir):
            add_file(zf, path, path.relative_to(plugin_dir).as_posix())
        add_file(zf, license_file, 'LICENSE')
        add_empty(zf, 'plugin-import-name-fluidez_reader.txt')
    return output


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description='Build the FluiDez Reader Calibre plugin zip.')
    parser.add_argument('--output', type=Path, default=repo_root() / 'dist' / 'fluidez-reader-calibre-plugin.zip')
    args = parser.parse_args(argv)
    try:
        output = build(args.output.resolve())
    except py_compile.PyCompileError as exc:
        print(exc.msg, file=sys.stderr)
        return 1
    print(output)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
