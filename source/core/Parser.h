//
// Created by hao on 3/6/19.
//

#ifndef PATH_TRACING_PARSER_H
#define PATH_TRACING_PARSER_H

#include "core.h"

struct RenderTask {
    RenderTask() : nx(512), ny(512), ns(32), depth(8), scene_name(""), output_name("output") {
    }

    bool valid() {
        return nx > 0 && ny > 0 && ns > 0 && depth > 0 && scene_name.compare("") != 0 && output_name.compare("") != 0;
    }

    int nx, ny;
    int ns;
    int depth;
    string scene_name;
    string output_name;
    vec3 look_at;
    vec3 look_from;
    float fov;
};

class ArgParser {
public:
    static bool ParseArg(int argc, char **argv, RenderTask &renderTask) {
        if (argc == 2) {
            string setting = readFileIntoString(argv[1]);
            auto j = json::parse(setting);
            renderTask.scene_name = j["scene"].get<std::string>();
            renderTask.output_name = j["output"].get<std::string>();
            renderTask.nx = j["resolution"][0];
            renderTask.ny = j["resolution"][1];
            renderTask.ns = j["resolution"][2];
            renderTask.look_at.x = j["camera"]["lookat"][0];
            renderTask.look_at.y = j["camera"]["lookat"][1];
            renderTask.look_at.z = j["camera"]["lookat"][2];
            renderTask.look_from.x = j["camera"]["lookfrom"][0];
            renderTask.look_from.y = j["camera"]["lookfrom"][1];
            renderTask.look_from.z = j["camera"]["lookfrom"][2];
            renderTask.fov = j["camera"]["fov"];
            if (j.find("depth") != j.end()) {
                renderTask.depth = j["depth"];
            }
        }
        cout << "Input parsed." << endl;
        return renderTask.valid();
    }

    static string readFileIntoString(char *filename) {
        ifstream ifile(filename);
        ostringstream buf;
        char ch;
        while (buf && ifile.get(ch))
            buf.put(ch);
        return buf.str();
    }

};


#endif //PATH_TRACING_PARSER_H
