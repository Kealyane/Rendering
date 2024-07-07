#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "tiny_obj_loader.h"
#include <iostream>
#include <vector>

void computeTangentsAndBitangents(const std::vector<float> &vertices, const std::vector<float> &texcoords,
    const std::vector<unsigned int> &indices, std::vector<float> &tangents, std::vector<float> &bitangents)
{
    tangents.resize(vertices.size());
    bitangents.resize(vertices.size());

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        glm::vec3 pos1(vertices[3 * i0], vertices[3 * i0 + 1], vertices[3 * i0 + 2]);
        glm::vec3 pos2(vertices[3 * i1], vertices[3 * i1 + 1], vertices[3 * i1 + 2]);
        glm::vec3 pos3(vertices[3 * i2], vertices[3 * i2 + 1], vertices[3 * i2 + 2]);

        glm::vec2 uv1(texcoords[2 * i0], texcoords[2 * i0 + 1]);
        glm::vec2 uv2(texcoords[2 * i1], texcoords[2 * i1 + 1]);
        glm::vec2 uv3(texcoords[2 * i2], texcoords[2 * i2 + 1]);

        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent = glm::normalize(tangent);

        glm::vec3 bitangent;
        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent = glm::normalize(bitangent);

        for (unsigned int j : {i0, i1, i2})
        {
            tangents[3 * j] = tangent.x;
            tangents[3 * j + 1] = tangent.y;
            tangents[3 * j + 2] = tangent.z;
            bitangents[3 * j] = bitangent.x;
            bitangents[3 * j + 1] = bitangent.y;
            bitangents[3 * j + 2] = bitangent.z;
        }
    }
}


