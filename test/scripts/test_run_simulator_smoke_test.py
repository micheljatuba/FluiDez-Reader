"""Run with python3 -m unittest discover -s test/scripts -p test_run_simulator_smoke_test.py."""
import argparse
import contextlib
import io
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "scripts"))
import run_simulator_smoke_test as smoke


class SimulatorSmokeRunnerTest(unittest.TestCase):
    def setUp(self):
        directory = tempfile.TemporaryDirectory(prefix="crossink-smoke-runner-test-")
        self.addCleanup(directory.cleanup)
        self.root = Path(directory.name)
        self.book = self.root / "fixture.epub"
        self.book.write_bytes(b"fixture")
        self.program = self.root / "program"
        self.program.touch()
        self.args = argparse.Namespace(
            book=str(self.book), env="x4-pro-simulator", build=False,
            page_turns=2, theme="classic", headless=True, timeout=120,
            font_dir=None, font_family=None, frontlight_sync=False,
            frontlight_layout=False, frontlight_captures=None, home_themes=False,
        )

    def run_smoke(self, *, output="", returncode=0, error=None):
        stdout, stderr = io.StringIO(), io.StringIO()
        with patch.object(smoke, "program_path", return_value=self.program), \
                patch.object(smoke.subprocess, "run", side_effect=error) as run, \
                contextlib.redirect_stdout(stdout), contextlib.redirect_stderr(stderr):
            run.return_value = subprocess.CompletedProcess([str(self.program)], returncode, output)
            result = smoke.run_smoke(self.args)
        return result, stdout.getvalue(), stderr.getvalue(), run

    def test_success_uses_isolated_filesystem_and_headless_x4_pro_options(self):
        def check_run(command, **kwargs):
            self.assertEqual(command, [str(self.program)])
            temp_root = kwargs["cwd"]
            self.assertEqual((temp_root / "fs_" / "books" / self.book.name).read_bytes(), b"fixture")
            self.assertEqual(kwargs["env"]["SDL_VIDEODRIVER"], "dummy")
            self.assertEqual(kwargs["env"]["CROSSINK_SIMULATOR_SMOKE_TEST"], "1")
            self.assertEqual(kwargs["env"]["CROSSINK_SIMULATOR_SMOKE_BOOK"], "/books/fixture.epub")
            self.assertEqual(kwargs["env"]["CROSSINK_SIMULATOR_SMOKE_PAGE_TURNS"], "2")
            self.assertEqual(kwargs["env"]["CROSSINK_SIMULATOR_SMOKE_THEME"], "0")
            self.assertEqual(kwargs["timeout"], 120)
            self.assertEqual(kwargs["stderr"], subprocess.STDOUT)
            return subprocess.CompletedProcess(command, 0, "Simulator smoke test passed\n")

        with patch.dict(smoke.os.environ, {}, clear=True):
            result, stdout, stderr, run = self.run_smoke(error=check_run)
        self.assertEqual(result, 0)
        self.assertIn("Simulator smoke test passed", stdout)
        self.assertEqual(stderr, "")
        self.assertFalse(run.call_args.kwargs["cwd"].exists())

    def test_nonzero_exit_is_not_hidden_by_success_marker(self):
        result, stdout, stderr, _ = self.run_smoke(output="Simulator smoke test passed\n", returncode=7)
        self.assertEqual(result, 7)
        self.assertIn("Simulator smoke test passed", stdout)
        self.assertIn("exit code 7", stderr)

    def test_missing_success_marker_fails(self):
        result, _, stderr, _ = self.run_smoke(output="Still reading\n")
        self.assertEqual(result, 2)
        self.assertIn("did not print its success marker", stderr)

    def test_crash_patterns_fail_even_with_success_marker(self):
        for pattern in smoke.CRASH_PATTERNS:
            with self.subTest(pattern=pattern):
                result, _, stderr, _ = self.run_smoke(output=f"Simulator smoke test passed\n{pattern}\n")
                self.assertEqual(result, 2)
                self.assertIn(pattern, stderr)

    def test_timeout_preserves_partial_output_and_cleans_filesystem(self):
        for output in (b"Last step: reader\n\xff", "Last step: reader\n", None):
            with self.subTest(output=output):
                error = subprocess.TimeoutExpired([str(self.program)], 120, output=output)
                result, stdout, stderr, run = self.run_smoke(error=error)
                self.assertEqual(result, 124)
                if output is not None:
                    self.assertIn("Last step: reader", stdout)
                self.assertIn("timed out after 120 seconds", stderr)
                self.assertFalse(run.call_args.kwargs["cwd"].exists())

    def test_missing_book_fails_without_launching(self):
        self.args.book = str(self.root / "missing.epub")
        result, _, stderr, run = self.run_smoke()
        self.assertEqual(result, 2)
        self.assertIn("Smoke test book not found", stderr)
        run.assert_not_called()

    def test_missing_binary_fails_without_launching(self):
        self.program.unlink()
        result, _, stderr, run = self.run_smoke()
        self.assertEqual(result, 2)
        self.assertIn("pio run -e x4-pro-simulator", stderr)
        run.assert_not_called()


if __name__ == "__main__":
    unittest.main()
