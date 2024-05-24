#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    gl::maximize_window(); // On peut la maximiser si on veut
    
    auto camera = gl::Camera{};
    //gl::set_events_callbacks({camera.events_callbacks()});

    gl::set_events_callbacks({
        camera.events_callbacks(),
        {
            .on_mouse_pressed = [&](gl::MousePressedEvent const& e) { },
        },
    });



    glEnable(GL_BLEND);
    // On peut configurer l'équation qui mélange deux couleurs, comme pour faire différents blend mode dans Photoshop. 
    // Cette équation-ci donne le blending "normal" entre pixels transparents.
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE); 

    auto const shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/vertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/fragment.glsl"},
    }};

    auto const shaderBackground = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/backgroundVertex.glsl"},
        .fragment = gl::ShaderSource::File{"res/backgroundFragment.glsl"},
    }};

    // auto const triangle_mesh = gl::Mesh{{
    // .vertex_buffers = 
    //     {{
    //     .layout = {gl::VertexAttribute::Position2D{0 /*Index de l'attribut dans le shader, on en reparle juste après*/}},
    //     .data   = 
    //         {
    //         -0.5f, -0.5f, 
    //         +0.5f, -0.5f, 
    //         +0.5f, +0.5f,

    //         -0.5f, -0.5f,
    //         +0.5f, +0.5f,
    //         -0.5f, +0.5f, 
    //         },
    //     }},
    // }};

    auto const rectangle_mesh = gl::Mesh{{
    .vertex_buffers = {{
        .layout = {gl::VertexAttribute::Position2D{0}},
        .data   = {
            -0.5f, -0.5f, // Position2D du 1er sommet
            +0.5f, -0.5f, // Position2D du 2ème sommet
            +0.5f, +0.5f, // Position2D du 3ème sommet
            -0.5f, +0.5f  // Position2D du 4ème sommet
        },
    }},
    .index_buffer   = {
        0, 1, 2, // Indices du premier triangle : on utilise le 1er, 2ème et 3ème sommet
        0, 2, 3  // Indices du deuxième triangle : on utilise le 1er, 3ème et 4ème sommet
    },
    }};

    while (gl::window_is_open())
    {
        glClearColor(0.62f, 0.0f, 1.0f, 1.f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
        glClear(GL_COLOR_BUFFER_BIT); // Exécute concrètement l'action d'appliquer sur tout l'écran la couleur choisie au-dessus
    
        //gl::bind_default_shader(); // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw(). On en reparle dans la section d'après.
        //triangle_mesh.draw(); // C'est ce qu'on appelle un "draw call" : on envoie l'instruction à la carte graphique de dessiner notre mesh.
        
/*
        // BLEND
        shader.bind();
        shader.set_uniform("aspect_ratio",gl::framebuffer_aspect_ratio());
        shader.set_uniform("offsetTime",gl::time_in_seconds());
        shader.set_uniform("squareSize",0.5f);
        rectangle_mesh.draw();

        shaderBackground.bind();
        rectangle_mesh.draw();
*/


        glm::mat4 const view_matrix = camera.view_matrix();
        
        // param1 : field of view in radians
        // param2 : aspect ratio
        // param3 : near plane
        glm::mat4 const projection_matrix = glm::infinitePerspective(1.f, gl::framebuffer_aspect_ratio(), 0.001f);
        //glm::mat4 const projection_matrix = glm::infinitePerspective(0.5f, 2.f, 0.001f);
        //glm::mat4 const projection_matrix = glm::infinitePerspective(2.f, 5.f, 0.5f);
        
        //glm::mat4 const ortho_matrix = glm::ortho(0.f,0.f,0.f,1.f);

        glm::mat4 const view_projection_matrix = projection_matrix * view_matrix;
        //glm::mat4 const view_projection_matrix = ortho_matrix * view_matrix;

        // param1 : mat4{1}
        // param2 : angle de la rotation
        // param3 : axe autour duquel on tourne
        glm::mat4 const rotation = glm::rotate(glm::mat4{1.f}, gl::time_in_seconds(), glm::vec3{0.f, 0.f, 1.f});

        // param1 : mat4{1}
        // param2 : vecteur de déplacement
        glm::mat4 const translation = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 1.f, 0.f});    

        //glm::mat4 const model_view_projection_matrix = translation * rotation * view_projection_matrix;
        glm::mat4 const model_view_projection_matrix = rotation * translation * view_projection_matrix;
        
        shader.bind();
        shader.set_uniform("aspect_ratio",gl::framebuffer_aspect_ratio());
        shader.set_uniform("offsetTime",gl::time_in_seconds());
        shader.set_uniform("squareSize",0.5f);
        shader.set_uniform("matrix",model_view_projection_matrix);
        rectangle_mesh.draw();

    }
}