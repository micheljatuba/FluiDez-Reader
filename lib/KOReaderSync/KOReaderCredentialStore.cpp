#include "KOReaderCredentialStore.h"

#include <Logging.h>
#include <MD5Builder.h>
#include <ObfuscationUtils.h>

namespace {
// FluiDez has no default sync server: syncing starts once the user enters the
// URL of a KOReader-compatible server (for example https://sync.koreader.rocks:443).

// Implicit server of configs saved before config version 2 with an empty serverUrl.
constexpr char LEGACY_DEFAULT_SERVER_URL[] = "https://sync.koreader.rocks:443";

// CrossPoint's public sync server: the implicit server of version 2 configs
// (CrossInk, and FluiDez up to v1.6-fluidez18), and the only server that accepts
// the extended position object.
constexpr char CROSSPOINT_SYNC_SERVER_URL[] = "https://sync.crosspointreader.com";

// Bumped when a change to defaults would alter behavior for existing configs.
constexpr uint8_t CONFIG_VERSION = 3;
}  // namespace

void KOReaderCredentialStore::toJson(JsonDocument& doc) const {
  doc["cfgVersion"] = CONFIG_VERSION;
  // Serialize fields directly: public getters lazy-load and saveToFile() already
  // holds the store mutex. Calling a getter here could recursively load while locked.
  doc["username"] = username;
  doc["password_obf"] = obfuscation::obfuscateToBase64(password);
  doc["serverUrl"] = serverUrl;
  doc["matchMethod"] = static_cast<uint8_t>(matchMethod);
  doc["sendMetadata"] = sendMetadata;
  doc["syncBehavior"] = static_cast<uint8_t>(syncBehavior);
}

bool KOReaderCredentialStore::fromJson(JsonVariantConst doc) {
  std::string user = doc["username"] | "";

  bool needsResave = false;
  obfuscation::DecodeStatus status = obfuscation::DecodeStatus::INVALID;
  std::string pass = obfuscation::deobfuscateFromBase64(doc["password_obf"] | "", &status);
  if (status == obfuscation::DecodeStatus::LEGACY && !pass.empty()) {
    needsResave = true;
  }
  if (status == obfuscation::DecodeStatus::INVALID || status == obfuscation::DecodeStatus::EMPTY || pass.empty()) {
    pass = doc["password"] | "";
    if (!pass.empty()) needsResave = true;
  }
  if (status == obfuscation::DecodeStatus::INVALID && pass.empty()) {
    LOG_ERR("KRS", "Ignoring unreadable KOReader password");
  }

  setCredentials(user, pass);
  setServerUrl(doc["serverUrl"] | "");

  // A config with credentials and no explicit URL was syncing against the
  // implicit server of its version: pin that URL so upgrading never switches
  // or stops the user's sync. Fresh setups have no server until one is entered.
  const uint8_t cfgVersion = doc["cfgVersion"] | (uint8_t)1;
  if (cfgVersion < CONFIG_VERSION) {
    const bool hadAccount = !username.empty() && !password.empty();
    if (serverUrl.empty() && hadAccount) {
      const char* implicitServer = cfgVersion < 2 ? LEGACY_DEFAULT_SERVER_URL : CROSSPOINT_SYNC_SERVER_URL;
      LOG_DBG("KRS", "Config v%u used an implicit server; pinning %s", cfgVersion, implicitServer);
      setServerUrl(implicitServer);
    }
    needsResave = true;  // stamp cfgVersion so this migration runs once
  }

  uint8_t method = doc["matchMethod"] | (uint8_t)0;
  if (method <= static_cast<uint8_t>(DocumentMatchMethod::BINARY)) {
    setMatchMethod(static_cast<DocumentMatchMethod>(method));
  } else {
    LOG_DBG("KRS", "Invalid matchMethod %u in JSON, resetting to FILENAME", method);
    setMatchMethod(DocumentMatchMethod::FILENAME);
  }
  setSendMetadata(doc["sendMetadata"] | false);

  const JsonVariantConst behaviorValue = doc["syncBehavior"];
  const bool missingBehavior = behaviorValue.isNull();
  uint8_t behavior = behaviorValue | static_cast<uint8_t>(KOReaderSyncBehavior::ASK_EVERY_TIME);
  if (behavior <= static_cast<uint8_t>(KOReaderSyncBehavior::SMART)) {
    setSyncBehavior(static_cast<KOReaderSyncBehavior>(behavior));
    needsResave = needsResave || missingBehavior;
  } else {
    LOG_DBG("KRS", "Invalid syncBehavior %u in JSON, resetting to ASK_EVERY_TIME", behavior);
    setSyncBehavior(KOReaderSyncBehavior::ASK_EVERY_TIME);
    needsResave = true;
  }

  if (needsResave) {
    LOG_DBG("KRS", "Resaving KOReader credentials to update format");
    requestResave();
  }

  return true;
}

void KOReaderCredentialStore::setCredentials(const std::string& user, const std::string& pass) {
  ensureLoaded();
  username = user;
  password = pass;
}

std::string KOReaderCredentialStore::getMd5Password() const {
  ensureLoaded();
  if (password.empty()) {
    return "";
  }

  // Calculate MD5 hash of password using ESP32's MD5Builder
  MD5Builder md5;
  md5.begin();
  md5.add(password.c_str());
  md5.calculate();

  return md5.toString().c_str();
}

bool KOReaderCredentialStore::hasCredentials() const {
  ensureLoaded();
  // Without a server there is nothing to sync with, so the account is unusable.
  return !username.empty() && !password.empty() && !serverUrl.empty();
}

void KOReaderCredentialStore::setServerUrl(const std::string& url) {
  ensureLoaded();
  serverUrl = url;
}

std::string KOReaderCredentialStore::getBaseUrl() const {
  ensureLoaded();
  std::string url;
  if (serverUrl.empty()) {
    return url;  // No server configured.
  } else if (serverUrl.find("://") == std::string::npos) {
    // Normalize URL: add http:// if no protocol specified (local servers typically don't have SSL)
    url = "http://" + serverUrl;
  } else {
    url = serverUrl;
  }

  // Strip trailing slashes to avoid double-slash in API paths
  while (!url.empty() && url.back() == '/') {
    url.pop_back();
  }

  return url;
}

bool KOReaderCredentialStore::usesCrossPointSyncServer() const { return getBaseUrl() == CROSSPOINT_SYNC_SERVER_URL; }

void KOReaderCredentialStore::setMatchMethod(DocumentMatchMethod method) {
  ensureLoaded();
  matchMethod = method;
}

void KOReaderCredentialStore::setSendMetadata(bool enabled) {
  ensureLoaded();
  sendMetadata = enabled;
}

void KOReaderCredentialStore::setSyncBehavior(KOReaderSyncBehavior behavior) {
  ensureLoaded();
  if (static_cast<uint8_t>(behavior) > static_cast<uint8_t>(KOReaderSyncBehavior::SMART)) {
    behavior = KOReaderSyncBehavior::ASK_EVERY_TIME;
  }
  syncBehavior = behavior;
}
