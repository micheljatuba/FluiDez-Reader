from calibre.utils.config import JSONConfig
from qt.core import (
    QCheckBox,
    QComboBox,
    QDialog,
    QDialogButtonBox,
    QFormLayout,
    QFrame,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QPlainTextEdit,
    QPushButton,
    QSpinBox,
    QVBoxLayout,
    QWidget,
)

from .log import get_log_text


PREFS = JSONConfig('plugins/fluidez_reader')
PREFS.defaults['host'] = 'fluidez.local'
PREFS.defaults['port'] = 81
PREFS.defaults['path'] = '/'
PREFS.defaults['chunk_size'] = 2048
PREFS.defaults['debug'] = False
PREFS.defaults['fetch_metadata'] = False
PREFS.defaults['send_to_root'] = False
PREFS.defaults['overwrite_existing'] = False
PREFS.defaults['upload_template'] = ''
PREFS.defaults['upload_retries'] = 3
PREFS.defaults['retry_delay'] = 2
PREFS.defaults['book_cooldown'] = 1
PREFS.defaults['socket_timeout'] = 30
# Optimizer settings (mirrors the FluiDez Reader web server optimizer).
PREFS.defaults['optimize'] = False
PREFS.defaults['optimize_grayscale'] = True
PREFS.defaults['optimize_auto_crop'] = False
PREFS.defaults['optimize_quality'] = 85
PREFS.defaults['optimize_split'] = True
PREFS.defaults['device_target'] = 'auto'  # 'auto' | 'X4' | 'X3'


