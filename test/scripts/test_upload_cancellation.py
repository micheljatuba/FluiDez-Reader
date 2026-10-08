#!/usr/bin/env python3
"""Compile the production upload WRITE/END/ABORTED paths with fake I/O.

Does not emulate Arduino's multipart transport or physical input debounce.
"""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]
source = (ROOT / 'src/network/FluiDezWebServer.cpp').read_text()
activity = (ROOT / 'src/activities/network/FluiDezWebServerActivity.cpp').read_text()


def function(text, signature):
    start = text.index(signature)
    end = text.index('\n}', start) + 2
    return text[start:end] + '\n'


parts = [function(source, signature) for signature in (
    'bool FluiDezWebServer::dropUploadIfCancelled()',
    'void FluiDezWebServer::abortUpload(',
    'void FluiDezWebServer::abortFontUpload()',
    'static bool flushUploadBuffer(')]
handler = function(source, 'void FluiDezWebServer::handleUpload(')
start = handler.index('  } else if (upload.status == UPLOAD_FILE_WRITE)')
parts.append('void FluiDezWebServer::handleUpload(UploadState& state) const {\n'
             'const HTTPUpload& upload = server->upload();\n  if' + handler[start + len('  } else if'):])
font = function(source, 'void FluiDezWebServer::handleFontUploadData()')
parts.append('void FluiDezWebServer::handleFontUploadData() {\n'
             'HTTPUpload& upload = server->upload();\nswitch(upload.status) {\n' +
             font[font.index('    case UPLOAD_FILE_WRITE:'):])
parts.append(function(activity, 'bool FluiDezWebServerActivity::checkUploadCancellation()'))
# Normal requests must return to the owner loop without consuming its touch events.
batch = activity[activity.index('      // Process a batch of HTTP requests'):activity.index('      lastHandleClientTime = millis();', activity.index('      // Process a batch'))]
assert 'mappedInput.update(' not in batch
assert 'exitToOrigin();' in batch and 'if (leaveRequested)' in batch
with tempfile.TemporaryDirectory(prefix='crossink-upload-cancel-') as tmp:
    root = Path(tmp)
    (root / 'UploadHandlers.inc').write_text('\n'.join(parts))
    executable = root / 'test'
    subprocess.run(['c++', '-std=c++20', '-Wall', '-Wextra', '-Wno-unused-variable',
                    '-fsanitize=address,undefined', '-I' + str(root),
                    str(ROOT / 'test/upload_cancellation/UploadCancellationTest.cpp'), '-o', str(executable)], check=True)
    subprocess.run([str(executable)], check=True)
print('PASS: book/font uploads, chunk/final cancellation, late END/ABORTED, cleanup and latched exit input')
