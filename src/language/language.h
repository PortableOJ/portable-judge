//
// Created by keqing on 2021-11-11.
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

public:
    Language(const string &name, string lang, string params, const string &codeName,
             const string &extension);

    [[nodiscard]] const string &getName() const;

    [[nodiscard]] const string &getLang() const;

    [[nodiscard]] const string &getParams() const;

    [[nodiscard]] const string &getCodeName() const;

    [[nodiscard]] const string &getExtension() const;

    bool operator==(const Language &rhs) const;

    bool operator!=(const Language &rhs) const;
};

/// region define

unordered_map<string, Language *> Language::languageMap;

Language::Language(const string &name, string lang, string params, const string &codeName, const string &extension)
                   : name(name), lang(std::move(lang)), params(std::move(params)), codeName(codeName), extension(extension) {
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

bool Language::operator==(const Language &rhs) const {
    return name == rhs.name;
}

bool Language::operator!=(const Language &rhs) const {
    return !(rhs == *this);
}

/// endregion

#endif //JUDGE_LANGUAGE_H