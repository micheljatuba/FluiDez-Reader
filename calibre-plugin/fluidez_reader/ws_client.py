import base64
import os
import select
import socket
import struct
import threading
import time

HOTSPOT_IP = '192.168.4.1'
_UNRESOLVED_RETRY_SECONDS = 30.0
_unresolved_until = {}


class WebSocketError(RuntimeError):
    pass


class UploadError(WebSocketError):
    def __init__(self, message, upload_started=False):
        super().__init__(message)
        self.upload_started = upload_started


class WebSocketClient:
    def __init__(self, host, port, timeout=10, debug=False, logger=None):
        self.host = host
        self.port = port
        self.timeout = timeout
        self.debug = debug
        self.logger = logger
        self.sock = None

    def _log(self, *args):
        if self.debug:
            msg = '[FluiDez WS] ' + ' '.join(str(a) for a in args)
            if self.logger:
                self.logger(msg)
            else:
                print(msg)

    def connect(self):
        try:
            self.sock = socket.create_connection((self.host, self.port), self.timeout)
        except OSError as exc:
            raise WebSocketError('Failed to connect: ' + str(exc)) from exc
        key = base64.b64encode(os.urandom(16)).decode('ascii')
        req = (
            'GET / HTTP/1.1\r\n'
            f'Host: {self.host}:{self.port}\r\n'
            'Upgrade: websocket\r\n'
            'Connection: Upgrade\r\n'
            f'Sec-WebSocket-Key: {key}\r\n'
            'Sec-WebSocket-Version: 13\r\n'
            '\r\n'
        )
        try:
            self.sock.sendall(req.encode('ascii'))
        except OSError as exc:
            raise WebSocketError('Failed to send handshake: ' + str(exc)) from exc
        data = self._read_http_response()
        if b' 101 ' not in data.split(b'\r\n', 1)[0]:
            raise WebSocketError('Handshake failed: ' + data.split(b'\r\n', 1)[0].decode('ascii', 'ignore'))
        self._log('Handshake OK')

    def _read_http_response(self):
        self.sock.settimeout(self.timeout)
        data = b''
        while b'\r\n\r\n' not in data:
            try:
                chunk = self.sock.recv(1024)
            except OSError as exc:
                raise WebSocketError('Failed to read handshake: ' + str(exc)) from exc
            if not chunk:
                break
            data += chunk
        return data

    def close(self):
        if not self.sock:
            return
        try:
            self._send_frame(0x8, b'')
        except Exception:
            pass
        try:
            self.sock.close()
        finally:
            self.sock = None

    def send_text(self, text):
        self._send_frame(0x1, text.encode('utf-8'))

    def send_binary(self, payload):
        self._send_frame(0x2, payload)

    def _send_frame(self, opcode, payload):
        if self.sock is None:
            raise WebSocketError('Socket not connected')
        fin = 0x80
        first = fin | (opcode & 0x0F)
        mask_bit = 0x80
        length = len(payload)
        header = bytearray([first])
        if length <= 125:
            header.append(mask_bit | length)
        elif length <= 65535:
            header.append(mask_bit | 126)
            header.extend(struct.pack('!H', length))
        else:
            header.append(mask_bit | 127)
            header.extend(struct.pack('!Q', length))

        mask = os.urandom(4)
        header.extend(mask)
        masked = bytearray(payload)
        for i in range(length):
            masked[i] ^= mask[i % 4]
        try:
            self.sock.sendall(header + masked)
        except OSError as exc:
            raise WebSocketError('Failed to send WebSocket frame: ' + str(exc)) from exc

    def read_text(self):
        deadline = time.time() + self.timeout
        while True:
            if time.time() > deadline:
                raise WebSocketError('Timed out waiting for text frame')
            opcode, payload = self._read_frame()
            if opcode == 0x8:
                code = None
                reason = ''
                if len(payload) >= 2:
                    code = struct.unpack('!H', payload[:2])[0]
                    reason = payload[2:].decode('utf-8', 'ignore')
                self._log('Server closed connection', code, reason)
                raise WebSocketError('Connection closed')
            if opcode == 0x9:
                # Ping -> respond with Pong
                self._send_frame(0xA, payload)
                continue
            if opcode == 0xA:
                # Pong -> ignore
                continue
            if opcode != 0x1:
                self._log('Ignoring non-text opcode', opcode, len(payload))
                continue
            return payload.decode('utf-8', 'ignore')

    def _read_frame(self):
        if self.sock is None:
            raise WebSocketError('Socket not connected')
        hdr = self._recv_exact(2)
        b1, b2 = hdr[0], hdr[1]
        opcode = b1 & 0x0F
        masked = (b2 & 0x80) != 0
        length = b2 & 0x7F
        if length == 126:
            length = struct.unpack('!H', self._recv_exact(2))[0]
        elif length == 127:
            length = struct.unpack('!Q', self._recv_exact(8))[0]
        mask = b''
        if masked:
            mask = self._recv_exact(4)
        payload = self._recv_exact(length) if length else b''
        if masked:
            payload = bytes(b ^ mask[i % 4] for i, b in enumerate(payload))
        return opcode, payload

    def _recv_exact(self, n):
        data = b''
        while len(data) < n:
            try:
                chunk = self.sock.recv(n - len(data))
            except OSError as exc:
                raise WebSocketError('Failed to read WebSocket frame: ' + str(exc)) from exc
            if not chunk:
                raise WebSocketError('Socket closed')
            data += chunk
        return data

    def drain_messages(self):
        if self.sock is None:
            return []
        messages = []
        while True:
            r, _, _ = select.select([self.sock], [], [], 0)
            if not r:
                break
            opcode, payload = self._read_frame()
            if opcode == 0x1:
                messages.append(payload.decode('utf-8', 'ignore'))
            elif opcode == 0x8:
                raise WebSocketError('Connection closed')
        return messages


