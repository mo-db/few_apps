#include "libs/sokol/sokol_app.h"
#include "libs/sokol/sokol_gfx.h"
#include "libs/sokol/sokol_glue.h"

// Shader and pipeline state
static sg_pipeline pip;
static sg_bindings bind;
static sg_pass_action pass_action;

// Uniform block
typedef struct {
    float resolution[2];
    float time;
} vs_params_t;

void init(void) {
    sg_setup(&(sg_desc){
        .context = sapp_sgcontext()
    });

    // Create shader
    sg_shader shd = sg_make_shader(&(sg_shader_desc){
        // ... (include vertex shader source)
        .fs.source = /* Your fragment shader source */
    });

    // Create pipeline
    pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = shd,
        .layout = {
            .attrs = {
                [0] = { .format = SG_VERTEXFORMAT_FLOAT3 }
            }
        }
    });

    // Set up vertex buffer (fullscreen quad)
    float vertices[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f };
    bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .data = SG_RANGE(vertices)
    });

    // Initialize pass action to clear the framebuffer
    pass_action = (sg_pass_action) {
        .colors[0] = { .action = SG_ACTION_CLEAR, .value = { 0.0f, 0.0f, 0.0f, 1.0f } }
    };
}

void frame(void) {
    vs_params_t vs_params = {
        .resolution = { sapp_width(), sapp_height() },
        .time = (float)sapp_frame_duration()
    };

    sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());
    sg_apply_pipeline(pip);
    sg_apply_bindings(&bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &SG_RANGE(vs_params));
    sg_draw(0, 4, 1);
    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .width = 800,
        .height = 600,
        .window_title = "Radiance Cascades",
    };
}