class FluiDezConfigWidget(QWidget):
    def __init__(self):
        super().__init__()
        layout = QFormLayout(self)
        layout.setFieldGrowthPolicy(QFormLayout.FieldGrowthPolicy.ExpandingFieldsGrow)
        self.host = QLineEdit(self)
        self.port = QSpinBox(self)
        self.port.setRange(1, 65535)
        self.path = QLineEdit(self)
        self.upload_template = QLineEdit(self)
        self.chunk_size = QSpinBox(self)
        self.chunk_size.setRange(512, 65536)
        self.upload_retries = QSpinBox(self)
        self.upload_retries.setRange(0, 10)
        self.retry_delay = QSpinBox(self)
        self.retry_delay.setRange(0, 60)
        self.retry_delay.setSuffix(' s')
        self.book_cooldown = QSpinBox(self)
        self.book_cooldown.setRange(0, 60)
        self.book_cooldown.setSuffix(' s')
        self.socket_timeout = QSpinBox(self)
        self.socket_timeout.setRange(5, 300)
        self.socket_timeout.setSuffix(' s')
        self.debug = QCheckBox('Ativar log de depuração', self)
        self.fetch_metadata = QCheckBox('Buscar metadados de livros carregados manualmente (baixa cada um uma vez ao conectar)', self)
        self.send_to_root = QCheckBox('Enviar para a raiz (ignorar qualquer modelo)', self)
        self.overwrite_existing = QCheckBox('Sobrescrever arquivo se ele já existir no dispositivo', self)

        # Optimizer controls.
        self.optimize = QCheckBox('Otimizar EPUBs antes da transferência', self)
        self.optimize_grayscale = QCheckBox('Converter imagens para tons de cinza', self)
        self.optimize_auto_crop = QCheckBox('Recortar margens uniformes automaticamente', self)
        self.optimize_split = QCheckBox(
            'Dividir capítulos/parágrafos grandes e remover fontes (evita falta de memória)', self)
        self.optimize_quality = QSpinBox(self)
        self.optimize_quality.setRange(1, 100)
        self.optimize_quality.setSuffix('%')
        self.device_target = QComboBox(self)
        self.device_target.addItem('Detectar automaticamente', 'auto')
        self.device_target.addItem('X4 / X4 Pro / X4 Classic / Sticky (480×800)', 'X4')
        self.device_target.addItem('X3 (528×792)', 'X3')

        self.host.setText(PREFS['host'])
        self.port.setValue(PREFS['port'])
        self.path.setText(PREFS['path'])
        self.upload_template.setText(PREFS['upload_template'])
        self.upload_template.setPlaceholderText('Deixe em branco para usar o modelo de envio para dispositivo do Calibre')
        self.chunk_size.setValue(PREFS['chunk_size'])
        self.upload_retries.setValue(PREFS['upload_retries'])
        self.retry_delay.setValue(PREFS['retry_delay'])
        self.book_cooldown.setValue(PREFS['book_cooldown'])
        self.socket_timeout.setValue(PREFS['socket_timeout'])
        self.debug.setChecked(PREFS['debug'])
        self.fetch_metadata.setChecked(PREFS['fetch_metadata'])
        self.send_to_root.setChecked(PREFS['send_to_root'])
        self.overwrite_existing.setChecked(PREFS['overwrite_existing'])
        self.optimize.setChecked(PREFS['optimize'])
        self.optimize_grayscale.setChecked(PREFS['optimize_grayscale'])
        self.optimize_auto_crop.setChecked(PREFS['optimize_auto_crop'])
        self.optimize_split.setChecked(PREFS['optimize_split'])
        self.optimize_quality.setValue(PREFS['optimize_quality'])
        idx = self.device_target.findData(PREFS['device_target'])
        self.device_target.setCurrentIndex(idx if idx >= 0 else 0)

        layout.addRow('Host', self.host)
        layout.addRow('Porta', self.port)

        notice = QLabel('Host e porta são valores de fallback usados quando a descoberta automática por UDP não encontra o dispositivo.')
        notice.setWordWrap(True)
        notice.setStyleSheet('color: gray; font-style: italic;')
        layout.addRow('', notice)

        layout.addRow('Caminho de envio', self.path)
        layout.addRow('Modelo de caminho de envio', self.upload_template)
        layout.addRow('Tamanho do bloco', self.chunk_size)

        reliability_heading = QLabel('<b>Confiabilidade do envio</b>')
        layout.addRow(reliability_heading)
        reliability_notice = QLabel('As novas tentativas reenviam o livro atual desde o início após falhas transitórias de WebSocket.')
        reliability_notice.setWordWrap(True)
        reliability_notice.setStyleSheet('color: gray; font-style: italic;')
        layout.addRow('', reliability_notice)
        layout.addRow('Tentativas de envio', self.upload_retries)
        layout.addRow('Atraso entre tentativas', self.retry_delay)
        layout.addRow('Pausa entre livros', self.book_cooldown)
        layout.addRow('Timeout do socket', self.socket_timeout)

        layout.addRow('', self.debug)
        layout.addRow('', self.fetch_metadata)
        layout.addRow('', self.send_to_root)
        layout.addRow('', self.overwrite_existing)

        sep = QFrame(self)
        sep.setFrameShape(QFrame.Shape.HLine)
        sep.setFrameShadow(QFrame.Shadow.Sunken)
        layout.addRow(sep)

        opt_heading = QLabel('<b>Otimizador</b>')
        layout.addRow(opt_heading)
        opt_notice = QLabel('Semelhante ao otimizador da página web do leitor: redimensiona as imagens para a '
                            'tela, converte para tons de cinza, recompacta como JPEG e '
                            'reescreve o EPUB. Um resumo é mostrado após cada transferência.')
        opt_notice.setWordWrap(True)
        opt_notice.setStyleSheet('color: gray; font-style: italic;')
        layout.addRow('', opt_notice)
        layout.addRow('', self.optimize)
        layout.addRow('Dispositivo alvo', self.device_target)
        layout.addRow('Qualidade JPEG', self.optimize_quality)
        layout.addRow('', self.optimize_grayscale)
        layout.addRow('', self.optimize_auto_crop)
        layout.addRow('', self.optimize_split)

        self.optimize.toggled.connect(self._sync_optimizer_enabled)
        self._sync_optimizer_enabled(self.optimize.isChecked())

        self.log_view = QPlainTextEdit(self)
        self.log_view.setReadOnly(True)
        self.log_view.setPlaceholderText('O log de descoberta aparecerá aqui quando a depuração estiver ativada.')
        self._refresh_logs()

        refresh_btn = QPushButton('Atualizar log', self)
        refresh_btn.clicked.connect(self._refresh_logs)
        log_layout = QHBoxLayout()
        log_layout.addWidget(refresh_btn)

        layout.addRow('Log', self.log_view)
        layout.addRow('', log_layout)

    def save(self):
        PREFS['host'] = self.host.text().strip() or PREFS.defaults['host']
        PREFS['port'] = int(self.port.value())
        PREFS['path'] = self.path.text().strip() or PREFS.defaults['path']
        PREFS['upload_template'] = self.upload_template.text().strip()
        PREFS['chunk_size'] = int(self.chunk_size.value())
        PREFS['upload_retries'] = int(self.upload_retries.value())
        PREFS['retry_delay'] = int(self.retry_delay.value())
        PREFS['book_cooldown'] = int(self.book_cooldown.value())
        PREFS['socket_timeout'] = int(self.socket_timeout.value())
        PREFS['debug'] = bool(self.debug.isChecked())
        PREFS['fetch_metadata'] = bool(self.fetch_metadata.isChecked())
        PREFS['send_to_root'] = bool(self.send_to_root.isChecked())
        PREFS['overwrite_existing'] = bool(self.overwrite_existing.isChecked())
        PREFS['optimize'] = bool(self.optimize.isChecked())
        PREFS['optimize_grayscale'] = bool(self.optimize_grayscale.isChecked())
        PREFS['optimize_auto_crop'] = bool(self.optimize_auto_crop.isChecked())
        PREFS['optimize_split'] = bool(self.optimize_split.isChecked())
        PREFS['optimize_quality'] = int(self.optimize_quality.value())
        PREFS['device_target'] = self.device_target.currentData()

    def _sync_optimizer_enabled(self, enabled):
        for w in (self.optimize_grayscale, self.optimize_auto_crop,
                  self.optimize_split, self.optimize_quality, self.device_target):
            w.setEnabled(enabled)

    def _refresh_logs(self):
        self.log_view.setPlainText(get_log_text())

    def validate(self):
        return True


class FluiDezConfigDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle('FluiDez Reader')
        self.widget = FluiDezConfigWidget()
        layout = QVBoxLayout(self)
        layout.addWidget(self.widget)
        buttons = QDialogButtonBox(QDialogButtonBox.StandardButton.Ok |
                                   QDialogButtonBox.StandardButton.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        layout.addWidget(buttons)
