#include "base_config.h"
#include <ArduinoJson.h>
#include <sodium.h>
#include <fstream>
#include "husarnet_config.h"
#include "licensing.h"
#include "util.h"

static const unsigned char* PUBLIC_KEY = reinterpret_cast<const unsigned char*>(
    "\x2a\x3f\x26\x7c\x2a\x68\xa6\x0f\x66\xf6\xaf\x2b\x0a\x42\x7b\x25"
    "\xb5\x30\x7c\x23\x47\x80\x2d\xdf\x35\x24\xf4\x9a\xfe\x7d\x01\xe5");

static std::string getSignatureData(const DynamicJsonDocument& doc) {
  std::string s;
  s.append("1\n");
  s.append(doc["installation_id"].as<std::string>() + "\n");
  s.append(doc["license_id"].as<std::string>() + "\n");
  s.append(doc["name"].as<std::string>() + "\n");
  s.append(doc["max_devices"].as<std::string>() + "\n");
  s.append(doc["dashboard_url"].as<std::string>() + "\n");
  s.append(doc["websetup_host"].as<std::string>() + "\n");
  for (auto address : doc["base_server_addresses"].as<JsonArray>()) {
    s.append(address.as<std::string>() + ",");
  }
  s[s.size() - 1] = '\n';  // remove the last comma
  s.append(doc["issued"].as<std::string>() + "\n");
  s.append(doc["valid_until"].as<std::string>());
  return s;
}

static void verifySignature(const std::string& signature,
                            const std::string& data) {
  auto* signaturePtr = reinterpret_cast<const unsigned char*>(signature.data());
  auto* dataPtr = reinterpret_cast<const unsigned char*>(data.data());
  if (crypto_sign_ed25519_verify_detached(signaturePtr, dataPtr, data.size(),
                                          PUBLIC_KEY) != 0) {
    LOG("license file is invalid");
    abort();
  }
}

BaseConfig::BaseConfig(const std::string& licenseFile) {
  DynamicJsonDocument doc(4096);
  deserializeJson(doc, licenseFile);

  std::string signature = base64Decode(doc["signature"].as<std::string>());
  std::string data = getSignatureData(doc);
  verifySignature(signature, data);

  auto baseTcpAddressesC = doc["base_server_addresses"].as<JsonArray>();
  for (auto address : baseTcpAddressesC) {
    this->baseTcpAddresses.push_back(
        {IpAddress::parse(address.as<std::string>()), 443});
  }
  this->dashboardUrl = doc["dashboard_url"].as<std::string>();
  this->baseDnsAddress = "";
  this->defaultJoinHost = doc["websetup_host"].as<std::string>();
  this->defaultWebsetupHosts.push_back(this->defaultJoinHost);
}

BaseConfig* BaseConfig::create(const std::string configDir) {
  BaseConfig* config;
  auto licenseFilePath = configDir + "license.json";
  std::ifstream input(licenseFilePath);
  if (input.is_open()) {
    std::string str((std::istreambuf_iterator<char>(input)),
                    std::istreambuf_iterator<char>());
    config = new BaseConfig(str);
  } else {
    LOG("license not found locally, will get default license online...");
    IpAddress ip = OsSocket::resolveToIp(::dashboardHostname);
    InetAddress address{ip, 80};
    std::string license = requestLicense(address);

    std::ofstream f(licenseFilePath);
    if (!f.good()) {
      LOG("failed to write: %s - have you tried running husarnet in elevated "
          "command prompt?",
          configDir.c_str());
      exit(1);
    }
    f << license;
    f.close();

    config = new BaseConfig(license);
  }

  return config;
}

bool BaseConfig::isDefault() const {
  return this->dashboardUrl == ::dashboardUrl;
}

const std::vector<InetAddress>& BaseConfig::getBaseTcpAddresses() const {
  return baseTcpAddresses;
}

const std::string& BaseConfig::getDashboardUrl() const {
  return dashboardUrl;
}

const std::string& BaseConfig::getBaseDnsAddress() const {
  return baseDnsAddress;
}

const std::vector<std::string>& BaseConfig::getDefaultWebsetupHosts() const {
  return defaultWebsetupHosts;
}

const std::string& BaseConfig::getDefaultJoinHost() const {
  return defaultJoinHost;
}