def _log(logger, debug, message):
    if not debug:
        return
    if logger:
        logger(message)
    else:
        print(message)


def _broadcast_from_host(host):
    parts = host.split('.')
    if len(parts) != 4:
        return None
    try:
        _ = [int(p) for p in parts]
    except Exception:
        return None
    parts[-1] = '255'
    return '.'.join(parts)


def _is_ipv4_literal(host):
    try:
        socket.inet_aton(host)
        return host.count('.') == 3
    except OSError:
        return False


def _lookup_ipv4(host, out):
    try:
        for info in socket.getaddrinfo(host, None, socket.AF_INET):
            if info[4][0] not in out:
                out.append(info[4][0])
    except Exception:
        pass


def _resolve_ipv4_hosts(hosts, timeout=1.5):
    """Resolve names in parallel under one deadline, so a .local name that does
    not answer cannot stall discovery. Names that fail are skipped for a while."""
    resolved = {}
    pending = []
    now = time.time()
    for host in hosts:
        if _is_ipv4_literal(host):
            resolved[host] = [host]
        elif _unresolved_until.get(host, 0.0) <= now:
            ips = []
            thread = threading.Thread(target=_lookup_ipv4, args=(host, ips), daemon=True)
            thread.start()
            pending.append((host, ips, thread))
    deadline = time.time() + timeout
    for host, ips, thread in pending:
        thread.join(max(0.0, deadline - time.time()))
        if ips:
            resolved[host] = list(ips)
        else:
            _unresolved_until[host] = time.time() + _UNRESOLVED_RETRY_SECONDS
    return resolved


def _local_broadcast_addrs():
    addrs = set()
    try:
        for info in socket.getaddrinfo(socket.gethostname(), None, socket.AF_INET):
            ip = info[4][0]
            if ip.startswith('127.'):
                continue
            bcast = _broadcast_from_host(ip)
            if bcast:
                addrs.add(bcast)
    except Exception:
        pass
    return addrs


