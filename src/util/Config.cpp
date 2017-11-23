#include "Config.h"
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>
#include <glm/glm.hpp>
#include <fstream>

#include "util/Logger.h"

#include "render/camera.h"

namespace cmcray
{

    namespace
    {

        const char DEF_CONFIG[] = "defconfig.json";
        const char* argv0;

        template<typename T> T fromValue(const rapidjson::Document& doc, const char* key, const T& defaultValue)
        {
            auto valit = doc.FindMember(key);
            if (valit != doc.MemberEnd()) return valit->value.Get<T>();
            else return defaultValue;
        }

        template<>
        glm::vec2 fromValue(const rapidjson::Document& doc, const char* key, const glm::vec2& defaultValue)
        {
            auto valit = doc.FindMember(key);
            if (valit != doc.MemberEnd())
            {
                glm::vec2 retval;
                retval.x = valit->value[0].GetFloat();
                retval.y = valit->value[1].GetFloat();
                return retval;
            }
            else return defaultValue;
        }

        template<>
        std::string fromValue(const rapidjson::Document& doc, const char* key, const std::string& defaultValue)
        {
            auto valit = doc.FindMember(key);
            if (valit != doc.MemberEnd())
            {
                return std::string(valit->value.GetString());
            }
            else return defaultValue;
        }

        template<>
        glm::vec3 fromValue(const rapidjson::Document& doc, const char* key, const glm::vec3& defaultValue)
        {
            auto valit = doc.FindMember(key);
            if (valit != doc.MemberEnd())
            {
                glm::vec3 retval{};
                retval.x = valit->value[0].GetFloat();
                retval.y = valit->value[1].GetFloat();
                retval.z = valit->value[2].GetFloat();
                return retval;
            }
            else return defaultValue;
        }

        template<>
        glm::mat4 fromValue(const rapidjson::Document& doc, const char* key, const glm::mat4& defaultValue)
        {
            auto valit = doc.FindMember(key);
            if(valit != doc.MemberEnd())
            {
                glm::mat4 retval{};
                for(int i = 0; i < 16; ++i)
                {
                    retval[i % 4][i / 4] = valit->value[i].GetFloat();
                }
                return retval;
            }
            else return defaultValue;
        }

        template<typename T>
        rapidjson::Value toValue(const T& val, rapidjson::Document& d)
        {
            rapidjson::Value v;
            v.Set<T>(val);
            return v;
        }

        template<>
        rapidjson::Value toValue(const std::string& val, rapidjson::Document& d)
        {
            rapidjson::Value v(rapidjson::kStringType);
            v.SetString(val.c_str(), val.size(), d.GetAllocator());
            return v;
        }

        template<>
        rapidjson::Value toValue(const glm::vec2& val, rapidjson::Document &d)
        {
            rapidjson::Value v(rapidjson::kArrayType);
            for(int i = 0 ; i < 2; ++i)
            {
                v.PushBack(val[i], d.GetAllocator());
            }
            return v;
        }

        template<>
        rapidjson::Value toValue(const glm::vec3& val, rapidjson::Document &d)
        {
            rapidjson::Value v(rapidjson::kArrayType);
            for(int i = 0; i < 3; ++i)
            {
                v.PushBack(val[i], d.GetAllocator());
            }
            return v;
        }

        template<>
        rapidjson::Value toValue(const glm::mat4& val, rapidjson::Document &d)
        {
            rapidjson::Value v(rapidjson::kArrayType);
            for(int i = 0; i < 16; ++i)
            {
                v.PushBack(val[i % 4][i / 4], d.GetAllocator());
            }
            return v;
        }

    }

    namespace Config
    {

#define OPT(type, name, defValue) type name = defValue;
#include "Config.inl"
#undef OPT

        void loadConfigFromFile(const char* fileName)
        {
            rapidjson::Document d;
            std::ifstream configFile(fileName);
            rapidjson::IStreamWrapper wrappedStream(configFile);
            d.ParseStream(wrappedStream);
            if (d.HasParseError())
            {
                auto errorCode = d.GetParseError();
                switch(errorCode)
                {
                    case rapidjson::kParseErrorNone:
                        // Shouldn't happen
                        break;
                    case rapidjson::kParseErrorDocumentEmpty:
                        if (!strcmp(fileName, DEF_CONFIG))
                        {
                            Log(WARNING) << "Missing or empty " << fileName << ", relying on pre-defined values";
                        }
                        else
                        {
                            Log(FATAL) << "Missing or empty configuration file: " << fileName
                                       << "\n\nUsage: " << argv0 << " [config_1.json] [config_2.json] ..."
                                       << "\n\nConfiguration files are read first to last, values from later files"
                                       << " have higher priority and overwrite values from earlier files";
                        }
                        break;
                    default:
                        Log(WARNING) << "Ignoring corrupted configuration file: " << fileName
                                     << "; error: " << rapidjson::GetParseError_En(errorCode);
                }
                return;
            }

#define OPT(type, name, defValue) name = fromValue(d, #name, name);
#include "Config.inl"

#undef OPT
        }

        void saveConfigToFile(const char* fileName)
        {
            rapidjson::Document d;
            d.SetObject();
            std::ofstream configFile(fileName);
            rapidjson::OStreamWrapper wrappedStream(configFile);
            auto& allocator = d.GetAllocator();
#define OPT(type, name, defValue) d.AddMember(#name, toValue(name, d), allocator);
#include "Config.inl"

#undef OPT
            rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(wrappedStream);
            d.Accept(writer);
        }

        void init(int argc, char** argv)
        {
            argv0 = argv[0];
            Log(INFO) << "Loading initial configuration from " << DEF_CONFIG;
            loadConfigFromFile(DEF_CONFIG);

            for(int i = 1; i < argc; ++i)
            {
                Log(INFO) << "Loading additional parameters from " << argv[i];
                loadConfigFromFile(argv[i]);
            }
            auto &cam = getDefaultCamera();
            cam.fov = cameraFov;
            cam.near = cameraNear;
            cam.far = cameraFar;
            cam.position = cameraPos;
            cam.rotation = cameraRotation;
            cam.targetSize = glm::vec2{screenX, screenY};
        }

    }
}

