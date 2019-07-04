//
// Created by hao on 3/6/19.
//

#ifndef PATH_TRACING_OBJLOADER_H
#define PATH_TRACING_OBJLOADER_H



// Obj loader
#include "core.h"
#include "../shapes/shapes.h"
#include "../materials/materials.h"
#include "../textures/textures.h"

class ObjLoader {
public:
    ObjLoader() = default;


    shared_ptr<Scene> loadObj(const string inputfile) {
        string location = inputfile.substr(0, inputfile.find_last_of('/'));
        string filename;
        if (inputfile.find_last_of('/') != string::npos)
            filename = inputfile.substr(inputfile.find_last_of('/') + 1);
        else
            filename = inputfile;
        cout << "Load obj : " << filename << endl;
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;
        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), location.c_str());
        if (!err.empty()) { // `err` may contain warning message.
            std::cerr << err << std::endl;
        }
        if (!ret) {
            exit(1);
        }

        // Loop over material
        float epsilon = 1e-6;
        vector<shared_ptr<Material>> materialList;
        set<int> sampleMaterialIndex;
        for (size_t m = 0; m < materials.size(); m++) {
            shared_ptr<Material> material;
            string name = materials[m].name;
            // use ka as emittion
//            vec3 ambient = vec3(materials[m].ambient[0], materials[m].ambient[1], materials[m].ambient[2]);
            // For diffuse
            vec3 diffuse = vec3(materials[m].diffuse[0], materials[m].diffuse[1], materials[m].diffuse[2]);
            shared_ptr<Texture> diff_tex = make_shared<ConstantTexture>(diffuse);
            if (materials[m].diffuse_texname.compare("") != 0) {
                cout << "Get diffure map : " << materials[m].diffuse_texname << endl;
                diffuse = vec3(1.0);
                int nx, ny, nn;
                unsigned char *tex_data = stbi_load((location + "/" + materials[m].diffuse_texname).c_str(), &nx, &ny,
                                                    &nn, 0);
                diff_tex = make_shared<ImageTexture>(tex_data, nx, ny, nn);
            }
            // For specular
            vec3 specular = vec3(materials[m].specular[0], materials[m].specular[1], materials[m].specular[2]);
            shared_ptr<Texture> specular_tex = make_shared<ConstantTexture>(specular);
            float ns = materials[m].shininess;
            // For media
            float ior = materials[m].ior;
            // For light
            vec3 emission = vec3(materials[m].emission[0], materials[m].emission[1], materials[m].emission[2]);
            if (length(emission) > epsilon) {
                sampleMaterialIndex.insert(m);
                material = make_shared<DiffuseLight>(make_shared<ConstantTexture>(emission));
            } else {
                if (abs(ior - 1.0) < epsilon) {
                    // Mix
                    if (abs(ns - 1) > 1e-6) {
                        float pDiffuse = length(diffuse);
                        float pSpecular = length(specular);
                        float sum = pDiffuse + pSpecular;
                        pDiffuse /= sum;
                        pSpecular /= sum;

                        material = make_shared<BRDF>(diff_tex, specular_tex,
                                                     pDiffuse, pSpecular, 4.0 / ns);
                    } else {
                        material = make_shared<Lambertian>(diff_tex);
                    }
//                    if (ns > 90000) {
//                        material = make_shared<Metal>(specular);
//                    }
                } else {
                    // media
//                material = make_shared<MicrofacetBSDF>(ior, make_shared<ConstantTexture>(diffuse), 0.4);
                    material = make_shared<Dielectric>(ior, make_shared<ConstantTexture>(diffuse));

                }
            }
            materialList.push_back(material);
        }

        // Save mesh
        vector<shared_ptr<Shape>> meshList;
        vector<shared_ptr<Shape>> triList;
        // Save sample triangle
        vector<shared_ptr<Shape>> sampleList;
        vector<shared_ptr<Shape>> sampleTriList;
        // Loop over shapes
        shared_ptr<Shape> triangle;
        for (size_t s = 0; s < shapes.size(); s++) {
            triList.clear();
            sampleTriList.clear();
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
                // Loop over vertices in the face.
                int fv = shapes[s].mesh.num_face_vertices[f];
                vec3 vertex[3];
                vec3 normal[3];
                vec2 uv[3];
                for (size_t v = 0; v < 3; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                    vertex[v] = vec3(vx, vy, vz);

                    tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                    tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                    tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
                    normal[v] = vec3(nx, ny, nz);

                    tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
                    uv[v] = vec2(tx, ty);
                }
                index_offset += fv;
                // per-face material
                int materialID = shapes[s].mesh.material_ids[f];
                triangle = make_shared<Triangle>(vertex[0], vertex[1], vertex[2],
                                                 normal[0], normal[1], normal[2],
                                                 uv[0], uv[1], uv[2], materialList[materialID]);

                triList.push_back(triangle);
                if (sampleMaterialIndex.find(materialID) != sampleMaterialIndex.end()) {
                    sampleTriList.push_back(triangle);
                }
            }
            if (!triList.empty())
                meshList.push_back(make_shared<Mesh>(triList));
            if (!sampleTriList.empty())
                sampleList.push_back(make_shared<Mesh>(sampleTriList));
        }

        if (filename.compare("room.obj") == 0) {
            shared_ptr<Material> light = make_shared<DiffuseLight>(make_shared<ConstantTexture>(vec3(50, 50, 40)));
            meshList.push_back(make_shared<Sphere>(vec3(0.0, 1.589, -1.274), 0.2, light));
            sampleList.push_back(make_shared<Sphere>(vec3(0.0, 1.589, -1.274), 0.2, light));
        } else if (filename.compare("cup.obj") == 0) {
            shared_ptr<Material> light = make_shared<DiffuseLight>(make_shared<ConstantTexture>(vec3(40, 40, 40)));
            meshList.push_back(make_shared<yz_rect>(vec3(-2.758771896, 1.5246, 0), vec2(1, 1), light));
            sampleList.push_back(make_shared<yz_rect>(vec3(-2.758771896, 1.5246, 0), vec2(1, 1), light));
        } else if (filename.compare("VeachMIS.obj") == 0) {
            shared_ptr<Material> light;
            light = make_shared<DiffuseLight>(make_shared<ConstantTexture>(vec3(800)));
            meshList.push_back(make_shared<Sphere>(vec3(-10, 10, 4), 0.500, light));
            sampleList.push_back(make_shared<Sphere>(vec3(-10, 10, 4), 0.500, light));

            light = make_shared<DiffuseLight>(make_shared<ConstantTexture>(vec3(901.803)));
            meshList.push_back(make_shared<Sphere>(vec3(3.75, 0, 0), 0.033, light));
            sampleList.push_back(make_shared<Sphere>(vec3(3.75, 0, 0), 0.033, light));

            light = make_shared<DiffuseLight>(make_shared<ConstantTexture>(vec3(100)));
            meshList.push_back(make_shared<Sphere>(vec3(1.25, 0, 0), 0.100, light));
            sampleList.push_back(make_shared<Sphere>(vec3(1.25, 0, 0), 0.100, light));

            light = make_shared<DiffuseLight>(make_shared<ConstantTexture>(vec3(11.1111)));
            meshList.push_back(make_shared<Sphere>(vec3(-1.25, 0, 0), 0.300, light));
            sampleList.push_back(make_shared<Sphere>(vec3(-1.25, 0, 0), 0.300, light));

            light = make_shared<DiffuseLight>(make_shared<ConstantTexture>(vec3(1.23457)));
            meshList.push_back(make_shared<Sphere>(vec3(-3.75, 0, 0), 0.900, light));
            sampleList.push_back(make_shared<Sphere>(vec3(-3.75, 0, 0), 0.900, light));
        }
        cout << "Model loaded." << endl;
        return make_shared<Scene>(make_shared<ShapeList>(meshList),
                                  make_shared<ShapeList>(sampleList), nullptr);
    }

};


#endif //PATH_TRACING_OBJLOADER_H