def discover_device(timeout=2.0, debug=False, logger=None, extra_hosts=None):
    ports = [8134, 54982, 48123, 39001, 44044, 59678]
    local_port = 0
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    sock.settimeout(0.5)
    try:
        sock.bind(('', local_port))
    except Exception:
        _log(logger, debug, '[FluiDez WS] discovery bind failed')

    msg = b'hello'
    try:
        addr, port = sock.getsockname()
        _log(logger, debug, f'[FluiDez WS] discovery local {addr} {port}')
    except Exception:
        pass

    targets = []
    for bcast in _local_broadcast_addrs():
        _log(logger, debug, f'[FluiDez WS] discovery subnet broadcast {bcast}')
        for port in ports:
            targets.append((bcast, port))
    for port in ports:
        targets.append(('255.255.255.255', port))

    hosts = []
    for host in list(extra_hosts or []) + [HOTSPOT_IP]:
        host = (host or '').strip()
        if host and host not in hosts:
            hosts.append(host)
    resolved = _resolve_ipv4_hosts(hosts)
    for host in hosts:
        ips = resolved.get(host)
        if not ips:
            _log(logger, debug, f'[FluiDez WS] discovery skipped unresolved host {host}')
            continue
        for ip in ips:
            _log(logger, debug, f'[FluiDez WS] discovery host {host} -> {ip}')
            for port in ports:
                targets.append((ip, port))
            bcast = _broadcast_from_host(ip)
            if bcast:
                for port in ports:
                    targets.append((bcast, port))

    seen_targets = set()
    targets = [t for t in targets if not (t in seen_targets or seen_targets.add(t))]

    try:
        for _ in range(3):
            for host, port in targets:
                try:
                    sock.sendto(msg, (host, port))
                except Exception as exc:
                    _log(logger, debug, f'[FluiDez WS] discovery send failed {host}:{port} {exc}')
            start = time.time()
            while time.time() - start < timeout:
                try:
                    data, addr = sock.recvfrom(256)
                except ConnectionResetError:
                    # On Windows, ICMP "port unreachable" replies to the probes sent
                    # to closed ports surface here; keep reading the real answers.
                    continue
                except Exception:
                    break
                _log(logger, debug, f'[FluiDez WS] discovery {addr} {data}')
                try:
                    text = data.decode('utf-8', 'ignore')
                except Exception:
                    continue
                lowered = text.lower()
                if not (lowered.startswith('crosspoint') or lowered.startswith('fluidez')):
                    _log(logger, debug, f'[FluiDez WS] discovery ignoring non-FluiDez response: {text}')
                    continue
                semi = text.find(';')
                port = 81
                if semi != -1:
                    try:
                        port = int(text[semi + 1:].strip().split(',')[0])
                    except Exception:
                        port = 81
                return addr[0], port
    finally:
        sock.close()
    return None, None


def upload_file(host, port, upload_path, filename, filepath, chunk_size=16384, debug=False, progress_cb=None,
                logger=None, timeout=10):
    client = WebSocketClient(host, port, timeout=timeout, debug=debug, logger=logger)
    upload_started = False
    try:
        try:
            client.connect()
            size = os.path.getsize(filepath)
            start = f'START:{filename}:{size}:{upload_path}'
            client._log('Sending START', start)
            client.send_text(start)

            msg = client.read_text()
            client._log('Received', msg)
            if not msg:
                raise WebSocketError('Unexpected response: <empty>')
            if msg.startswith('ERROR'):
                raise WebSocketError(msg)
            if msg != 'READY':
                raise WebSocketError('Unexpected response: ' + msg)

            sent = 0
            with open(filepath, 'rb') as f:
                while True:
                    chunk = f.read(chunk_size)
                    if not chunk:
                        break
                    upload_started = True
                    client.send_binary(chunk)
                    sent += len(chunk)
                    if progress_cb:
                        progress_cb(sent, size)
                    client.drain_messages()

            # Wait for DONE or ERROR
            while True:
                msg = client.read_text()
                client._log('Received', msg)
                if msg == 'DONE':
                    return
                if msg.startswith('ERROR'):
                    raise WebSocketError(msg)
        except UploadError:
            raise
        except (WebSocketError, OSError) as exc:
            raise UploadError(str(exc), upload_started=upload_started) from exc
    finally:
        client.close()
