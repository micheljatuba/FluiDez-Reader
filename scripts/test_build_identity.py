"""Check that volatile build identity reaches only BuildInfo.cpp."""
from pathlib import Path
import runpy
import unittest

ROOT = Path(__file__).resolve().parents[1]
MODULE = runpy.run_path(str(ROOT / "scripts/git_branch.py"))

class Node:
    def __init__(self, path): self.path = path
    def get_path(self): return self.path

class Env(dict):
    def __init__(self, name="default"):
        super().__init__(PROJECT_DIR=str(ROOT), PIOENV=name)
        self.defines = []
        self.middleware = []
    def Append(self, **kwargs): self.defines.extend(kwargs.get("CPPDEFINES", []))
    def Clone(self):
        copy = Env(self["PIOENV"])
        copy.defines = list(self.defines)
        return copy
    # The middleware hands its scoped defines to Object instead of a cloned env.
    def Object(self, node, CPPDEFINES=None): return (node, CPPDEFINES)
    def AddBuildMiddleware(self, callback, pattern): self.middleware.append((callback, pattern))

class BuildIdentityTest(unittest.TestCase):
    def test_identity_only_changes_one_compile_environment(self):
        function = MODULE["inject_version"]
        globals_ = function.__globals__
        previous = globals_["get_git_short_sha"]
        self.addCleanup(globals_.__setitem__, "get_git_short_sha", previous)
        for name in ("default", "sticky", "x4-pro", "x4-classic", "debug", "sticky-debug", "x4-pro-debug", "test", "gh_release_rc", "x4-pro-simulator"):
            captures = []
            for sha in ("abc1234", "def5678"):
                globals_["get_git_short_sha"] = lambda _: sha
                env = Env(name)
                function(env)
                self.assertEqual(len(env.middleware), 1)
                callback, pattern = env.middleware[0]
                self.assertEqual(pattern, "*src/util/BuildInfo.cpp")
                other = Node("src/main.cpp")
                self.assertIs(callback(env, other), other)  # other sources compile unchanged
                _, scoped = callback(env, Node("src/util/BuildInfo.cpp"))
                names = [d[0] for d in env.defines if isinstance(d, tuple)]
                self.assertNotIn("FLUIDEZ_GIT_SHA", names)
                self.assertNotIn("FLUIDEZ_GIT_DIRTY", names)
                self.assertNotIn("FLUIDEZ_VERSION", names)
                self.assertIn("FLUIDEZ_GIT_SHA", dict(d for d in scoped if isinstance(d, tuple)))
                captures.append((env.defines, scoped))
            self.assertEqual(captures[0][0], captures[1][0])
            self.assertNotEqual(captures[0][1], captures[1][1])

if __name__ == "__main__": unittest.main()
