//
// Created by 胡柯青 on 2021-11-11.
//

#ifndef JUDGE_LANGUAGE_H
#define JUDGE_LANGUAGE_H

#include <utility>

#include "../util/__init__.h"

class Language {
private:
    static unordered_map<string, Language *> languageMap;

    const string name{};
    const string lang{};
    const string params{};
    const string codeName{};
    const string extension{};
    const string runningExtension{};

public:

    const static Language *getLanguage(const string &name);

    Language(const string &name, string lang, string params, string codeName,
             string extension, string runningExtension);

    [[nodiscard]] const string &getName() const;

    [[nodiscard]] const string &getLang() const;

    [[nodiscard]] const string &getParams() const;

    [[nodiscard]] const string &getCodeName() const;

    [[nodiscard]] const string &getExtension() const;

    [[nodiscard]] const string &getRunningExtension() const;

    bool operator==(const Language &rhs) const;

    bool operator!=(const Language &rhs) const;
};

/// region define

unordered_map<string, Language *> Language::languageMap;

const Language *Language::getLanguage(const string &name) {
    auto iter = languageMap.find(name);
    if (iter != languageMap.end()) return iter->second;
    return nullptr;
}

Language::Language(const string &name, string lang, string params, string codeName, string extension,
                   string runningExtension)
        : name(name), lang(move(lang)), params(move(params)), codeName(move(codeName)),
          extension(move(extension)), runningExtension(move(runningExtension)) {
    languageMap.insert({name, this});
}

const string &Language::getName() const {
    return name;
}

const string &Language::getLang() const {
    return lang;
}

const string &Language::getParams() const {
    return params;
}

const string &Language::getCodeName() const {
    return codeName;
}

const string &Language::getExtension() const {
    return extension;
}

const string &Language::getRunningExtension() const {
    return runningExtension;
}

bool Language::operator==(const Language &rhs) const {
    return name == rhs.name;
}

bool Language::operator!=(const Language &rhs) const {
    return !(rhs == *this);
}


/// endregion

#endif //JUDGE_LANGUAGE_H
