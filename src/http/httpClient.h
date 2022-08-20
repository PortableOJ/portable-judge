//
// Created by 胡柯青 on 2022-08-06.
//

#ifndef JUDGE_HTTPCLIENT_H
#define JUDGE_HTTPCLIENT_H

#include "../env/__init__.h"
#include "httplib.h"
#include "../configor/json.hpp"

using namespace configor;

using httplib::Client;

const int fileReadBufferSize = 1024;

class HttpClient {
private:

    Client client;

    static HttpClient *INSTANCE;

    HttpClient(const string &host, int port);

    static string getUrl(const string &url, const json &params);

public:
    static HttpClient &get();

    static HttpClient &get(const string &host, int port);

    void auth(const string &url, const json &params);

    bool get(const string &url, const json &params, const function<void(const json &)> &callback);

    bool post(const string &url, const json &body, const function<void(const json &)> &callback);

    bool download(const string &url, const json &params, const path &file);

    bool uploadFile(const string &url, const path &file);
};

HttpClient *HttpClient::INSTANCE = nullptr;

string HttpClient::getUrl(const string &url, const json &params) {
    string uri = url;
    for (auto iter = params.begin(); iter != params.end(); iter++) {
        if (iter == params.begin()) uri += '?';
        else uri += '&';
        uri += iter.key() + "=" + iter.value().dump();
    }
    return uri;
}

HttpClient::HttpClient(const string &host, int port) : client(host, port) {}

HttpClient &HttpClient::get() {
    if (INSTANCE == nullptr) {
        Env *env = Env::ctx();
        const string *host = env->getString(constant.serverUrl);
        int port = env->getInt(constant.serverPort);
        INSTANCE = new HttpClient(*host, port);
    }
    return *INSTANCE;
}

HttpClient &HttpClient::get(const string &host, int port) {
    if (INSTANCE == nullptr) {
        INSTANCE = new HttpClient(host, port);
    }
    return *INSTANCE;
}

const string application_json = "application/json";

void HttpClient::auth(const string &url, const json &params) {
    httplib::Headers headers;
    httplib::Result result = client.Post(url, params.dump(), application_json);
    for (auto &item: result->headers) {
        if (item.first == "Set-Cookie") {
            string cookie;
            for (char &c: item.second) {
                if (c == ';') break;
                cookie.push_back(c);
            }
            headers.insert({"Cookie", cookie});
        }
    }
    client.set_default_headers(headers);
}

bool HttpClient::get(const string &url, const json &params, const function<void(const json &)> &callback) {
    string uri = getUrl(url, params);
    httplib::Result result = client.Get(uri);
    if (result->status == 200) {
        json response = json::parse(result->body);
        callback(response);
        return true;
    } else {
        Logger::err("HTTP Get 请求出错, url: %, status: %, response: %", uri, result->status, result->body);
    }
    return false;
}

bool HttpClient::post(const string &url, const json &body, const function<void(const json &)> &callback) {
    httplib::Result result = client.Post(url, body.dump(), application_json);
    if (result->status == 200) {
        json response = json::parse(result->body);
        callback(response);
        return true;
    } else {
        Logger::err("HTTP Post 请求出错, url: %, data: % , status: %, response: %", url, body, result->status,result->body);
    }
    return false;
}

bool HttpClient::download(const string &url, const json &params, const path &file) {
    string uri = getUrl(url, params);
    httplib::Result result = client.Get(uri, [&](const char *data, size_t length) {
        ofstream out(file, std::ios::binary);
        out.write(data, (long) length);
        return true;
    });
    return true;
}

bool HttpClient::uploadFile(const string &url, const path &file) {
    ifstream in(file);
    char data[fileReadBufferSize];
    httplib::Result result = client.Post(url, [&](size_t offset, httplib::DataSink &sink) {
        in.read(data, fileReadBufferSize);
        sink.write(data, fileReadBufferSize);
        return true;
    }, "text/plain");
    return true;
}

#endif //JUDGE_HTTPCLIENT_H
