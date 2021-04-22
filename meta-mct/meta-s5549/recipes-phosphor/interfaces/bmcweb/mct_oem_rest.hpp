#pragma once
#include <app.h>
#include <tinyxml2.h>

#include <cstdio>
#include <async_resp.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <dbus_singleton.hpp>
#include <dbus_utility.hpp>
#include <filesystem>
#include <fstream>
#include <memory>
#include <regex>
#include <sdbusplus/message/types.hpp>
#include <sys/stat.h>


namespace crow
{
namespace mct_oem_rest
{

using GetSubTreeType = std::vector<
    std::pair<std::string,
              std::vector<std::pair<std::string, std::vector<std::string>>>>>;

const char *notFoundMsg = "404 Not Found";
const char *badReqMsg = "400 Bad Request";
const char *methodNotAllowedMsg = "405 Method Not Allowed";
const char *forbiddenMsg = "403 Forbidden";
const char *methodFailedMsg = "500 Method Call Failed";
const char *methodOutputFailedMsg = "500 Method Output Error";
const char *notFoundDesc =
    "org.freedesktop.DBus.Error.FileNotFound: path or object not found";
const char *propNotFoundDesc = "The specified property cannot be found";
const char *noJsonDesc = "No JSON object could be decoded";
const char *methodNotFoundDesc = "The specified method cannot be found";
const char *methodNotAllowedDesc = "Method not allowed";
const char *forbiddenPropDesc = "The specified property cannot be created";
const char *forbiddenResDesc = "The specified resource cannot be created";

std::unique_ptr<sdbusplus::bus::match::match> fwUpdateMatcher;

inline void solLogHandler(const crow::Request& req, crow::Response& res)
{
    std::filesystem::path loc("/var/log/obmc-console.log");
    std::ifstream readFile(loc);

    if (!std::filesystem::exists(loc) || std::filesystem::is_empty(loc) ||
        !std::filesystem::is_regular_file(loc))
    {
        BMCWEB_LOG_ERROR << loc << "SOL data Not found";
        res.result(boost::beast::http::status::not_found);
        res.end();
        return;
    }

    if (readFile.good())
    {
        res.addHeader("Content-Type", "application/octet-stream");
        res.body() = {std::istreambuf_iterator<char>(readFile),
                      std::istreambuf_iterator<char>()};
        res.end();
        return;
    }
    else
    {
        res.result(boost::beast::http::status::not_found);
        res.end();
        return;
    }
}

inline void crashdumpsHandler(const crow::Request& req, crow::Response& res)
{
    std::vector<std::string> crashdumpslist;
    nlohmann::json crashdumpslistData;
    std::string path = "/var/lib/crashdump/output";

    if(std::filesystem::exists(std::filesystem::path(path))){
        for (const auto & entry : std::filesystem::directory_iterator(path))
        {
            if(entry.path().filename().string().find("json",0) == std::string::npos)
            {
                continue;
            }
            std::ifstream ifs(entry.path().string());
            nlohmann::json crashdumpsData = nlohmann::json::parse(ifs);
            crashdumpslist.push_back(entry.path().filename().string());
            crashdumpslistData.push_back({entry.path().filename().string(),crashdumpsData});
        }
    }

    res.addHeader("Content-Type", "application/json");

    res.jsonValue = {{"status", "ok"},
                     {"message", "200 OK"},
                     {"list", std::move(crashdumpslist)},
                     {"data", std::move(crashdumpslistData)}
                     };
    res.end();
}

inline void requestRoutes(App& app)
{
    BMCWEB_ROUTE(app, "/sol.log")
        .privileges({"ConfigureManager"})
        .methods(boost::beast::http::verb::get)(
            [](const crow::Request &req, crow::Response &res) {
            solLogHandler(req, res);
        });

    BMCWEB_ROUTE(app, "/crashdumps")
        .privileges({"Login"})
        .methods(boost::beast::http::verb::get)(
            [](const crow::Request &req, crow::Response &res) {
            crashdumpsHandler(req, res);
        });
}
} // namespace mct_oem_rest
} // namespace crow