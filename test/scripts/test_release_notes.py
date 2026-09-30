"""Run with python3 -m unittest discover -s test/scripts -p test_release_notes.py."""
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

SCRIPT = Path(__file__).resolve().parents[2] / "scripts" / "release_notes.py"
sys.path.insert(0, str(SCRIPT.parent))
import release_notes

NOTES = """# Novidades

## [Próxima versão]

## [v1.6-fluidez10] - 2026-09-30

- Dez

## [v1.6-fluidez1] - 2026-01-01

Primeira.

- Um
"""


class ExtractNotesTest(unittest.TestCase):
    def test_returns_only_the_section_of_the_version(self):
        self.assertEqual("- Dez", release_notes.extract_notes(NOTES, "1.6-fluidez10"))

    def test_accepts_the_tag_name(self):
        self.assertEqual("- Dez", release_notes.extract_notes(NOTES, "v1.6-fluidez10"))

    def test_does_not_match_a_longer_version(self):
        self.assertEqual("Primeira.\n\n- Um", release_notes.extract_notes(NOTES, "1.6-fluidez1"))
        self.assertIsNone(release_notes.extract_notes(NOTES, "1.6-fluidez"))

    def test_dots_in_the_version_are_literal(self):
        self.assertIsNone(release_notes.extract_notes(NOTES, "1x6-fluidez10"))

    def test_missing_section_returns_none(self):
        self.assertIsNone(release_notes.extract_notes(NOTES, "1.6-fluidez99"))
        self.assertIsNone(release_notes.extract_notes(NOTES, ""))

    def test_empty_section_returns_empty_text(self):
        self.assertEqual("", release_notes.extract_notes(NOTES.replace("- Dez\n", ""), "1.6-fluidez10"))

    def test_reads_crlf_files(self):
        self.assertEqual("- Dez", release_notes.extract_notes(NOTES.replace("\n", "\r\n"), "1.6-fluidez10"))


class CommandLineTest(unittest.TestCase):
    def run_script(self, version, text):
        with tempfile.TemporaryDirectory(prefix="fluidez-release-notes-test-") as directory:
            notes_file = Path(directory) / "NOVIDADES.md"
            notes_file.write_text(text, encoding="utf-8")
            return subprocess.run(
                [sys.executable, str(SCRIPT), version, "--file", str(notes_file)],
                capture_output=True,
                check=False,
            )

    def test_prints_the_notes_as_utf8(self):
        result = self.run_script("1.6-fluidez10", NOTES.replace("- Dez", "- Configurações"))
        self.assertEqual(0, result.returncode)
        self.assertEqual("- Configurações\n", result.stdout.decode("utf-8"))

    def test_fails_without_notes_for_the_version(self):
        for version, problem in (("1.6-fluidez99", "is missing"), ("1.6-fluidez10", "is empty")):
            with self.subTest(version=version):
                result = self.run_script(version, NOTES.replace("- Dez\n", ""))
                self.assertEqual(1, result.returncode)
                self.assertEqual(b"", result.stdout)
                self.assertIn(problem, result.stderr.decode("utf-8"))


if __name__ == "__main__":
    unittest.main()
