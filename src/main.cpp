#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "tiny_obj_loader.h"
#include <iostream>
#include <vector>

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
        }};

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

    
    auto const mesh3D = gl::Mesh{{
        .vertex_buffers = {{
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
        },
    }};

    // GLuint vao, vbo, ebo;
    // glGenVertexArrays(1, &vao);
    // glGenBuffers(1, &vbo);
    // glGenBuffers(1, &ebo);

    // glBindVertexArray(vao);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // // Positions
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    // // Normales
    // if (!normals.empty())
    // {
    //     GLuint normalVBO;
    //     glGenBuffers(1, &normalVBO);
    //     glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    //     glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    //     glEnableVertexAttribArray(1);
    //     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    // }

    // // Coordonnées de texture
    // if (!texcoords.empty())
    // {
    //     GLuint texcoordVBO;
    //     glGenBuffers(1, &texcoordVBO);
    //     glBindBuffer(GL_ARRAY_BUFFER, texcoordVBO);
    //     glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), texcoords.data(), GL_STATIC_DRAW);
    //     glEnableVertexAttribArray(2);
    //     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    // }

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);


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
                // glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, gl::time_in_seconds(), glm::vec3{0.f, 0.f, 1.f});

                // // param1 : mat4{1}
                // // param2 : vecteur de déplacement
                // glm::mat4 const translation = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 1.f, 0.f});

                // glm::mat4 const model_view_projection_matrix = translation * rotation * view_projection_matrix;
                glm::mat4 const model_view_projection_matrix = view_projection_matrix;

                /*
                shader.bind();
                shader.set_uniform("aspect_ratio",gl::framebuffer_aspect_ratio());
                shader.set_uniform("offsetTime",gl::time_in_seconds());
                shader.set_uniform("squareSize",0.5f);

                shader.set_uniform("matrix",view_projection_matrix);
                shader.set_uniform("my_texture",texture);

                cube_mesh.draw();
                */

                shader.bind();
                shader.set_uniform("matrix", model_view_projection_matrix);
                shader.set_uniform("my_texture", texture);
                mesh3D.draw();
            });

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderRender.bind();
        shaderRender.set_uniform("textureCube", render_target.color_texture(0));
        rectangle_mesh.draw();

        
    }
}