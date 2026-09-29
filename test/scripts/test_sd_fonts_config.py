"""Run with python3 -m unittest discover -s test/scripts -p test_sd_fonts_config.py."""
import unittest
from pathlib import Path

import yaml

CONFIG = Path(__file__).resolve().parents[2] / "lib" / "EpdFont" / "scripts" / "sd-fonts.yaml"
# generate-font-manifest.py reads "languages" with a default, so it stays optional.
REQUIRED_FIELDS = {"name", "description", "intervals", "sizes", "styles"}
REQUIRED_STYLES = {"regular", "bold", "italic", "bolditalic"}


class SdFontsConfigTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.families = yaml.safe_load(CONFIG.read_text(encoding="utf-8"))["families"]

    def test_family_names_are_unique(self):
        names = [family["name"] for family in self.families]
        self.assertCountEqual(names, set(names))

    def test_every_family_is_complete(self):
        for family in self.families:
            with self.subTest(family=family.get("name")):
                self.assertTrue(REQUIRED_FIELDS.issubset(family))
                self.assertEqual(REQUIRED_STYLES, set(family["styles"]))
                self.assertTrue(family["sizes"])

    def test_every_style_resolves_to_a_source(self):
        for family in self.families:
            for style, spec in family["styles"].items():
                with self.subTest(family=family["name"], style=style):
                    self.assertTrue(
                        ("url" in spec) ^ ("path" in spec),
                        "a style needs exactly one of 'url' or 'path'",
                    )

    def test_variable_styles_pin_numeric_axes(self):
        for family in self.families:
            for style, spec in family["styles"].items():
                axes = spec.get("variable")
                if axes is None:
                    continue
                with self.subTest(family=family["name"], style=style):
                    self.assertTrue(axes, "'variable' must pin at least one axis")
                    for axis, value in axes.items():
                        self.assertRegex(axis, r"^[A-Za-z]{4}$")
                        self.assertIsInstance(value, (int, float))

    def test_bold_is_heavier_than_regular_for_variable_families(self):
        for family in self.families:
            styles = family["styles"]
            weights = {
                name: styles[name]["variable"]["wght"]
                for name in REQUIRED_STYLES
                if "wght" in styles[name].get("variable", {})
            }
            if not weights:
                continue
            with self.subTest(family=family["name"]):
                if {"regular", "bold"} <= set(weights):
                    self.assertLess(weights["regular"], weights["bold"])
                if {"italic", "bolditalic"} <= set(weights):
                    self.assertLess(weights["italic"], weights["bolditalic"])


if __name__ == "__main__":
    unittest.main()