int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window();        // On peut la maximiser si on veut

    auto camera = gl::Camera{};
    // gl::set_events_callbacks({camera.events_callbacks()});

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    // On peut configurer l'équation qui mélange deux couleurs, comme pour faire différents blend mode dans Photoshop.
    // Cette équation-ci donne le blending "normal" entre pixels transparents.
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);

    // ------ CALLBACK ------------

    gl::set_events_callbacks({
        camera.events_callbacks(),
        {
            .on_mouse_pressed = [&](gl::MousePressedEvent const &e) {},
        },
    });

    // ----- SHADERS ---------

    auto const shader = gl::Shader{{
        .vertex = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
    }};

    auto const shaderBackground = gl::Shader{{
        .vertex = gl::ShaderSource::File{"res/backgroundVertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/backgroundFragment.glsl"},
    }};

    auto const shaderRender = gl::Shader{{
        .vertex = gl::ShaderSource::File{"res/renderVertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/renderFragment.glsl"},
    }};

    auto const shaderMesh = gl::Shader{{
        .vertex = gl::ShaderSource::File{"res/meshVertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/meshFragment.glsl"},
    }};

    // ------ MESHES ----------

    auto const rectangle_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0}, gl::VertexAttribute::UV{1}},
            .data = {
                -1.f, -1.f, 0.f, 0.f, // Position2D du 1er sommet - 0
                +1.f, -1.f, 1.f, 0.f, // Position2D du 2ème sommet - 1
                +1.f, +1.f, 1.f, 1.f, // Position2D du 3ème sommet - 2
                -1.f, +1.f, 0.f, 1.f  // Position2D du 4ème sommet - 3
            },
        }},
        .index_buffer = {
            0, 1, 2, // Indices du premier triangle : on utilise le 1er, 2ème et 3ème sommet
            0, 2, 3  // Indices du deuxième triangle : on utilise le 1er, 3ème et 4ème sommet
        },
    }};

    auto const cube_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position3D{0}, gl::VertexAttribute::UV{1}},
            .data = {
                -0.5f, -0.5f, -0.5f, 0.f, 0.f, // 0
                +0.5f, -0.5f, -0.5f, 1.f, 0.f, // 1
                +0.5f, +0.5f, -0.5f, 1.f, 1.f, // 2
                -0.5f, +0.5f, -0.5f, 0.f, 1.f, // 3
                -0.5f, -0.5f, 0.5f, 0.f, 0.f,  // 4
                +0.5f, -0.5f, 0.5f, 1.f, 0.f,  // 5
                +0.5f, +0.5f, 0.5f, 1.f, 1.f,  // 6
                -0.5f, +0.5f, 0.5f, 0.f, 1.f,  // 7
            },
        }},
        .index_buffer = {
            0, 1, 2, 0, 2, 3, 
            1, 5, 6, 1, 6, 2, 
            5, 4, 7, 5, 7, 6, 
            4, 0, 7, 7, 3, 0, 
            2, 6, 7, 2, 7, 3, 
            1, 5, 4, 1, 4, 0
            },
    }};

    // ------ TEXTURE -----------

    auto const texture = gl::Texture{
        gl::TextureSource::File{
            // Peut être un fichier, ou directement un tableau de pixels
            .path = "res/meshes/fourareen2K_albedo.jpg",
            .flip_y = true,                              // Il n'y a pas de convention universelle sur la direction de l'axe Y. Les fichiers (.png, .jpeg) utilisent souvent une direction différente de celle attendue par OpenGL. Ce booléen flip_y est là pour inverser la texture si jamais elle n'apparaît pas dans le bon sens.
            .texture_format = gl::InternalFormat::RGBA8, // Format dans lequel la texture sera stockée. On pourrait par exemple utiliser RGBA16 si on voulait 16 bits par canal de couleur au lieu de 8. (Mais ça ne sert à rien dans notre cas car notre fichier ne contient que 8 bits par canal, donc on ne gagnerait pas de précision). On pourrait aussi stocker en RGB8 si on ne voulait pas de canal alpha. On utilise aussi parfois des textures avec un seul canal (R8) pour des usages spécifiques.
        },
        gl::TextureOptions{
            .minification_filter = gl::Filter::Linear,  // Comment on va moyenner les pixels quand on voit l'image de loin ?
            .magnification_filter = gl::Filter::Linear, // Comment on va interpoler entre les pixels quand on zoom dans l'image ?
            .wrap_x = gl::Wrap::Repeat,                 // Quelle couleur va-t-on lire si jamais on essaye de lire en dehors de la texture ?
            .wrap_y = gl::Wrap::Repeat,                 // Idem, mais sur l'axe Y. En général on met le même wrap mode sur les deux axes.
        }
    };

    auto const normal_map = gl::Texture{
        gl::TextureSource::File{
            .path = "res/meshes/fourareen_normalMap.png", 
            .flip_y = true,
            .texture_format = gl::InternalFormat::RGBA8, 
        },
        gl::TextureOptions{
            .minification_filter = gl::Filter::Linear,
            .magnification_filter = gl::Filter::Linear, 
            .wrap_x = gl::Wrap::Repeat, 
            .wrap_y = gl::Wrap::Repeat,
        }
    };    

    // ------ RENDER TARGET --------

    auto render_target = gl::RenderTarget{gl::RenderTarget_Descriptor{
        .width = gl::framebuffer_width_in_pixels(),
        .height = gl::framebuffer_height_in_pixels(),
        .color_textures = {
            gl::ColorAttachment_Descriptor{
                .format = gl::InternalFormat_Color::RGBA8,
                .options = {
                    .minification_filter = gl::Filter::NearestNeighbour,  // On va toujours afficher la texture à la taille de l'écran,
                    .magnification_filter = gl::Filter::NearestNeighbour, // donc les filtres n'auront pas d'effet. Tant qu'à faire on choisit le moins coûteux.
                    .wrap_x = gl::Wrap::ClampToEdge,
                    .wrap_y = gl::Wrap::ClampToEdge,
                },
            },
        },
        .depth_stencil_texture = gl::DepthStencilAttachment_Descriptor{
            .format = gl::InternalFormat_DepthStencil::Depth32F,
            .options = {
                .minification_filter = gl::Filter::NearestNeighbour,
                .magnification_filter = gl::Filter::NearestNeighbour,
                .wrap_x = gl::Wrap::ClampToEdge,
                .wrap_y = gl::Wrap::ClampToEdge,
            },
        },
    }};

    // ------ CALLBACK --------

    gl::set_events_callbacks({
        camera.events_callbacks(),
        {.on_framebuffer_resized = [&](gl::FramebufferResizedEvent const &e)
         {
             if (e.width_in_pixels != 0 && e.height_in_pixels != 0) // OpenGL crash si on tente de faire une render target avec une taille de 0
                 render_target.resize(e.width_in_pixels, e.height_in_pixels);
         }},
    });

    // ------ LOAD 3D MESH -------------

    auto objPath = gl::make_absolute_path("res/meshes/fourareen.obj");
    std::string filePath = objPath.generic_string();

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(filePath))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        return -1;
    }

    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();
    auto &materials = reader.GetMaterials();

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texcoords;
    std::vector<unsigned int> indices;

    for (const auto &shape : shapes)
    {
        for (const auto &index : shape.mesh.indices)
        {
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            vertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            if (!attrib.normals.empty())
            {
                normals.push_back(attrib.normals[3 * index.normal_index + 0]);
                normals.push_back(attrib.normals[3 * index.normal_index + 1]);
                normals.push_back(attrib.normals[3 * index.normal_index + 2]);
            }

            if (!attrib.texcoords.empty())
            {
                texcoords.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                texcoords.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
            }

            indices.push_back(indices.size());
        }
    }

    std::vector<float> tangents;
    std::vector<float> bitangents;

    computeTangentsAndBitangents(vertices, texcoords, indices, tangents, bitangents);

    auto const mesh3D = gl::Mesh{
        {
        .vertex_buffers = {
        {
            .layout = {gl::VertexAttribute::Position3D{0}},
            .data = vertices,
        },
        {
            .layout = {gl::VertexAttribute::UV{1}},
            .data = texcoords,
        },
        {
             .layout = {gl::VertexAttribute::Normal3D{2}},
             .data = normals,
        },
        {
            .layout = {gl::VertexAttribute::Position3D{3}},
             .data = tangents,
        },
        {
            .layout = {gl::VertexAttribute::Position3D{4}},
             .data = bitangents,
        },
        },
    }
    };


    // ---- BOUCLE WHILE -------

    while (gl::window_is_open())
    {
        render_target.render([&]()
            {
                glClearColor(0.2f, 0.2f, 0.2f, 1.f); // Dessine du rouge, non pas à l'écran, mais sur notre render target
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glm::mat4 const view_matrix = camera.view_matrix();

                // param1 : field of view in radians
                // param2 : aspect ratio
                // param3 : near plane
                glm::mat4 const projection_matrix = glm::infinitePerspective(1.f, gl::framebuffer_aspect_ratio(), 0.001f);
                // glm::mat4 const projection_matrix = glm::infinitePerspective(0.5f, 2.f, 0.001f);
                // glm::mat4 const projection_matrix = glm::infinitePerspective(2.f, 5.f, 0.5f);

                // glm::mat4 const ortho_matrix = glm::ortho(0.f,0.f,0.f,1.f);

                glm::mat4 const view_projection_matrix = projection_matrix * view_matrix;
                // glm::mat4 const view_projection_matrix = ortho_matrix * view_matrix;

                // param1 : mat4{1}
                // param2 : angle de la rotation
                // param3 : axe autour duquel on tourne
                glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, gl::time_in_seconds(), glm::vec3{1.f, 0.f, 0.f});

                // // param1 : mat4{1}
                // // param2 : vecteur de déplacement
                // glm::mat4 const translation = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 1.f, 0.f});

                // glm::mat4 const model_view_projection_matrix = translation * rotation * view_projection_matrix;
                glm::mat4 const model_view_projection_matrix = view_projection_matrix * rotation;

                glm::mat4 const normal_matrix = glm::inverse(glm::transpose(model_view_projection_matrix));
                /*
                shader.bind();
                shader.set_uniform("aspect_ratio",gl::framebuffer_aspect_ratio());
                shader.set_uniform("offsetTime",gl::time_in_seconds());
                shader.set_uniform("squareSize",0.5f);

                shader.set_uniform("matrix",view_projection_matrix);
                shader.set_uniform("my_texture",texture);

                cube_mesh.draw();
                */
                glm::vec3 light_direction = normalize(glm::vec3(0.2, 0.3, -1.));
                glm::vec3 colorDirectionalLight = glm::vec3(0.0, 0.0, 1.0);

                glm::vec3 point_light = normalize(glm::vec3(0.5, 0.8, 1.));
                glm::vec3 colorPointLight = glm::vec3(0.0, 1.0, 0.0);

                glm::vec3 colorAmbiantLight = glm::vec3(1.0, 0.0, 0.0);

                shader.bind();
                shader.set_uniform("model_matrix", model_view_projection_matrix);
                shader.set_uniform("normal_matrix", normal_matrix);

                shader.set_uniform("my_texture", texture);
                shader.set_uniform("my_normal_map", normal_map);

                shader.set_uniform("light_direction", light_direction);
                shader.set_uniform("colorDirectionalLight", colorDirectionalLight);

                shader.set_uniform("point_light", point_light);
                shader.set_uniform("colorPointLight", colorPointLight);

                shader.set_uniform("colorAmbiantLight", colorAmbiantLight);

                mesh3D.draw();
            });

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderRender.bind();
        shaderRender.set_uniform("textureCube", render_target.color_texture(0));
        rectangle_mesh.draw();

        
    }
}